#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include "types.h"

#define ONETWOEIGHTMB 0x08000000
#define ONETHREETWOMB 0x8400000
#define ONEKB 1024
#define FOURMB 0x0400000
#define VID 0xB8007
#define PDMAX 33
#define ONETHOUS 0x1000
#define BUFFER_SIZE 40
#define SHIFT 22
#define ANDONE 0xFFC00000
#define FILENUM 8
#define FDSTART 2
#define SEV 7
#define HALFFD 4
#define FIV 5
#define MAXPROCESS 6
#define MAXPROC 3
#define ONETWOEIGHT 128
#define TFOUR 24
#define FILEBUF 0x7F
#define CONT 0x83FFFFC
#define FIREXEC 0x8048000
#define MB_8 0x800000
#define MB_4 0x400000
#define KB_8 0x2000
#define VIRTUAL_START 0x8000000
#define KERNEL_MEM 0x400000
#define PAGE 0x400000
#define PROG_ADDR 0x8048000
#define ARGUMENT_SIZE 128
#define HEX_7 0x7
#define HEX_80 0x80
#define mask 0x007fe000


typedef struct sysJumpTable
{
    int32_t (*read)(int32_t fd, void *buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void *buf, int32_t nbytes);
    int32_t (*open)(const uint8_t *filename);
    int32_t (*close)(int32_t fd);
} sysJumpTable_t;

typedef struct fd
{
    sysJumpTable_t *sys_calls;
    int32_t inode;
    uint32_t filePosition;
    uint32_t flags;
} fd_t;

typedef struct process_control_block
{
    fd_t fdArr[FILENUM];
    uint32_t esp;
    uint32_t ebp;
    uint32_t parentID;
    uint32_t currID;
    int8_t args[96];
    uint32_t terminalID;
} pcb_t;

extern uint32_t process_count;

extern int32_t halt(uint8_t status);
extern int32_t execute(const uint8_t *command);
extern int32_t read(int32_t fd, void *buf, int32_t nbytes);
extern int32_t write(int32_t fd, const void *buf, int32_t nbytes);
extern int32_t open(const uint8_t *filename);
extern int32_t close(int32_t fd);
extern int32_t getargs(uint8_t *buf, int32_t nbytes);
extern int32_t vidmap(uint8_t **screen_start);
extern int32_t set_handler(int32_t signum, void *handler_address);
extern int32_t sigreturn(void);

extern uint32_t shell_count;
extern int falg;

extern pcb_t *get_pcb(uint32_t process_num);
void execute_paging(uint32_t process_num);
int32_t invalid_sys_read(int32_t fd, void *buffer, int32_t nbytes);
int32_t invalid_sys_write(int32_t fd, const void *buffer, int32_t nbytes);
void context_switch(uint32_t eip, uint32_t temp);
void halt_return(uint8_t status, uint32_t esp, uint32_t ebp);

uint32_t video_table[1024] __attribute__((aligned(4096)));
#endif
