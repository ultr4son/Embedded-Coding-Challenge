/*
 * author: iancain
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <memory.h>
#include <memory_pool.h>

// PRIVATE: declared inside *.c file
typedef struct memory_pool_block_header
{
    uint32_t magic;      // NODE_MAGIC = 0xBAADA555. error checking
    size_t size;
    bool inuse;      // true = currently allocated. Used for error checking

    struct memory_pool_block_header * next;

} memory_pool_block_header_t;

struct memory_pool {
    size_t count;         // total elements
    size_t block_size;   // size of each block
    size_t available;

    struct memory_pool_block_header * pool;
    void ** shadow; // shadow copy of nodes to free on destroy even if caller/user still has them in acquired state
};

//---
// MACROS
//

// HTODB = header to data block
//     converts header pointer to container data block
//
#define MEMORY_POOL_HTODB(_header_, _block_size_) ((void *)_header_ - _block_size_)

// DBTOH = data block to header
//     convert data block pointer to point to embedded header information block
//
#define MEMORY_POOL_DBTOH(_data_block_, _block_size_) ((memory_pool_block_header_t *)(_data_block_ + _block_size_))

// magic value to check for data corruption
#define NODE_MAGIC 0xBAADA555

memory_pool_t * memory_pool_init(size_t count, size_t block_size)
{
    memory_pool_t *mp = NULL;
    void * block = NULL;
    int n = 0;

    // allocate memory pool struct. give ownership back to caller
    mp = (memory_pool_t*) malloc (sizeof(memory_pool_t));
    if( mp == NULL ) {
        printf("ERROR: memory_pool_init: unable to malloc memory_pool_t. OOM\n");
        return NULL;
    }

    for( n = 0; n < count; ++n ) {
        // allocate data block
        //   data block size + header size
        //
        size_t total_size = block_size + sizeof(memory_pool_block_header_t);

		block = malloc(total_size);

        // move to end of data block to create header
        //

		memory_pool_block_header_t* header = (block + block_size);

		header->magic = NODE_MAGIC;
		header->inuse = true;
		header->size = sizeof(memory_pool_block_header_t);
		

        // add to stack (just a simple stack)
		header->next = mp->pool;
		mp->pool = header;


        printf("MEMORY_POOL: i=%d, data=%p, header=%p, block_size=%zu, next=%p\n",
               n, block, header, header->size, header->next);
    }

    printf("memory_pool_init(mp=%p, count=%zu, block_size=%zu)\n", mp, count, block_size);

    mp->count = count;
    mp->block_size = block_size;
    mp->available = count;
	mp->shadow = &mp->pool;

    return n == count ? mp : NULL;
}

bool memory_pool_destroy(memory_pool_t *mp)
{

    printf("memory_pool_destroy(mp = %p, count=%zu, block_size=%zu)\n", mp, mp->count, mp->block_size);

	struct memory_pool_block_header* current = *mp->shadow;

	// free all data blocks from pool
	for(int n = 0; n < mp->count; ++n ) {
		struct memory_pool_block_header* next = current->next;
		
		free(MEMORY_POOL_HTODB(current, mp->block_size));
		
		current = next;
    }
	 
    // free memory pool itself
	free(mp);

    return true;
}

void * memory_pool_acquire(memory_pool_t * mp)
{
	if (mp->available > 0) {
		struct memory_pool_block_header* header = mp->pool;
		mp->pool = mp->pool->next;

		// get data block from header
		void* data = MEMORY_POOL_HTODB(header, mp->block_size);

		mp->available--;

		printf("memory_pool_acquire: mp=%p, data=%p\n", mp, data);
		return data;  // return to caller
	}
	else {
		return NULL;
	}
}

bool memory_pool_release(memory_pool_t *mp, void * data)
{
    // move to header inside memory block using MEMORY_POOL_DBTOH(data, mp->block_size);
	memory_pool_block_header_t* header = MEMORY_POOL_DBTOH(data, mp->block_size);


    printf("memory_pool_release: data=%p, header=%p, block_size=%zu, next=%p\n",
           data, header, header->size, header->next);

    // push on stack
	// avoid double release
	if (mp->pool != header) {
		header->next = mp->pool;
		mp->pool = header;
		mp->available++;
	}

    return true;
}

size_t memory_pool_available(memory_pool_t *mp)
{
    if( mp == NULL ) {
        printf("ERROR: memory_pool_available: memory pool invalid\n");
        return 0;
    }
    return mp->available;
}

void memory_pool_dump(memory_pool_t *mp)
{
    if( mp == NULL ) {
        printf("ERROR: memory_pool_dump: memory pool invalid\n");
        return;
    }

    printf("memory_pool_dump(mp = %p, count=%zu, available=%zu, block_size=%zu)\n",
            mp, mp->count, mp->available, mp->block_size);

    memory_pool_block_header_t * header = mp->pool;

    for(int n = 0; n < mp->available; ++n ) {
        void * data_block = MEMORY_POOL_HTODB(header, mp->block_size);
        printf(" + block: i=%d, data=%p, header=%p, inuse=%s, block_size=%zu, next=%p\n",
               n, data_block, header, header->inuse ? "TRUE":"FALSE", header->size, header->next);

        header = header->next;
    }
}
