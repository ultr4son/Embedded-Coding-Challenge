/*
 * author: iancain
 * date: 10/1/2018
 * description:
 * Objective: speed operations of malloc/free and adapt idiomatically and separate memory
             management from other data storage patterns such as linked lists, stacks,
             double buffering
   Limitations: Fixed sized memory blocks. Due to the O(1) requirement only fixed sized
                memory allocation can be performed. Memory fragmentation and
                collection/collating operations are not desired due to performance demands

  Support O(1) operation in acquire and release operations
  Strategy:
    stack object to manage memory blocks
       acquire = pop_front  (acquire block off the front/top of stack)
       release = push_back  (release block by putting on back/bottom of stack)
 */


#include <stdlib.h>
#include <stdbool.h>   // NOTE: c99 bool requires #include <stdbool.h>

// Extra Credit: why is this typecast here but declared inside the *.c file?
// Having the typedef in the .h file hides implementation details (size and layout) of the struct and allows for having multiple implmentations of the same interface defined in the .h file.

// Extra Extra Credit: explain in detail how this is using Opaque data type?
// An opaque data type is one whose exact structure is not defined in its interface (in this case a header file).
// This enforces that the memory_pool type is "private" to an external developer because they do not have complete information of the type's structure.

// Extra Extra Extra Credit: declaration vs definition
// A declaration is a description of what the compiler should expect to be implemented by the program. 
// Declarations allow for the developer to create interfaces to different aspects of the program, as the developer is now required by the compiler to create the accompanying definition code to the declaration code.
// Definitions are implementations of the declared identifiers.

typedef struct memory_pool memory_pool_t;

memory_pool_t * memory_pool_init(size_t count, size_t block_size);
bool memory_pool_destroy(memory_pool_t *mp);

void * memory_pool_acquire(memory_pool_t *mp);
bool memory_pool_release(memory_pool_t *mp, void * data);

// convieneince functions
size_t memory_pool_available(memory_pool_t *mp);
void memory_pool_dump(memory_pool_t *mp);
