#include "paging.h"
#include "types.h"

// pageTable_t page_table[ONEKB] __attribute__((aligned(FOURKB)));
// pageDirectory_t page_directory[ONEKB] __attribute__((aligned(FOURKB)));
uint32_t page_table[ONEKB] __attribute__((aligned(FOURKB)));
uint32_t page_directory[ONEKB] __attribute__((aligned(FOURKB)));

/*
This function initializes our page directory and page table.
Inputs: none
Outputs: none
*/
void paging_init()
{
    unsigned int x;
    for(x=0; x<ONEKB; x++){
        // uint32_t ind = x*FOURKB;
        page_table[x] = 0; // sets page table entry to zero
        page_directory[x] = PDEDEFAULT; // sets every pde to default
    }
    page_directory[0] = ((uint32_t)page_table) | P; // set first index to address of page table
    page_directory[1] = PAGESPACE; // set second index to address that allocates a 4mb page
    page_directory[2] = ((uint32_t)page_table) | P; //or with 3 sets present and rw to 1
    page_directory[3] = ((uint32_t)page_table) | P;
    page_directory[4] = ((uint32_t)page_table) | P;
    page_table[RAND] = VIDADD | P;
    page_table[PGINDONE] = ADDYONE + P;
    page_table[PGINDTWO] = ADDYTWO + P;
    page_table[PGINDTHREE] = ADDYTHREE + P;


    asm volatile(
        "movl %0, %%eax    \n\
        movl %%eax, %%cr3     \n\
        movl %%cr4, %%eax     \n\
        orl $0x00000010, %%eax    \n\
        movl %%eax, %%cr4     \n\
        movl %%cr0, %%eax     \n\
        orl $0x80000000, %%eax    \n\
        movl %%eax, %%cr0"
        :                     /* no outputs */
        : "r"(page_directory) /* input */
        : "%eax"              /* clobbered register */
    );

    // Flush TLB
    asm volatile("movl %%cr3, %%eax \n\
                   movl %%eax, %%cr3 \n\
                   " ::
                     : "memory");
}
