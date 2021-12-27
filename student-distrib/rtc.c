#include "rtc.h"
#include "idt.h"
#include "idt_assembly_linkage.h"
#include "i8259.h"
#include "x86_desc.h"
#include "tests.h"
#include "lib.h"
#define RTC_IRQ_NUM 8
#define REGISTER_A_MASK 0x8A
#define REGISTER_B_MASK 0x8B
#define REGISTER_C 0xC
#define BIT_6_TURNON 0x40
#define RTC_W_MASK 0xF0
#define BASE_FREQ 0x0F
#define MIN_HZ 2
#define MAX_HZ 1024

void test_interrupts();

volatile uint32_t rtc_ticks;
volatile int32_t curr_rtc_interrupt;
volatile int interrupt_occurred;

// OS DEV https://wiki.osdev.org/RTC
/*
This function initializes the RTC chip. 
It does this by enabling the interrupts as well as the IRQ number.
It takes no inputs or outputs.
*/
void rtc_init(void)
{
    unsigned char prev;
    cli();              // start of critical section

    outb(REGISTER_B_MASK, RTC_IO);     // sets the register B index, disable NMI
    prev = inb(CMOS_IO); // get current value of register B
    outb(REGISTER_B_MASK, RTC_IO);     //reset index to B
    outb(prev | BIT_6_TURNON, CMOS_IO); // turns on bit 6 of register B

    outb(REGISTER_A_MASK, RTC_IO); // sets register A index, disable NMI
    prev = inb(CMOS_IO); // gets initial value of A
    outb(REGISTER_A_MASK, RTC_IO); // resets index to A
    outb((prev & RTC_W_MASK) | BASE_FREQ, CMOS_IO); // writes base rate to A. (bottom 4 bits)
    sti();              // end of critical section

    enable_irq(RTC_IRQ_NUM);            // enable irq

    // rtc_ticks = 0;
    // curr_rtc_interrupt = 0;

    // printf("r t c  o c c u r r e d");
}

// OS DEV https://wiki.osdev.org/RTC
/*
This function handles the RTC interrupt and is called when the interrupt is raised. 
It takes no inputs or outputs.
*/
void rtc_handler(void)
{   
    send_eoi(RTC_IRQ_NUM);          // end interrupt
    cli();              // start of critical section

    // printf("r t c  h a n d l e r  o c c u r r e d");
    /* test_interrupts(); */
    // OS DEV

    outb(REGISTER_C, RTC_IO); // select register C
    inb(CMOS_IO); // just throw away contents
    interrupt_occurred = 1; //set interrupt flag to 1
    
    sti();              // end of critical section
}

/* 
This function opens and initializes the RTC. It takes in file name as an input parameter.
It outputs 0 once RTC initialization is finished.
*/
int32_t rtc_open(const uint8_t* filename)
{   
    unsigned char prev;
    cli();                  // start of critical section
    outb(REGISTER_A_MASK, RTC_IO); // sets register A index, disable NMI
    prev = inb(CMOS_IO); // gets initial value of A
    outb(REGISTER_A_MASK, RTC_IO); // resets index to A
    outb((prev & RTC_W_MASK) | BASE_FREQ, CMOS_IO); // writes base rate to A. (bottom 4 bits)
    sti();                  // end of critical section
    return 0;
}

/*
This function closes the RTC. It takes in the file descriptor as an input parameter.
It outputs 0 to signify RTC being closed.
*/
int32_t rtc_close(int32_t fd)
{
    return 0;
}

/*
This functions waits for periodic rtc interrupt to occur. It takes in file descriptor, 
buffer, and size as input parameters. Checks value in volatile int interrupt_occurred.
Stays in while loop until interrupt_occurred = 1. Outputs 0 when while loop terminates.
*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
    interrupt_occurred = 0; // reset interrupt flag
    while(interrupt_occurred == 0); // wait till  interrupt rtc_handler is called
    return 0;    
}

/*
This function write the new rate to the RTC based on the frequency. It takes in file
descriptor, buffer, and size as input parameters. It writes the buffer rate to register A.
Outputs -1 if invalid frequency. Outputs 0 if frequency is valid.
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
{
    if (buf == NULL){
        return -1;
    }
    int write_freq = BASE_FREQ;
    unsigned char prev;
    int i;
    int rate = 0;
    // loop through frequencies and check which rate to set to.
    for (i = MIN_HZ; i <= MAX_HZ; i*=2){
        if (*(int*)(buf) == i){
            rate = write_freq;
            break;
        }
        write_freq--;
    }
    // if frequency is invalid return -1
    if (rate == 0){
        return -1;
    }


    cli();                  // start of critical section
    rate &= BASE_FREQ; // set hex
    outb(REGISTER_A_MASK, RTC_IO); // sets register A index, disable NMI
    prev = inb(CMOS_IO); // gets initial value of A
    outb(REGISTER_A_MASK, RTC_IO); // resets index to A
    outb((prev & RTC_W_MASK) | rate, CMOS_IO); // writes base rate to A. (bottom 4 bits)
    sti();                  // end of critical section
    return 0;
}
