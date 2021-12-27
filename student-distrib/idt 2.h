#ifndef IDT_H
#define IDT_H

#define SYS_CALL 0x80

extern void idt_init();

// HANDLERS
extern void divide_error(void);
extern void reserved(void);
extern void NMI_interrupt(void);
extern void breakpoint(void);
extern void overflow(void);
extern void bound_exceeded(void);
extern void invalid_opcode(void);
extern void device_unavailable(void);
extern void double_fault(void);
extern void coprocessor_overun(void);
extern void invalid_tss(void);
extern void no_segment(void);
extern void stack_seg_fault(void);
extern void gen_protection(void);
// extern void page_fault();
// extern void math_fault();
// extern void alignment_check();
// extern void machine_check();
// extern void float_point_exception();
extern void system_call(void);


#endif
