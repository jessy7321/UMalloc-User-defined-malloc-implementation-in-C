#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "umalloc.c"

/* TestCase 0. Consistency*/
int checkConsistency() {
  int random_size = rand() % 10; // Selects random size between 1 to 10B
//  printf("Random Size : %d\n", random_size);
  char* ptr1 = (char*) malloc(random_size);
  for (int i = 0; i < random_size; i++)
    ptr1[i] = i + 'A';
  free (ptr1);
//  printf("String variable 1 contains %s and is at address %p\n", ptr1, ptr1);

  char* ptr2 = (char*) malloc(random_size);
  for (int i = 0; i < random_size; i++) {
    ptr2[i] = i + 'A';
  }
//  printf("String variable 2 contains %s and is at address %p\n", ptr2, ptr2);
  free (ptr2);

  return (ptr1 == ptr2);
}

/* TestCase 1. Maximization (Simple Coalescence) */
int checkMaximization() {
  int size = 1;
  void* result = malloc(size);
  while (result) {
    free (result);
    result = malloc(size *= 2);
  }
  if ((result = malloc(size /= 2))) {
    free (result);
    return 1;
  }
  return 0;
}

int allocateAndFreeIfPossible(size_t size) {
  void* result = malloc(size);
  if (result == NULL)
      return 0;
  free(result);
  return 1;
}

// Fn. to find max allocation space value
size_t findMaxAllocation() {
  for (size_t max_size = 1; ; max_size *= 2)
    if (!allocateAndFreeIfPossible(max_size))
      return max_size / 2;
}

/* TestCase 2. Basic Coalescence */
int checkBasicCoalescence() {
  size_t max_size = findMaxAllocation();
  size_t half_of_max = max_size / 2, quarter_of_max = max_size / 4;
  return !(!allocateAndFreeIfPossible(half_of_max) || !allocateAndFreeIfPossible(quarter_of_max));
}

#define MAX_LARGE 9216
#define MAX_SMALL 1048576  // 10*1024*1024 - (9216*1024) = 1048576 [1b for 1048576 times is super slow]
struct _SR {
  void* _large[MAX_LARGE];
  void* _small[MAX_SMALL];
  int countSmall;
} saturationRecord;

size_t getSaturationPoint() {
  // Memory Saturation
  size_t _1KB = 1024;
  for (int i = 0; i < MAX_LARGE; ++i) {
    void* r = malloc(_1KB);
    if (r == NULL) {
      printf("checkIntermediateCoalescence(): Failed at 1kb block of %d\n", i);
      return -1;
    }
    saturationRecord._large[i] = r;
  }

  for (int j = 0; j < MAX_SMALL; ++j) {
    void* r = malloc(1);
    if (r == NULL) {
      saturationRecord.countSmall = j;
      printf ("Memory Saturation Complete\n");
      return (MAX_LARGE * _1KB) + j;
    }
    saturationRecord._small[j] = r;
  }
  printf("Decide higher number of 1b blocks\n");
  return -1;
}

// TestCase 4. Time Overhead
void getTimeOverhead () {
    free (saturationRecord._small[saturationRecord.countSmall-1]);
    clock_t start = clock();
    saturationRecord._small[saturationRecord.countSmall-1] = malloc(1);
    clock_t end = clock();
    printf("Time Overhead: %.1lf micro-second(s)\n", 1000*(double)(end-start)/CLOCKS_PER_SEC);
}

// Fn. to free Saturated Memory
void freeSaturationMem() {
  for (int k = 0; k < MAX_LARGE; ++k)
    free(saturationRecord._large[k]);
  for (int k = 0; k < saturationRecord.countSmall; ++k)
    free(saturationRecord._small[k]);
}

/* TestCase 5. Intermediate Coalescence */
int checkIntermediateCoalescence() {
  size_t maxSize = findMaxAllocation();
  getSaturationPoint();
  getTimeOverhead();
  freeSaturationMem();
  return allocateAndFreeIfPossible(maxSize);
}

// main test function
int main(int argc, char** argv) {

  printf("Consistency Check: %s\n",              (checkConsistency()             ? "Passed" : "Failed"));
  printf("Maximization Check: %s\n",             (checkMaximization()            ? "Passed" : "Failed"));
  printf("Basic Coalescence Check: %s\n",        (checkBasicCoalescence()        ? "Passed" : "Failed"));
  printf("Intermediate Coalescence Check: %s\n", (checkIntermediateCoalescence() ? "Passed" : "Failed"));

  return 0;
}
