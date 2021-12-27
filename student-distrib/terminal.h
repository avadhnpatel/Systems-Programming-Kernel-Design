#include "types.h"
#include "system_call.h"
#ifndef TERMINAL_H
#define TERMINAL_H
#define VIDEO 0xB8000
#define TERMINAL1 0xB9000
#define TERMINAL2 0xBA000
#define TERMINAL3 0xBB000
#define termsize 3
#define onete 128
#define FOURKB 4096
#define ATTRIB 0x7

extern void terminal_init();
int32_t terminal_open(const uint8_t *filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_write(int32_t fd, const void* keyboard_buffer, int32_t num_bytes);
int32_t terminal_read(int32_t fd, void* buffer, int32_t num_bytes);
void switch_terminal(int32_t id);
void copy_mem(char * dest, char * src);

typedef struct terminals {
    int screen_x;
    int screen_y;
    uint8_t keyboard_buffer[onete];
    int buffer_index;
    int active_flag;
    int enter_index[onete];
    unsigned int enter_offset;
    unsigned int caps_pressed;
    unsigned int shift_pressed;
    unsigned int cntrl_pressed;
    int enter_flag;
    int cur_pcb;
    uint32_t esp;
    uint32_t k_esp;
    uint32_t ebp;
    uint32_t cr3;
    int fishCheck;
} terminal_t;

extern terminal_t terminal[termsize];
extern int cur_terminal;
extern int halt_flag;
extern int no_process_flag;
extern int address;
extern int execute_terminal;
#endif
