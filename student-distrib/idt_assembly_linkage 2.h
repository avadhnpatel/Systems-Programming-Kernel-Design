#ifndef IDT_ASSEMBLY_LINKAGE_H
#define IDT_ASSEMBLY_LINKAGE_H

#include "idt.h"
#include "idt_assembly_linkage.h"
#include "tests.h"
#include "system_call.h"


void divide_error_linkage();
void reserved_linkage();
void NMI_int_linkage();
void breakpoint_linkage();
void overflow_linkage();
void bound_exceeded_linkage();
void invalid_opcode_linkage();
void device_unavailable_linkage();
void double_fault_linkage();
void coprocessor_overun_linkage();
void invalid_tss_linkage();
void no_segment_linkage();
void stack_seg_fault_linkage();
void gen_protection_linkage();
void page_fault_linkage();
void math_fault_linkage();
void alignment_check_linkage();
void machine_check_linkage();
void float_point_ex_linkage();
void debug_linkage();
void system_call_linkage();
void keyboard_linkage();
void pit_linkage();
void rtc_linkage();
void rtc_test();
void system_call_test();
void overflow_test();


#endif
