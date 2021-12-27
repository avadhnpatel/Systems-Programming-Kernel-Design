/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define MASK_VAL 0xFF
#define PORT_INDICATOR 8
#define SLAVE_IRQ_NUM 2

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC 
    This function initializes the slave and master pics.
    No inputs or return values
*/
void i8259_init(void)
{
    unsigned long flags;

    cli_and_save(flags);    // saves the the flags for the interrupt
    master_mask = MASK_VAL; // mask all interrupts
    slave_mask = MASK_VAL;

    outb(ICW1, MASTER_COMMAND); //set the control words to the ports of the pic in all lines below
    outb(ICW1, SLAVE_COMMAND);
    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW2_SLAVE, SLAVE_DATA);
    outb(ICW3_MASTER, MASTER_DATA);
    outb(ICW3_SLAVE, SLAVE_DATA);
    outb(ICW4, MASTER_DATA);
    outb(ICW4, SLAVE_DATA);

    outb(master_mask, MASTER_DATA);
    outb(slave_mask, SLAVE_DATA);

    enable_irq(SLAVE_IRQ_NUM);

    sti();
    restore_flags(flags); // restores the flags for the interrupt
}

/* 
    This funciton Enables (unmasks) the specified IRQ that is being inputted.
    It takes in the IRQ number that needs to be enabled and returns nothing.
*/
void enable_irq(uint32_t irq_num)
{
    uint16_t port;
    if (irq_num < PORT_INDICATOR)
    {
        port = MASTER_DATA;
        master_mask &= ~(1 << irq_num); // shifts the the irqnum left 1 then nots it and ands it with the slave mask (updates the mask)

        outb(master_mask, port);
    }
    else
    {
        port = SLAVE_DATA;
        irq_num -= PORT_INDICATOR;
        slave_mask &= ~(1 << irq_num); // shifts the irq num left 1 then nots it and ands it with the slave mask (updates the mask)

        outb(slave_mask, port);
    }
}

/* 
    This funciton Disables (masks) the specified IRQ that is being inputted.
    It takes in the IRQ number that needs to be disabled and returns nothing.
*/
void disable_irq(uint32_t irq_num)
{
    uint16_t port;
    if (irq_num < PORT_INDICATOR)
    {
        port = MASTER_DATA;
        master_mask |= (irq_num << 1); // shifts the irq num left 1 then ors it with the master mask (updates the mask)

        outb(master_mask, port);
    }
    else
    {
        port = SLAVE_DATA;
        irq_num -= PORT_INDICATOR;
        slave_mask |= (irq_num << 1); // shifts the irq num left 1 then ors it with the master mask (updates the mask)

        outb(master_mask, port);
    }
}

/* This function sends an end-of-interrupt signal for the specified IRQ that is being inputted.
    It inputs the the IRQ number that needs to be sent to eoi.
    It returns nothing.
*/
// take irq number determine slave or master and handle cases
// slave communicates w master
// 0x20-0x27 master PIC, 0x28-0x2F slave PIC
// Data and command for master 0x20, 0x21
// Data and command for slave 0xA0, 0xA1                        MAGIC NUMBERS
// if only one interrupt, bug in send_eoi
void send_eoi(uint32_t irq_num)
{
    if (irq_num >= PORT_INDICATOR) //checks if irq is on slave pic
    {
        outb(EOI | (irq_num - PORT_INDICATOR), SLAVE_COMMAND);
        outb(EOI | SLAVE_IRQ_NUM, MASTER_COMMAND);
    }
    else // irq is on master
        outb(EOI | irq_num, MASTER_COMMAND);
}
