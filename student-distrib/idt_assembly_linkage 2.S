.data
    RTC_INTERRUPT = 40
    OVERFLOW_EX = 4
    SYST_CALL = 0x80

.globl divide_error_linkage, reserved_linkage, NMI_int_linkage, breakpoint_linkage, overflow_linkage
.globl bound_exceeded_linkage, invalid_opcode_linkage, device_unavailable_linkage, double_fault_linkage
.globl coprocessor_overun_linkage, invalid_tss_linkage, no_segment_linkage, stack_seg_fault_linkage
.globl gen_protection_linkage, page_fault_linkage, math_fault_linkage, alignment_check_linkage
.globl machine_check_linkage, float_point_ex_linkage, system_call_linkage, keyboard_linkage, rtc_linkage
.globl pit_linkage, rtc_test, overflow_test, system_call_test, debug_linkage

ret_value:
    .long 0x0


# this handles the exception for divide_error
# there is no input or output
divide_error_linkage:
    pusha
    call divide_error 
    popa
    iret

# this handles the exception for reserved
# there is no input or output
reserved_linkage:
    pusha
    call reserved
    popa
    iret

# this handles the exception for nmi interrupt
# there is no input or output
NMI_int_linkage:
    pusha
    call NMI_interrupt # 
    popa
    iret

# this handles the breakpoint exception
# there is no input or output
breakpoint_linkage:
    pusha
    call breakpoint
    popa
    iret

# this handles the overflow exception
# there is no input or output
overflow_linkage:
    pusha
    call overflow
    popa
    iret

# this handles the exceeded bound exception
# there is no input or output
bound_exceeded_linkage:
    pusha
    call bound_exceeded
    popa
    iret

# this handles the invalid opcode exception
# there is no input or output
invalid_opcode_linkage:
    pusha
    call invalid_opcode
    popa
    iret

# this handles the unavailable device exception
# there is no input or output
device_unavailable_linkage:
    pusha
    call device_unavailable
    popa
    iret

# this handles the double fault exception
# there is no input or output
double_fault_linkage:
    pusha
    call double_fault
    popa
    iret

# this handles the coprocessor overun exception
# there is no input or output
coprocessor_overun_linkage:
    pusha
    call coprocessor_overun
    popa
    iret

# this handles the invalid tss exception
# there is no input or output
invalid_tss_linkage:
    pusha
    call invalid_tss
    popa
    iret

# this handles the no segment exception
# there is no input or output
no_segment_linkage:
    pusha
    call no_segment
    popa
    iret

# this handles the segmentation fault for the stack exception
# there is no input or output
stack_seg_fault_linkage:
    pusha
    call stack_seg_fault
    popa
    iret

# this handles the protection exception
# there is no input or output
gen_protection_linkage:
    pusha
    call gen_protection
    popa
    iret

page_fault_linkage:
    pusha
    call page_fault
    popa
    iret

math_fault_linkage:
    pusha
    call math_fault
    popa
    iret

alignment_check_linkage:
    pusha
    call alignment_check
    popa
    iret

machine_check_linkage:
    pusha
    call machine_check
    popa
    iret

float_point_ex_linkage:
    pusha
    call float_point_exception
    popa
    iret

debug_linkage:
    pusha
    call debug
    popa
    iret

# this handles the system call exception
# there is no input or output
system_call_linkage:
    cmpl $1, %eax
    jl INVALID_SYSCALL
    cmpl $10, %eax
    jg INVALID_SYSCALL
    pusha

    pushl %edx
    pushl %ecx
    pushl %ebx

    call *system_call_jump_table(,%eax,4)

    popl %ebx
    popl %ecx
    popl %edx
    movl %eax, ret_value
    popa
    movl ret_value, %eax
    iret

system_call_jump_table:
    .long 0x0
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn


INVALID_SYSCALL:
    movl $-1, %eax
    iret

# this handles the keyboard exception
# there is no input or output
keyboard_linkage:
    pusha
    call keyboard_handler
    popa
    iret

# this handles the rtc exception
# there is no input or output
rtc_linkage:
    pusha
    call rtc_handler
    popa
    iret

# this handles the rtc exception
# there is no input or output
pit_linkage:
    pusha
    call pit_handler
    popa
    iret


# ################################################################
# some tests to manually trigger interrupts

# this handles the rtc test 
# there is no input or output
rtc_test:
    int $RTC_INTERRUPT
    ret

# this handles the overflow test 
# there is no input or output
overflow_test:
    int $OVERFLOW_EX
    ret

# this handles the system call test 
# there is no input or output
system_call_test:
    int $SYST_CALL
    ret


