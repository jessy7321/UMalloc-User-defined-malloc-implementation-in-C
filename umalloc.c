#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umalloc.h"

/*We are implementing using a Doubly Linked List ---> |First,....blk1,..blk2,...blkN,.....Last| --> aka LL with dummy nodes*/

// function to initialize memory |h(x),user(x),.....free space....,h(y),user(y)|
// At the beginning, two uninitialized nodes exist, one at the beginning and another at the end of the memory.
void initialize() {               
  Block* first = (Block*) mem;    
  Block* last = (Block*) (mem + sizeof(mem) - sizeof(Block));   
  first->prev = NULL;             
  first->next = last;             
  last->prev = first;             
  last->next = NULL;              
  first->size = last->size = 0;   //nothing is allocated yet, so size = 0.
  init = 1;                       //marks that the memory is initialized
}


//function to return pointer to the user space of the given block
void* user_space(Block* blk) {              
  return ((void*) blk) + sizeof(Block);     
}


//function the return pointer to the metadata space of a block, given the userspace address
Block* block_ptr(void* usr_addr) {          
  return (void*) usr_addr - sizeof(Block); 
}


//function to return a pointer to the end of a block
Block* end_of(Block* blk) {                
  return user_space(blk) + blk->size;       
}


// function to return the amount of free space after a block
// free_space(x) = h(y) - end(user(x))
size_t free_space_after(Block* blk) {
  void* next = blk->next; 
  void* current_block_end = (void*) (end_of(blk));
  return next - current_block_end;          
}


//function to check if we can allocate next block and return a starting pointer to the free space if true
//if there is a hole where amount of free space is >= what we need, return a pointer to the starting of that free space
Block* find_predecessor_of_next_free(size_t requested_size) {
  size_t total_block_size = sizeof(Block) + requested_size;       
  size_t total_free_space = 0;          
  for (Block* blk = (Block*) mem; blk->next != NULL; blk = blk->next) {     
    if (free_space_after(blk) >= total_block_size) {         //FIRST FIT IMPLEMENTATION 
      return blk;                                                          
    }                                                                       
  }
  if(total_free_space >= total_block_size){
     printf("There is enough free memory, but there is no block large enough for the requested allocation\n");
  }
  else if (sizeof(Block) < total_free_space && total_free_space < total_block_size){
     printf("Memory not full but there is not enough free memory for allocation of requested size\n");
  }
  else if (total_free_space < sizeof (Block)){
      printf("There is no free memory.\n");
  }

      return NULL;     //cannot allocate
}


//function to add a next node after a given node (let given node be called 'pred')
//create new_node and update the pointers
Block* add_node_after(Block* pred, size_t size) {      
  Block* next = pred->next;                            
  Block* new_node = end_of(pred);                      
  new_node->size = size;                              
  new_node->prev = pred;                              
  new_node->next = next;                               

  pred->next = new_node;                               
  next->prev = new_node;                               
  return new_node;                                     
}


//function to remove a specified node (call is 'blk')
//The next pointer of block before blk, will now point to the next of what blk's next was pointing to.
void remove_node(Block* blk, char* file, int line) {
  int blk_in_mem_flag = 0;
  for (Block* curr = (Block*) mem; curr->next != NULL && curr != blk; curr = curr->next) {
      if (curr->next == blk) {
          blk_in_mem_flag = 1;
          break;
      }
  }
  if (!blk_in_mem_flag){
    printf ("Error on free() in line %d of file %s: Block Address %p not allocated by malloc()\n", line, file, blk);
    return;
  }

  if (blk->next && blk->prev){
    Block* prev = blk->prev;
    Block* next = blk->next;

    blk->next = blk->prev = NULL;
    prev->next = next;
    next->prev = prev;    
  }
  else {
    printf ("Error on free() in line %d of file %s: Block Address %p is already freed\n", line, file, blk);
  }                
}



// Implementation of umalloc()
// 1. Initializes memory       2. Finds free space       3. Returns starting address of free space and adds new node if found, otherwise NULL
void* umalloc(size_t requested_size, char* file, int line) {
  if (!init) {                                                          
    initialize();                                                       
  }

  if (requested_size <= 0 || requested_size > MAX_INT) {
      printf ("Error on malloc() in line %d of file %s: Block of requested size %ld bytes makes no sense.\n", line, file, requested_size);
      return NULL;
  }

  Block* pred = find_predecessor_of_next_free(requested_size);          
  if (pred == NULL) {           
    printf ("Error on malloc() in line %d of file %s: Block of requested size %ld bytes not found.\n", line, file, requested_size);
    return NULL;                                                       
  }
  Block* new_node = add_node_after(pred, requested_size);              
  return user_space(new_node);                                          
}


// Implementation of ufree()
// get the starting address of the block and remove
void ufree(void* usr_addr, char* file, int line) {                      
  Block* p = block_ptr(usr_addr);                            
  remove_node(p, file, line);
}