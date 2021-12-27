#include "scheduling.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "system_call.h"
#include "types.h"
#include "idt_assembly_linkage.h"

#define P_FREQ 1193180
#define SET_HZ 50
#define COMM_REG 0x36
#define COMM_REG_1 0x43
#define MASK_1 0xFF
#define EIGHT 8
#define CHAN_Z 0x40
#define PIT_IRQ 0
#define MAX_SHELL_COUNT 2


/*
This function initiates the pit. It takes no parameters. 
It sets up the frequency of the pit interrupt. 
It calculates the divisor and sets low and high bytes.
*/
// cited from osdev: http://www.osdever.net/bkerndev/Docs/pit.htm
void pit_init(void){
    cli();

    enable_irq(PIT_IRQ); 
    uint32_t divisor = P_FREQ / SET_HZ; // calculate divisor
    outb(COMM_REG, COMM_REG_1); // set comm byte
    outb(MASK_1 & divisor, CHAN_Z); // setlow 
    outb(divisor >> EIGHT, CHAN_Z); // sethigh

    sti();

    return;
}


/*
It gets called by the pit device. It gets the prev_pcb and gets the previous esp and ebp.
It then puts that in an array that acts like a queue of processes. Then it stores the old esp ebp.
It then calls another get_pcb and switchs context to esp ebp. This is the scheduler algorithm.
*/
void pit_handler(void){

    send_eoi(0);
    cli();

    // pcb_t* curr_pcb = get_pcb(process_count);

    // uint32_t esp, ebp;
    // // move esp and ebp into pcb struct
    // asm volatile("movl %%esp, %0;"
    //              "movl %%ebp, %1;"
    //              : "=r"(esp), "=r"(ebp));

    if(shell_count <= MAX_SHELL_COUNT){ // call three separate shells for three different terminals
        shell_count += 1;
        // printf("shell count: %u\n", shell_count);
        execute((uint8_t *)"shell");
    } 


    sti();

    return;
}
