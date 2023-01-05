# UMalloc-User-defined-malloc-implementation-in-C

<img width="401" alt="image" src="https://user-images.githubusercontent.com/67055118/210855234-8d9257e9-531f-48a8-807e-1f496a39be9a.png">

**1.0 STRUCTURE OF MEMORY**<br>
On initialization, the memory has two nodes to begin with. One at the beginning of the memory, and another at the end of the memory. Both of them are blocks containing only the metadata and no data is allocated. So, their size is 0. They are used as references to the starting and ending of the memory.
Initially, the first node’s previous pointer is a NULL and the next pointer to the last node. For the last node, the next pointer is NULL and previous pointer is the first node.
After initialization, the first block actually allocated by umalloc() will follow the first node and the pointers are updated accordingly. Those blocks will have a non-zero data size.

**2.0 SUMMARY OF APPROACH**<br>
For our implementation, we chose to use a Doubly Linked List. This way, we could keep track of the addresses of blocks in both directions. Each block in the memory is partitioned into two parts: metadata and data. The metadata holds the pointers to the previous and next nodes, and the size of the data. The first and last nodes have only metadata for reasons indicated above.
When umalloc() is called, to allocate a block of data, it traverses through the whole doubly linked list to see if it can find a hole that is large enough to allocate the requested size. This is first-fit approach If found, it will add a “struct Block” node to the first metadata node if it’s the first allocation, or as the next node of those previously allocated. So we have metadata and data, followed by another metadata and data, and so on. Now it is first-fit with segmentation. With the pointer to the userspace it returns, data can be allocated. 
When ufree() is called, it gets the address to the starting of the block (where the metadata starts), and removes that node. The pointers are updated accordingly. If the freed node was between two nodes, this creates a free space between the two blocks. Later, that hole can be used to umalloc() new node with size less than or equal to that freed node.

**3.0	 PRIMARY FUNCTIONS**<br>
**_void * umalloc(size_t size, char * file, int line);_**<br>
Firstly, initialize the memory the first time umalloc() is called. The variable init, is first set to 0, and then set to 1, when initialize() is called. Each time umalloc() is called, it checks if the memory is initialized. After that, it finds a pointer to a free space that is large enough to fit the requested size by calling find_predecessor_of_next_free(). Then adds a new node to the memory and returns a pointer to the starting address of the user space. In case a free space is not found, it returns NULL. umalloc() also returns NULL is free space not found, as no memory allocated. 

_**void ufree(void * curr_bloc, char * file, int line);**_<br>
A function to free the blocks of memory allocated. Given a block’s userspace address, determine the starting address of the metadata. Remove that node using remove_node().


**4.0 ERROR DETECTION**<br>
_**Errors on umalloc()**_<br>
There are errors on umalloc() for the following cases, where a block is not allocated:
	When checking for a free space:
	(total_free_space calculates the sum of all the free chunks in the memory.)
A.	total_free_space < sizeof(Block)
In our code, each block requires a minimum of 24bytes for metadata. So for a new allocation, we need at least 24B. If even that is not available, we can say that the memory is full.
B.	total_free_space >= requested_size
There is free memory available. But due to internal fragmentation, there is no ‘one’ block that is large enough to allocate the requested_size.
C.	total_free_space < requested_size
Memory is not full, but not enough to allocate requested_size
When checking for size:
If the requested_size is not within the limits of 0 to 10MB, then that size would not make any sense.

_**Errors on ufree()**_<br>
There are errors on ufree() for the following cases, where a block is not allocated:
A.	Error occurs when we are looking to remove a node that has never been allocated by umalloc().
B.	Error occurs when we are looking to free a pointer that has already been freed.

**5.0 TESTING INSTRUCTIONS**<br>
make clean
make all
./memgrind
