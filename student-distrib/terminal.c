#include "terminal.h"
#include "keyboard.h"
#include "idt.h"
#include "idt_assembly_linkage.h"
#include "i8259.h"
#include "x86_desc.h"
#include "tests.h"
#include "lib.h"
#include "system_call.h"
#include "paging.h"
int flag1, flag2, flag3;
int address;
terminal_t terminal[termsize];
int cur_terminal;
void terminal_init(){
    int i;
    for (i = 0; i < termsize; i++)
    {
        terminal[i].screen_x = 0; //setting all the below to 0 when initializing
        terminal[i].screen_y = 0;
        terminal[i].buffer_index = 0;
        terminal[i].active_flag = 0;
        terminal[i].enter_offset = 0;
        terminal[i].shift_pressed = 0;
        terminal[i].cntrl_pressed = 0;
        terminal[i].enter_flag = 0;
        memset(terminal[i].keyboard_buffer, 0, MAX_CHAR); //allocate space for keyboard buffer
        memset(terminal[i].enter_index, 0, MAX_CHAR);
        terminal[i].cur_pcb = 0;
    }
    cur_terminal = 0;
    halt_flag = 0;
    flag1 = 0;
    flag2 = 0;
    flag3 = 0;
    return;
}

/*
This function initializes the terminal and returns 0.
Input: filename
Output: returns 0
*/
int32_t terminal_open(const uint8_t *filename){
    return 0;
}

/*
This function clears any terminal specific variables by returning 0.
Input: file descriptor even though it is not used
Output: returns 0
*/
int32_t terminal_close(int32_t fd){
    return 0;
}

/*
This function writes to the screen given a buffer and returns the number of bytes.
Input: fd is the file directory, buf is the buffer that is going to be used to write, nbytes is the number of bytes
Output: it returns the number of bytes
*/
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    unsigned int i;
    pcb_t * curr_pcb;
    if (process_count == 1 && flag1 < 2) // for initializtion of the first shell
    {
        //print terminal 0
        curr_pcb = get_pcb(1);
        flag1++;
    }
    else if (process_count == 2 && flag2 < 2)  // for initializtion of the second shell
    {
        curr_pcb = get_pcb(2);
        flag2++;
    }
    else if (process_count == 3 && flag3 < 2)  // for initializtion of the third shell
    {
        curr_pcb = get_pcb(3);
        flag3++;
    }
    else {
    //printf("%u\n",terminal[cur_terminal].cur_pcb);
   // printf("curt%u\n",cur_terminal);
    curr_pcb = get_pcb(terminal[execute_terminal].cur_pcb + 1);
    }
    
    //printf("term: %u\n", curr_pcb->terminalID);
    //printf("pc: %u\n", process_count);
    for(i = 0; i < nbytes; i++){  
        if(((uint8_t*)buf)[i] != '\0'){
            // if(falg != 0){
            //     putc(((uint8_t*)buf)[i], cur_terminal);
            // }
            // else{
                putc(((uint8_t*)buf)[i], curr_pcb->terminalID); //outputs to the terminal of the current pcb
            // }
        }
    }
    if (cur_terminal == execute_terminal)
    {
        move_cursor(terminal[execute_terminal].screen_x, terminal[execute_terminal].screen_y); // moves the cursor to the executed terminal
    }
    
    terminal[execute_terminal].enter_flag = 0;
    falg--;
    halt_flag = 0;
    return i;
}
/*
This function reads from the keyboard buffer and returns the number of bytes that were read.
Input: fd is the file directory, buffer is the buffer that is passed in, and nbytes are the number of bytes.
Output: it returns the number of bytes
*/
int32_t terminal_read(int32_t fd, void* buffer, int32_t nbytes){
        if(nbytes == 0)
            return -1;

        else if(nbytes > MAX_CHAR)
            nbytes = MAX_CHAR;

        unsigned int i;
        while (terminal[cur_terminal].enter_flag == 0) { //checks if enter flag exists
            if (terminal[cur_terminal].enter_flag == 1)
            {
                break;
            }
        }
        memset(buffer, 0, nbytes); // set all to zero

        for(i = 0; i < MAX_CHAR; i++){
            if (i < terminal[cur_terminal].buffer_index - 1){
                ((uint8_t*)buffer)[i] = terminal[cur_terminal].keyboard_buffer[i];
            }
            else{
                // ((uint8_t*)buffer)[i] = '\0';
                break;
            }
            // putc(((uint8_t*)buffer)[i]);
        }
        
        // clear buffer
        memset(terminal[cur_terminal].keyboard_buffer, 0, MAX_CHAR); // set all to zero
        terminal[cur_terminal].buffer_index = 0;
        terminal[cur_terminal].enter_flag = 0;
        return i + 1;
}

/*
This function takes in a terminal ID and switches the current terminal with the terminal that was passed in.
Input: the terminal ID is passed that you want to switch to
Output: none
*/
void switch_terminal(int id) {

    if (id == cur_terminal)
    {
        return;
    }
    // if (terminal[cur_terminal].fishCheck)
    // {
    //     if (cur_terminal == 0)
    //     {
    //         video_table[0] = TERMINAL1;
    //     }
    //     else if (cur_terminal == 1)
    //     {
    //         video_table[0] = TERMINAL2;
    //     }
    //     else if (cur_terminal == 2)
    //     {
    //         video_table[0] = TERMINAL3;
    //     }
    // }
    // if (terminal[id].fishCheck)
    // {
    //     video_table[0] = VIDEO;
    // }
    // page_directory[PDMAX] = ((uint32_t)video_table) | SEV;
    // execute_paging(terminal[id].cur_pcb);

    

    if (cur_terminal == 0)
    {
        copy_mem((char *)TERMINAL1, (char *)VIDEO); //copy memory to virtual address of terminal 1
    }
    if (cur_terminal == 1)
    {
        copy_mem((char *)TERMINAL2, (char *)VIDEO);  //copy memory to virtual address of terminal 2
    }
    if (cur_terminal == 2)
    {
    //    printf("copy mem2");
        copy_mem((char *)TERMINAL3, (char *)VIDEO);  //copy memory to virtual address of terminal 3
    }
    
    if (id == 0)
    {
   //     printf("copy mem0");
        
        cur_terminal = 0;
        copy_mem((char *)VIDEO, (char *)TERMINAL1);  //copy memory from terminal 1 to to physical visual memory
    }
    if (id == 1)
    {
      //  printf("copy mem1");
        cur_terminal = 1;
        copy_mem((char *)VIDEO, (char *)TERMINAL2); //copy memory from terminal 2 to to physical visual memory
    }
    if (id == 2)
    {
     //   printf("copy mem2");
        cur_terminal = 2;
        copy_mem((char *)VIDEO, (char *)TERMINAL3); //copy memory from terminal 3 to to physical visual memory
    }
    move_cursor(terminal[cur_terminal].screen_x, terminal[cur_terminal].screen_y);
    return;
}

/*
This function takes in a source character pointer and copies it to to a destination character pointer.
Input: dest and src are character pointers that are used to output to video memory
Output: none
*/
void copy_mem(char * dest, char * src) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(dest + (i << 1)) = *(uint8_t *)(src + (i << 1));;
        *(uint8_t *)(dest + (i << 1) + 1) = ATTRIB;
    }
    move_cursor(terminal[cur_terminal].screen_x, terminal[cur_terminal].screen_y);
}
