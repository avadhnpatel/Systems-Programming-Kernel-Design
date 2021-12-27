# .globl context_switch, halt_return

# context_switch:
#     movl 8(%esp), %eax
#     pushfl
#     popl %ecx
#     orl $0x200, %ecx
#     movl 4(%esp), %edx
#     movl (%edx), %edx
#     pushl $0x2B
#     pushl %eax
#     pushl %ecx #    pushl $0x83FFFFC
#     pushl $0x23
#     pushl %edx
#     iret

# halt_return:
#     movl 4(%esp), %eax 
#     movl 8(%esp), %ecx # parent esp
#     movl 12(%esp), %edx # parent ebp
#     movl %ecx, %esp
#     movl %edx, %ebp
#     leave
#     ret

