////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      mem.c , libmem.so*, Makefile, mem.h, mem.o, tests file
// Semester:         CS 354 SPRING 2019
//
// Author:           Niharika Tomar
// Email:            ntomar@wisc.edu
// CS Login:         tomar
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct block_header {
    int size_status;
    /*
     * Size of the block is always a multiple of 8.
     * Size is stored in all block headers and free block footers.
     *
     * Status is stored only in headers using the two least significant bits.
     *   Bit0 => least significant bit, last bit
     *   Bit0 == 0 => free block
     *   Bit0 == 1 => allocated block
     *
     *   Bit1 => second last bit
     *   Bit1 == 0 => previous block is free
     *   Bit1 == 1 => previous block is allocated
     *
     * End Mark:
     *  The end of the available memory is indicated using a size_status of 1.
     *
     * Examples:
     *
     * 1. Allocated block of size 24 bytes:
     *    Header:
     *      If the previous block is allocated, size_status should be 27
     *      If the previous block is free, size_status should be 25
     *
     * 2. Free block of size 24 bytes:
     *    Header:
     *      If the previous block is allocated, size_status should be 26
     *      If the previous block is free, size_status should be 24
     *    Footer:
     *      size_status should be 24
     */
} block_header;

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */

block_header *start_block = NULL;

/*
 * Helper method to get the size of the data to be stored in multiples of 8
 */
int getSize(int size){
    int pload = size;
    int header = 4;
    int padding = 0;
    if((pload + header)% 8){
        padding = 8 - ((pload + header)% 8);
    }
    return size + padding + header;
}

/*
 * Helper method to get remove the header size from the data size
 */
int removeHeader(int initSize){
    return initSize - initSize % 4;
}

/*
 * Helper method to get to the address of the footer
 */
block_header* getCurFooter(block_header *ptr, int curSize){
    return (block_header*)((char*)ptr + curSize - 4);
}

/*
 * Helper method to get the address of the previous footer
 */
block_header* getPrevFooter(block_header *ptr){
    return (block_header*)((void*)ptr - 4);
}

/*
 * Helper method to get to the address of the previous header
 */
block_header* getPrevHeader(block_header *ptr, int prevSize){
    return (block_header*)((void*)ptr - prevSize);
}

/*
 * Helper method to get to the address of the next header
 */
block_header* getNextHeader(block_header *ptr, int curSize){
    return (block_header*)((void*)ptr + curSize);
}

/*
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use BEST-FIT PLACEMENT POLICY to find the block closest to the required block size
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic.
 * Ex:  while(!(end of alloc mem)){
 *   if(cur block is the best fit){
 *      set best pointer to current
 *      update current to point to next header
 *  }
 *  else if(current mem chunk size < mem needed){
 *  //do nothing as end of loop takes care of that case
 *  }
 *  else{
 *      spliting occurs
 *      update the header and footer
 *  }
 *  if(next block == available/not filled){
 *   if(almost the mem needed) {
 *      set best pointer to current
 *      update headers and footer
 *   }
 *  }
 * }
 * update blocks and return
 */
void* Alloc_Mem(int size) {
    int memChunk = 0;
    block_header *check = start_block;
    while(check->size_status != 1){ //checking if we did not reach end of memory
        int temp = removeHeader(check->size_status); //getting data w/o header
        memChunk = memChunk + temp;
        check = getNextHeader(check,temp); //updating check (size-checker)
    }
    
    //checking size validity
    if(size <= 0 || size >= memChunk-8){
        fprintf(stderr, "Error:mem.c: Block request could not be fulfilled\n");
        return NULL; //returning NULL
    }
    
    //initialisation
    int a;
    int p;
    int bfCheck = 0;
    int bfSize = 0;
    //getting size of data including header and padding size
    int dataSize = getSize(size);
    block_header *current = (block_header*)start_block; //initialising current
    int curSize;
    //getting a huge chunk of memory required for BF heap allocation
    long int chunkAllocator = 987654321;
    block_header *best = NULL; //best fit block
    block_header *split = (block_header*)start_block;
    block_header *footer = (block_header*)start_block;
    block_header *next = (block_header*)start_block;
    
    //finding the best-fit location
    while(current->size_status != 1){
        curSize = removeHeader(current->size_status);
        a = current->size_status % 2;
        p  = (current->size_status % 4) - a;
        if(!a){ //checking availability of current block
            if(curSize == dataSize){ //condition check
                bfCheck = 1;
                best = current; //updating best
                next = getNextHeader(current,curSize); //updating next
                break; //exit loop
            } else if(curSize < dataSize){
                //do nothing as end of loop takes care of that case
            } else{
                //spliting occurs
                int chunkDiff = curSize - dataSize;
                //if next block empty
                if(chunkAllocator > chunkDiff){ //condition check
                    //update values
                    chunkAllocator = chunkDiff;
                    best = current;
                    bfSize = curSize;
                    split = getNextHeader(best,dataSize);
                    footer = getCurFooter(best,curSize);
                }
            }
        }
        current = getNextHeader(current,curSize); //updating current to point to next block
    }
    
    if(best == NULL){ //If no best-fit size of mem available
        fprintf(stderr, "Error:mem.c: Fail. Block request could not be fulfilled\n");
        return NULL;
    }
    
    //p bit check for setting up the bf block
    if(!p){
        best->size_status = dataSize + 1; //binary value 01
    } else{
       best->size_status = dataSize + 3; //binary value 11
    }
    
    //split check
    if(!bfCheck){
        //updating split with header
        split->size_status = bfSize - dataSize + 2;
        //updating split with footer
        footer->size_status = bfSize - dataSize + 2;
    } else{
        next->size_status = next->size_status + 2; //updating next
    }
    return (int*)best + 1; //returning best fit block
}

/*
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 * Ex: if(!(prev mem block available)){
 *   coalesce curr and prev blocks
 * } else{
 *   free the current block of mem
 * }
 * if(!(next block is available)){
 *   coalesce current and next
 * }
 * if all else fails, return 0
 */
int Free_Mem(void *ptr) {
    // returning -1 if ptr is NULL or not a multiple of 8
    if(ptr == NULL || ((int)ptr) % 8 != 0){ //checking size validity
        fprintf(stderr, "Error:mem.c: Block request could not be fulfilled\n");
        return -1;
    }
    //getting the ending location
    block_header *end = (block_header*)ptr -1;
    while(end->size_status != 1){
        int temp = removeHeader(end->size_status); //removing header size from data
        end = getNextHeader(end,temp);//updating next header
    }
    
    //initialising
    block_header *free = (block_header*)ptr -1;
    block_header *prevFooter = getPrevFooter(free);
    int prevSize = prevFooter->size_status - 2;
    
    //checking if input ptr is outside the specified area
    if((int)ptr > (int)end||(int)ptr < (int)start_block){
        fprintf(stderr, "Error:mem.c: Block request is out of specified heap space\n");
        return -1; // Return -1 if ptr is outside of the heap space
    }
    
    //getting previous header
    block_header *prevHeader = getPrevHeader(free,prevSize);
    //getting current size
    int curSize = removeHeader(free->size_status);
    //getting current footer
    block_header *footer = getCurFooter(free,curSize);
    //getting next header
    block_header *nextHeader = getNextHeader(free,curSize);
    
    int coalescePrev = 0;
    if(free->size_status % 2 != 1){
        fprintf(stderr, "Error:mem.c: Block is already freed\n");
        return -1; //Return -1 if ptr block is already freed
    }
    int p = (free->size_status % 4) - 1; //p bits update
    if(p){
        //if no coalescing needed
        free->size_status = free->size_status - 1;
        //setting header-footer size
        footer->size_status = curSize + 2;
        nextHeader->size_status = nextHeader->size_status - 2;
    }
    else{
        //coalescing needed with prev mem bock check
        int finalSize = prevSize + curSize;
        //setting header-footer size
        prevHeader->size_status =  finalSize + 2;
        footer->size_status =  finalSize + 2;
        nextHeader->size_status = nextHeader->size_status - 2;
        coalescePrev = 1;
    }
    //coalescing needed with next mem block check
    if(nextHeader->size_status % 2 != 1){ //coalesce
        block_header *current = free;
        if(coalescePrev){
            current = prevHeader; //updating current
        }
        //updating
        curSize = current->size_status -2;
        int nextSize = nextHeader->size_status;
        int  finalNextSize = curSize + nextSize;
        current->size_status =  finalNextSize + 2; //updating current size
        block_header *coalescedFooter = getCurFooter(current, finalNextSize);
        coalescedFooter->size_status =  finalNextSize + 2;
    }
    return 0;
}

/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */
int Init_Mem(int sizeOfRegion) {
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    block_header* end;
    static int allocated_once = 0;
    
    if (0 != allocated_once) {
        fprintf(stderr,
                "Error:mem.c: Init_Mem has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }
    
    // Get the pagesize
    pagesize = getpagesize();
    
    // Calculate padsize as the padding required to round up sizeOfRegion
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;
    
    alloc_size = sizeOfRegion + padsize;
    
    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE,
                     fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
    
    allocated_once = 1;
    
    // for double word alignment and end mark
    alloc_size -= 8;
    
    // To begin with there is only one big free block
    // initialize heap so that start block meets
    // double word alignement requirement
    start_block = (block_header*) space_ptr + 1;
    end = (block_header*)((void*)start_block + alloc_size);
    
    // Setting up the header
    start_block->size_status = alloc_size;
    
    // Marking the previous block as used
    start_block->size_status += 2;
    
    // Setting up the end mark and marking it as used
    end->size_status = 1;
    
    // Setting up the footer
    block_header *footer = (block_header*) ((char*)start_block + alloc_size - 4);
    footer->size_status = alloc_size;
    
    return 0;
}

/*
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts)
 * t_End    : address of the last byte in the block
 * t_Size   : size of the block as stored in the block header
 */
void Dump_Mem() {
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;
    
    block_header *current = start_block;
    counter = 1;
    
    int used_size = 0;
    int free_size = 0;
    int is_used = -1;
    
    fprintf(stdout, "************************************Block list***\
            ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
            --------------------------------\n");
    
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
        
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }
        
        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }
        
        if (is_used)
            used_size += t_size;
        else
            free_size += t_size;
        
        t_end = t_begin + t_size - 1;
        
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status,
                p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
        
        current = (block_header*)((char*)current + t_size);
        counter = counter + 1;
    }
    
    fprintf(stdout, "---------------------------------------------------\
            ------------------------------\n");
    fprintf(stdout, "***************************************************\
            ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
            ******************************\n");
    fflush(stdout);
    
    return;
}
