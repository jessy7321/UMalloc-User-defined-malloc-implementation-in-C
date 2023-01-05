#ifndef umalloc_h_
#define umalloc_h_

#define malloc(x) umalloc(x, __FILE__ , __LINE__ )
#define free(x) ufree(x, __FILE__ , __LINE__ )

#define MAX_INT (10*1024*1024) // 10 MB

static char mem[MAX_INT] = {0};

typedef struct _block {
    struct _block* prev;
    struct _block* next;
    size_t size;
} Block;

static char init = 0;

#endif
