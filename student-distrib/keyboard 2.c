#include "keyboard.h"
#include "idt.h"
#include "idt_assembly_linkage.h"
#include "i8259.h"
#include "x86_desc.h"
#include "tests.h"
#include "lib.h"
#include "terminal.h"
// #include <stdbool.h>

#define SPACE_CHAR 0x39
#define SCREEN_WIDTH 80
int cur_terminal;
//unsigned int terminal[cur_terminal].enter_index[MAX_CHAR];
//unsigned int terminal[cur_terminal].enter_offset = 0;
unsigned int alt_pressed = 0;
unsigned int caps_pressed = 0;
//unsigned int terminal[cur_terminal].shift_pressed = 0;
//unsigned int terminal[cur_terminal].cntrl_pressed = 0;
//unsigned int terminal[cur_terminal].buffer_index;
//unsigned int terminal[cur_terminal].enter_flag;
terminal_t terminal[3];

//uint8_t terminal[cur_terminal].keyboard_buffer[MAX_CHAR];
unsigned char keys[4][NUM_KEYS] = {            // nothing pressed
                                {0, ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
                                'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f',  // left control
                                'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', // left shift
                                'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' ', 0},          // right shift, * keypad, left alt, CAPS
                                            // shift pressed
                                {0, ESCAPE, '!', '@', '#', '$', '%', '^', '&', '*', '(',
                                ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
                                'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F',      // left control
                                'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X',     // left shift
                                'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' ', 0},          // right shift, * keypad, left alt, CAPS
                                            // caps pressed
                                {0, ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                '0', '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
                                'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F',      // left control
                                'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X',     // left shift
                                'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, 0, 0, ' ', 0},          // right shift, * keypad, left alt, CAPS
                                //             // shift and caps
                                {0, ESCAPE, '!', '@', '#', '$', '%', '^', '&', '*', '(',
                                ')', '_', '+', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
                                'u', 'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's', 'd', 'f',      // left control
                                'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|', 'z', 'x',     // left shift
                                'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, 0, 0, ' ', 0}         // right shift, * keypad, left alt, CAPS
};

/*
this function initializes the keyboard by enabling the IRQ
it takes in no inputs and does not return anything
*/
void keyboard_init(void)
{
    enable_irq(KEYBOARD_IRQ_INT); // enable irq 1 for keyboard
    terminal[cur_terminal].buffer_index = 0;             // initialize index for buffer
    terminal[cur_terminal].enter_flag = 0;               // declare interrupt flag
}

/*
this function handles the keyboard interrupt
it takes in no inputs and returns no values
*/
void keyboard_handler(void)
{
    cli(); // start of critical section

    uint8_t keyboard_status = inb(KEYBOARD_COMMAND); // get current value
    unsigned char keyboard_input;

    if (keyboard_status & KEYBOARD_MASK) // checks the last bit of keyboard_status
    {
        keyboard_input = inb(KEYBOARD_DATA); // reads the keycode
        // if (keyboard_input <= MAX_PRESSED)
        // {
        //     printf("%c", keys[0][keyboard_input]);
        // }
        
        // keyboard input logic
        if(keyboard_input == SHIFT_PRESS_L || keyboard_input == SHIFT_PRESS_R) // check if shift
            terminal[cur_terminal].shift_pressed = 1;
        else if(keyboard_input == SHIFT_UNPRESS_L || keyboard_input == SHIFT_UNPRESS_R) // check if released shift
            terminal[cur_terminal].shift_pressed = 0;
        else if(keyboard_input == CAPS_PRESS) // check if caps press
            caps_pressed = 1 - caps_pressed; // terminal[cur_terminal].caps_pressed;
        else if(keyboard_input == ALT_PRESS) // check if alt pressed
            alt_pressed = 1;
        else if(keyboard_input == ALT_UNPRESS) // check if alt released
            alt_pressed = 0;
        else if(keyboard_input == CNTRL_PRESS) // check if control pressed
            terminal[cur_terminal].cntrl_pressed = 1;  
        else if(keyboard_input == CNTRL_UNPRESS) // check if control released
            terminal[cur_terminal].cntrl_pressed = 0;
        else if(keyboard_input < MAX_PRESSED || keyboard_input == F1_UNPRESS || keyboard_input == F2_UNPRESS || keyboard_input == F3_UNPRESS) // check if input less than 88
            handle_key(keyboard_input);
    }
    // if (terminal[cur_terminal].cntrl_pressed)
    //     printf("%s", "CNTRL PRESSED");

    send_eoi(KEYBOARD_IRQ_INT); //end of interrupt
    sti();  // end of critical section
}

/*
This function takes the keyboard_input scancode and maps it to a value to be outputted 
and then handles the screen output corresponding to the value. It takes in hexadecimal 
scancode as an input.
*/
void handle_key(unsigned char keyboard_input){
    // initialize vars
    unsigned int key_type; 
    unsigned char curr_char;

    // Terminal check
    if (keyboard_input == F1_UNPRESS || keyboard_input == F2_UNPRESS || keyboard_input == F3_UNPRESS)
    {
        if (alt_pressed)
        {
            //switch terminal
            if (keyboard_input == F1_UNPRESS)
            {
                switch_terminal(0);
               // printf("switch 0");
            }
            else if (keyboard_input == F2_UNPRESS)
            {
                switch_terminal(1);
              //  printf("switch 1");
            }
            else if (keyboard_input == F3_UNPRESS)
            {
                switch_terminal(2);
             //   printf("switch 2");
            }
            
        }
        return;
    }
    

    // types of key processing
    if(terminal[cur_terminal].shift_pressed && caps_pressed) // check if shift and caps both pressed
        key_type = 3;
    else if(caps_pressed) // check if just caps is pressed
        key_type = 2;
    else if(terminal[cur_terminal].shift_pressed) // check if just shift is pressed
        key_type = 1;
    else                    // check if none is pressed
        key_type = 0;

    curr_char = keys[key_type][keyboard_input]; // index mapping struct

    // cntrl + l
    if(terminal[cur_terminal].cntrl_pressed){
        if(curr_char == 'l' || curr_char == 'L'){
            // clear screen
            clear();

            // put cursor at the top left
            move_cursor(0, 0);
            terminal[cur_terminal].buffer_index = 0; // set buf index
            terminal[cur_terminal].enter_offset = 0;
            memset(terminal[cur_terminal].enter_index, 0, MAX_CHAR); // set all to zero
            memset(terminal[cur_terminal].keyboard_buffer, 0, MAX_CHAR); // set all to zero
        }
        return;
    }

    // backspace case
    if(curr_char == '\b'){
        if(terminal[cur_terminal].buffer_index > 0){                   // check if greater than zero
            terminal[cur_terminal].keyboard_buffer[terminal[cur_terminal].buffer_index] = NULL; 
            terminal[cur_terminal].buffer_index -= 1;                  // decrement buffer index
            if(get_screen_x() > 0)              // check if function returns positive val
                set_screen_x(get_screen_x() - 1);
            else if(get_screen_y() != 0){       // check if nonzero
                    set_screen_y(get_screen_y() - 1);
                    set_screen_x(terminal[cur_terminal].enter_index[get_screen_y() + terminal[cur_terminal].enter_offset]);
                }
            handle_backspace(); // call backspace handler
            move_cursor(get_screen_x(), get_screen_y()); // move cursor
        }
        return;
    }
    // tab case
    if (curr_char == '\t')             // check if char is tab
    {
        handle_key(SPACE_CHAR);       // print 3 spaces for tab
        handle_key(SPACE_CHAR);
        handle_key(SPACE_CHAR);
        return;
    }
    

    // enter case
    if(curr_char == '\n' && keyboard_input < MAX_PRESSED){
        if(terminal[cur_terminal].buffer_index < MAX_CHAR - 1){ // check if buffer index is in bounds
            handle_enter(1);              // call enter handler
        }
        return;
    }
    
    if(keyboard_input < MAX_PRESSED){
        // check if buffer is full
        if(terminal[cur_terminal].buffer_index < MAX_CHAR - 1){ //check if buffer index is in bounds
            terminal[cur_terminal].keyboard_buffer[terminal[cur_terminal].buffer_index] = curr_char;
            terminal[cur_terminal].buffer_index += 1;           // increment buffer index
            int temp_loc = get_screen_x();
            putc(curr_char, cur_terminal);
            if (temp_loc == (SCREEN_WIDTH - 1)) { // check if in bounds
                set_screen_x(SCREEN_WIDTH-1);
                handle_enter(0); // call enter key handler
                set_screen_x(0);
            }
        }
        move_cursor(get_screen_x(), get_screen_y());    // move cursor
        return;
    }
}

/* 
This function acts as a handler for pressing enter on keyboard. This function
has not input or output.
*/
void handle_enter(int flag) {
    //keyboard interrupt flag
    unsigned char curr_char = '\n';
    if(get_screen_y() == NUM_ROWS - 1){
        // unsigned int i,j,z;
        // for(i = 0; i < SCREEN_WIDTH; i++){
        //     terminal[cur_terminal].buffer_index --;        // decrement buffer index
        //     if(terminal[cur_terminal].keyboard_buffer[i] == '\n'){ // check if new line char
        //         terminal[cur_terminal].enter_flag = 1;  
        //         break;
        //     }
        // }

        // if (i == 0)
        // {
        //     i++;        //increment i if ==0
        // }
        
        // for(z = 0; z < i; z++) { // iterate through terminal[cur_terminal].keyboard_buffer
        //     for (j = 0; j < MAX_CHAR; j++)
        //     {
        //         terminal[cur_terminal].keyboard_buffer[j] = terminal[cur_terminal].keyboard_buffer[j+1];   // IFFY need to make sure we max out at 128 characters
        //     }
        // }
        vertical_scroll(cur_terminal);
        terminal[cur_terminal].enter_offset += 1; // increment enter offset
        terminal[cur_terminal].enter_index[get_screen_y() - 1 + terminal[cur_terminal].enter_offset] = get_screen_x();
        set_screen_x(0); 
        set_screen_y(NUM_ROWS - 1);  
    }
    else{
        terminal[cur_terminal].enter_index[get_screen_y() + terminal[cur_terminal].enter_offset] = get_screen_x(); // iffy
        putc(curr_char, cur_terminal); //print curr char
    }
    // moves cursor to newline
    move_cursor(get_screen_x(), get_screen_y()); // move cursor
    terminal[cur_terminal].keyboard_buffer[terminal[cur_terminal].buffer_index] = curr_char;
    terminal[cur_terminal].buffer_index += 1; // increment buffer index
    terminal[cur_terminal].enter_flag = flag;
    return;
}
