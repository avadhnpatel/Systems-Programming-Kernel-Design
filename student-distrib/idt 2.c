#include "idt.h"
#include "idt_assembly_linkage.h"
#include "i8259.h"
#include "x86_desc.h"
#include "tests.h"
#include "lib.h"
#include "system_call.h"


#define NUM_OF_INTERRUPTS 41
#define USER_PRIORITY_LEVEL 3
#define KERNEL_PRIORITY_LEVEL 0
#define PIT_INT 32
#define KEYBOARD_INT 33
#define RTC_INT 40

/*
this function initialzies the interrupt descriptor table
there is no input or return value
*/
void idt_init(void)
{
    int i;
    for (i = 0; i < NUM_OF_INTERRUPTS; i++)
    {
        // uint16_t offset_15_00;
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        idt[i].reserved3 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].reserved0 = 0;
        idt[i].size = 1;
        idt[i].dpl = KERNEL_PRIORITY_LEVEL;
        idt[i].present = 1;
        // uint16_t offset_31_16;
    }

    // 16bit or 32bit interrupt gate depending on devices
    // for(i = 32; i < 41; i++){
    //     // uint16_t offset_15_00;
    //     idt[i].seg_selector = KERNEL_CS;
    //     idt[i].reserved4 = 0;
    //     idt[i].reserved3 = 0;
    //     idt[i].reserved2 = 1;
    //     idt[i].reserved1 = 1;
    //     idt[i].reserved0 = 0;
    //     idt[i].size = 1;
    //     idt[i].dpl = 0;
    //     idt[i].present = 1;
    //     // uint16_t offset_31_16;
    // }

    // privilege level for user
    idt[SYS_CALL].seg_selector = KERNEL_CS;
    idt[SYS_CALL].reserved4 = 0;
    idt[SYS_CALL].reserved3 = 1; // watch for this
    idt[SYS_CALL].reserved2 = 1;
    idt[SYS_CALL].reserved1 = 1;
    idt[SYS_CALL].reserved0 = 0;
    idt[SYS_CALL].size = 1;
    idt[SYS_CALL].dpl = USER_PRIORITY_LEVEL; // kernel vs user
    idt[SYS_CALL].present = 1;

    // using set_idt_entry, the idt is being loaded into the table
    SET_IDT_ENTRY(idt[0], &divide_error_linkage);
    SET_IDT_ENTRY(idt[1], &debug_linkage);
    SET_IDT_ENTRY(idt[2], &NMI_int_linkage);
    SET_IDT_ENTRY(idt[3], &breakpoint_linkage);
    SET_IDT_ENTRY(idt[4], &overflow_linkage);
    SET_IDT_ENTRY(idt[5], &bound_exceeded_linkage);
    SET_IDT_ENTRY(idt[6], &invalid_opcode_linkage);
    SET_IDT_ENTRY(idt[7], &device_unavailable_linkage);
    SET_IDT_ENTRY(idt[8], &double_fault_linkage);
    SET_IDT_ENTRY(idt[9], &coprocessor_overun_linkage);
    SET_IDT_ENTRY(idt[10], &invalid_tss_linkage);
    SET_IDT_ENTRY(idt[11], &no_segment_linkage);
    SET_IDT_ENTRY(idt[12], &stack_seg_fault_linkage);
    SET_IDT_ENTRY(idt[13], &gen_protection_linkage);
    SET_IDT_ENTRY(idt[14], &page_fault_linkage);               //  DONT NEED ANYTHING 0x14 - 0x1F
    SET_IDT_ENTRY(idt[15], &reserved_linkage);
    SET_IDT_ENTRY(idt[16], &math_fault_linkage);
    SET_IDT_ENTRY(idt[17], &alignment_check_linkage);
    SET_IDT_ENTRY(idt[18], &machine_check_linkage);
    SET_IDT_ENTRY(idt[19], &float_point_ex_linkage);

    SET_IDT_ENTRY(idt[PIT_INT], &pit_linkage);     // IRQ 0
    SET_IDT_ENTRY(idt[KEYBOARD_INT], &keyboard_linkage); // IRQ 1
    SET_IDT_ENTRY(idt[RTC_INT], &rtc_linkage);           // IRQ 8
    SET_IDT_ENTRY(idt[SYS_CALL], &system_call_linkage);
}

// HANDLERS

/* this function handles the exception for divide_error 
   there is no input our output */
void divide_error(void)
{
    clear();
    printf("Divide Error");
    while (1)
        ;
}

/* this function handles the exception for reserved 
   there is no input our output */
void reserved(void)
{
    clear();
    printf("RESERVED Error");
    while (1)
        ;
}

/* this function handles the exception for the NMI Interrupt 
   there is no input our output */
void NMI_interrupt(void)
{
    clear();
    printf("Nonmaskable Interrupt Error");
    while (1)
        ;
}

/* this function handles the exception for the breakpoint 
   there is no input our output */
void breakpoint(void)
{
    clear();
    printf("Breakpoint Error");
    while (1)
        ;
}

/* this function handles the overflow exception 
   there is no input our output */
void overflow(void)
{
    clear();
    printf("Overflow Error");
    while (1)
        ;
}

/* this function handles the exception for bound being exceeded 
   there is no input our output */
void bound_exceeded(void)
{
    clear();
    printf("Bound Exceeded Error");
    while (1)
        ;
}

/* this function handles the exception for an invalid opcode 
   there is no input our output */
void invalid_opcode(void)
{
    clear();
    printf("Invalid Opcode Error");
    while (1)
        ;
}

/* this function handles the exception for the device being unavailable 
   there is no input our output */
void device_unavailable(void)
{
    clear();
    printf("Device Unavailable Error");
    while (1)
        ;
}

/* this function handles the exception for double fault 
   there is no input our output */
void double_fault(void)
{
    clear();
    printf("Double Fault Error");
    while (1)
        ;
}

/* this function handles the exception for the coprocessor being overun 
   there is no input our output */
void coprocessor_overun(void)
{
    clear();
    printf("Coprocessor Segment Overun Error");
    while (1)
        ;
}

/* this function handles the exception for invalid tss 
   there is no input our output */
void invalid_tss(void)
{
    clear();
    printf("Invalid TSS Error");
    while (1)
        ;
}

/* this function handles the exception for no segment 
   there is no input our output */
void no_segment(void)
{
    clear();
    printf("Segment Not Present Error");
    while (1)
        ;
}

/* this function handles the exception for a seg fault on the stack 
   there is no input our output */
void stack_seg_fault(void)
{
    clear();
    printf("Stack-Segment Fault Error");
    while (1)
        ;
}

/* this function handles the exception for protection 
   there is no input our output */
void gen_protection(void)
{
    // clear();
    printf("General Protection Error");
    while (1)
        ;
}

void page_fault(){
    // clear();
    printf("Page Fault Error");
    while(1);
}

void math_fault(){
    clear();
    printf("Math Fault Error");
    while(1);
}

void alignment_check(){
    clear();
    printf("Alignment Check Error");
    while(1);
}

void machine_check(){
    clear();
    printf("Machine Check Error");
    while(1);
}

void float_point_exception(){
    clear();
    printf("Floating Point Exception Error");
    while(1);
}

void debug(){
    clear();
    printf("Debug Error");
    while(1);
}

/* this function handles the exception for the system call 
   there is no input our output */
// void system_call(void)
// {
//     clear();
//     printf("System Call Occurred"); // FOR NOW
//     while (1)
//         ;
// }
