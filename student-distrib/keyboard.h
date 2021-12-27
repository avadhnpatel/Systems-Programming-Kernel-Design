#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

#define KEYBOARD_DATA       0x60
#define KEYBOARD_COMMAND    0x64
#define NUM_KEYS 0x3B
#define ESCAPE 0x1B
#define KEYBOARD_MASK 0x1
#define MAX_PRESSED 88
#define KEYBOARD_IRQ_INT 1
#define SHIFT_PRESS_L 0x2A
#define SHIFT_PRESS_R 0x36
#define SHIFT_UNPRESS_L 0xAA
#define SHIFT_UNPRESS_R 0x36
#define ALT_PRESS 0x38
#define ALT_UNPRESS 0xB8
#define CAPS_PRESS 0x3A
#define CNTRL_PRESS 0x1D
#define CNTRL_UNPRESS 0x9D
#define NUM_ROWS    25
#define MAX_CHAR 128
#define MAX_SIZE 40
#define F1_UNPRESS 0x3b
#define F2_UNPRESS 0x3c
#define F3_UNPRESS 0x3d
#define NUM_COLS    80

extern uint8_t keyboard_buffer[128];
extern unsigned int buffer_index;
extern unsigned int enter_flag;

extern void keyboard_init(void);
extern void keyboard_handler(void);
extern void handle_key(unsigned char keyboard_input);
extern void handle_enter(int flag);

#endif
