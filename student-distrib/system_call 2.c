#include "system_call.h"
#include "idt.h"
#include "idt_assembly_linkage.h"
// #include "i8259.h"
#include "x86_desc.h"
#include "tests.h"
#include "lib.h"
#include "filesys.h"
#include "keyboard.h"
#include "terminal.h"
#include "rtc.h"
#include "filesys.h"
#include "terminal.h"
#include "paging.h"
#include "syscall_assembly.h"

struct sysJumpTable rtc_table = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};
struct sysJumpTable stdin_table = {&terminal_read, &invalid_sys_write, &terminal_open, &terminal_close};
struct sysJumpTable stdout_table = {&invalid_sys_read, &terminal_write, &terminal_open, &terminal_close};
struct sysJumpTable file_table = {&readFile, &writeFile, &openFile, &closeFile};
struct sysJumpTable dir_table = {&readDirectory, &writeDirectory, &openDirectory, &closeFile};

struct sysJumpTable *function_table[MAXPROC] = {&rtc_table, &dir_table, &file_table};

uint8_t curr_processes[MAXPROCESS] = {0, 0, 0, 0, 0, 0};
uint32_t process_count = 0;
uint32_t shell_count = 0;
uint8_t file_name[THIRTYTWO + 1];
int halt_flag;
int falg = 0;
int execute_terminal;
terminal_t terminal[3];
/*
This funciton takes in the porcess number and reutrn the pcb associated with that process number.
Input: process_num which is the process number you are passing in.
Output: pcb that is associated with that number.
*/
pcb_t *get_pcb(uint32_t process_num)
{
    return (pcb_t *)(MB_8 - (process_num)*KB_8); //returns location of current pcb
}

/*
This funciton clears the file directories/pcbs and halts the current process.
Input: the status of the current pcb
Output: the status of the command which can be a number from -1 to 256
*/
int32_t halt(uint8_t status)
{
    halt_flag = 1;
    // printf("Reached halt\n");
    pcb_t *curr_pcb;
    int i;

    cli();                        //clear
    terminal[cur_terminal].active_flag = 0;
    curr_pcb = get_pcb(process_count); //gets current
    // terminal[cur_terminal].active_flag = 0;
    // for (i = 0; i < 3; i++)
    // {
    //     if (terminal[i].cur_pcb == curr_pcb->currID)
    //     {
    //         terminal[i].cur_pcb = -1;
    //     }
    // }
    for (i = FDSTART; i < FILENUM; i++){ //start at 2 because first two are allocated for
        curr_pcb->fdArr[i].filePosition = 0; // clear file position
        curr_pcb->fdArr[i].sys_calls = NULL;
        curr_pcb->fdArr[i].inode = -1; //clear inode
        curr_pcb->fdArr[i].flags = 0;  //set flag to 0 bc it is not running
        close(i);
    }

    curr_processes[curr_pcb->currID] = 0;       // set process not running
    // if(strncmp((int8_t *)curr_pcb->args, (int8_t *)"fish", 4) == 0)
    //     printf("\n");
    // if(curr_pcb->terminalID != cur_terminal && strncmp((int8_t *)file_name, (int8_t *)"shell", 5) == 0)
    //     curr_pcb->terminalID = cur_terminal;

    // uint32_t check_ret;
    // asm volatile("popl %%eax;"
    //     "movl %%eax, %0;"
    //         : "=r"(check_ret));
    // printf("ret_value: %u\n", check_ret);
   
    // decrement shell if not orig shell and filename was shell
    // if(strncmp((int8_t *)file_name, (int8_t *)"shell", 5) == 0 && process_count != 1){
    //     shell_count -= 1;
    // }
    // printf("shell count halt: %u\n", shell_count);

    if(curr_pcb->currID <= 2){          // trying to exit from shell
        process_count -= 1;
        execute((uint8_t*)"shell");
    }
    // decrement process count
    process_count -= 1;
    // printf("process count:%u\n", process_count);

    uint32_t p_esp = curr_pcb->esp; //parent esp
    uint32_t p_ebp = curr_pcb->ebp; //parent ebp

    execute_paging(curr_pcb->parentID);

    // Flush TLB
    asm volatile("movl %%cr3, %%eax \n\
                   movl %%eax, %%cr3 \n\
                   " ::
                     : "memory");

    // TSS
    tss.esp0 = MB_8 - (KB_8 * (curr_pcb->parentID)) - HALFFD; //location of parent pcb
    // printf("tss esp0 %x\n", tss.esp0);
    tss.ss0 = KERNEL_DS;

    // make 8bit
    int32_t bit8_status = (int32_t)(status & 0xFF);

    sti();

    // sets parent esp and ebp to current esp and ebp
    asm volatile("mov %0, %%eax;"
                "mov %1, %%esp;"
                "mov %2, %%ebp;"
                "jmp EXECUTE_LABEL;"
                :
                : "r" (bit8_status), "r" (p_esp), "r" (p_ebp)
                : "%eax"
                );
    // putc("\n");
    return status;
}

/*
This function takes in a command and creates a new process and loads the files based on the command.
Input: command is a pointer to a command that is needed to be executed
Output: the status of the command which can be a number from -1 to 256
*/
int32_t execute(const uint8_t *command)
{
    execute_terminal = cur_terminal;
    // printf("Reached execute\n");
    // memset(file_name, 0, 33); // set all to zero
    if (command == NULL){ //null check
        printf("NULL Command\n");
        return -1;
    }

    // if(shell_count == 3){       // shell check
    //     printf("Too many shells\n");
    //     return -1;
    // }

    if (process_count >= MAXPROCESS){ //checks if there are more than 6 processes
        printf("Too many processes\n");
        return -1;
    }

     // get file name from command
    while (*command == ' ')
        command++;
    // uint8_t file_name[THIRTYTWO + 1]; //allocates 32 bits space for filename
    uint8_t index = 0;
    while (*command != ' ' && *command != '\0' && *command != '\n' && index < 32){ //iterates through commands
        file_name[index] = *command;
        command++;
        index++;
    }
    // if (index > THIRTYTWO){ //check if it is greater than the maximum
    //     printf("Invalid Filename\n");
    //     return -1;
    // }
    file_name[index] = '\0';

    while (*command == ' ' && *command != '\0')
        command++;

    // get argument name from command
    index = 0;
    uint8_t argument_name[MAX_CHAR + 1];
    while (*command != ' ' && *command != '\0' && *command != '\n'){ //iterates through commands
        argument_name[index] = *command;
        command++;
        index++;
    }
    // if (index > THIRTYTWO){ //argument is too large
    //     printf("Invalid Argument\n");
    //     return -1;
    // }
    argument_name[index] = '\0';

    // check if file exists
    if (openFile((uint8_t *)file_name) != 0){ //checks if filename exists
        // falg = 2;
        printf("File does not exist\n");
        return -1;
    }

    uint8_t file_buffer[BUFFER_SIZE];
    dentry_t dentry;
    if (read_dentry_by_name((uint8_t *)file_name, &dentry) == -1) //checks if file name exists
        return -1;
    if (read_data(dentry.iNodeNumber, 0, file_buffer, BUFFER_SIZE) == -1)   // read file into buffer
        return -1;

    // not an executable if ELF not at beginning
    if (file_buffer[0] != FILEBUF || file_buffer[1] != 'E' || file_buffer[FDSTART] != 'L' || file_buffer[MAXPROC] != 'F')
        return -1;

    // start executing executable at program entry point
    uint32_t eip_val;
    eip_val = (file_buffer[24] + (file_buffer[25]<<8) + (file_buffer[26]<<16) + (file_buffer[27]<<24));

    // find open process index
    unsigned int process_index;
    for(process_index = 0; process_index < MAXPROCESS; process_index++){
        if(curr_processes[process_index] == 0)
            break;
    }
    // if (process_index == MAXPROCESS){ //checks if there are more than 6 processes
    //     printf("Too many processes\n");
    //     return -1;
    // }

    // mark as in progress
    curr_processes[process_index] = 1;

    // paging first executable starts at 128MB (0x08000000) + offset (0x00048000) = 0x08048000
    // copying file to 0x08048000 and execute at entry point
    execute_paging(process_count);

    // Flush TLB
    asm volatile("movl %%cr3, %%eax \n\
                   movl %%eax, %%cr3 \n\
                   " ::
                     : "memory");

    // copy file to 0x08048000 address
    uint8_t *load_prog = (uint8_t *)PROG_ADDR;
    uint32_t bytes = -1;
    uint8_t program_buffer[ONETWOEIGHT]; //128 mb for the stack
    uint32_t offset = 0;
    unsigned int k;
    while (bytes != 0){
        bytes = read_data(dentry.iNodeNumber, offset, program_buffer, ONETWOEIGHT); //reads the bytes over from the buffer
        offset += bytes;
        for (k = 0; k < bytes; k++){
            *load_prog = program_buffer[k]; //loads the bytes into the buffer
            load_prog++;
        }
    }

    pcb_t* curr_pcb = get_pcb(process_count + 1);       // get current pcb

    curr_pcb->currID = process_count;
    // printf("process count: %u\n", curr_pcb->currID);

    // if not original shell
    if(process_count != 0) {
        // if(strncmp((int8_t *)file_name, (int8_t *)"shell", 5) == 0)     // increment shell count if not original shell
        //     shell_count++;
        pcb_t* parent_pcb = get_pcb(process_count);                     // get parent pcb
        curr_pcb->parentID = parent_pcb->currID;
    }
    // 
    // printf("shell count execute: %u\n", shell_count);

    // increment proccess count
    process_count++;

    // move esp and ebp into pcb struct
    asm volatile("movl %%esp, %0;"
                "movl %%ebp, %1;"
                : "=r"(curr_pcb->esp), "=r"(curr_pcb->ebp));

    // set up stdin and stdout tables
    curr_pcb->fdArr[0].sys_calls = &stdin_table; //from jump table
    curr_pcb->fdArr[0].flags = 1;
    curr_pcb->fdArr[1].sys_calls = &stdout_table;
    curr_pcb->fdArr[1].flags = 1;
    strcpy((int8_t *) curr_pcb->args, (int8_t *) argument_name);    // set arguments in pcb for getargs

    // TSS
    tss.ss0 = KERNEL_DS;
    tss.esp0 = MB_8 - (process_count - 1) * KB_8 - HALFFD;      // maybe not -1

    if(process_count <= 3){
        curr_pcb->terminalID = process_count - 1;
        terminal[process_count - 1].cur_pcb = curr_pcb->currID;
        terminal[process_count - 1].active_flag = 1;
    }
    else {
        terminal[cur_terminal].cur_pcb = curr_pcb->currID;
        terminal[cur_terminal].active_flag = 1;
        curr_pcb->terminalID = cur_terminal;
    }
    cli();

    // context switch to user program
    // descriptor.pdf
    // use iret, worr about 5: return address (EIP), CS, ESP (bottom of user program paging 132 mb in
    // virtual memory), SS (data segment)
    asm volatile("pushl $0x2B;"
                "pushl $0x83FFFFC;"
                "pushfl;"
                "popl %%ecx;"
                "orl $0x200, %%ecx;"
                "pushl %%ecx;"
                "pushl $0x23;"
                "pushl %0;"
                "iret;"
                "EXECUTE_LABEL: "
                :
                : "r" (eip_val)
                : "ecx", "eax"
                );

    return 0;
}

/*
This function is used to reset the paging in the execute function.
Input: it takes in the process number of the pcb you are trying to change the paging for.
Output: none
*/
void execute_paging(uint32_t process_num)
{
    // 8000000/400000
    page_directory[VIRTUAL_START >> SHIFT] = ((process_num)*MB_4 + MB_8) | HEX_80 | HEX_7;
}

/*
This function will read the inputted bytes from a file into the buffer.
Inputs: fd is the file descriptor array, *buf is the buffer, and nbytes is the number of bytes to be read
Ouptut: output of selected read function in the jump table, or -1 if it fails
*/
int32_t read(int32_t fd, void *buf, int32_t nbytes)
{
    pcb_t *pcb = get_pcb(process_count); //gets current pcb
    if (buf == NULL || nbytes < 0)       //null check
        return -1;

    if (fd < 0 || fd > SEV || fd == 1) //checks if fd is in bounds
    {
        return -1;
    }
    if (!pcb->fdArr[fd].flags) //checks if pcb is running
    {
        return -1;
    }
    return pcb->fdArr[fd].sys_calls->read(fd, buf, nbytes);
}

/*
This function will write a certain number of bytes from the buffer to a file.
Inputs: fd is the file descriptor array, *buf is the buffer, and nbytes is the number of bytes to be written
Output: output of selected write function in the jump table, or -1 if it fails
*/
int32_t write(int32_t fd, const void *buf, int32_t nbytes)
{
    pcb_t *pcb = get_pcb(process_count); //gets current pcb
    if (buf == NULL)                     //null check
        return -1;
    if (fd < 0 || fd > SEV || fd == 0) //checks if fd is in bounds
    {
        return -1;
    }
    if (!pcb->fdArr[fd].flags) //checks if pcb is running
    {
        return -1;
    }
    return pcb->fdArr[fd].sys_calls->write(fd, buf, nbytes);
}

/*
This function opens the file related to the filename passed in.
Input: Filename of the file
Output: -1 if it fails, index of fd array if it opens
*/
int32_t open(const uint8_t *filename)
{
    if (filename == NULL || strlen((int8_t *)filename) == 0) //null check
        return -1;
    int i;
    // int fd;
    int open;
    pcb_t *pcb = get_pcb(process_count); //gets current pcb
    for (i = FDSTART; i < FILENUM; i++)  //within the fd bounds see if any pcb is running
    {
        if (!pcb->fdArr[i].flags)
        {
            open = i;
            break;
        }
    }
    if (i == FILENUM) //past fd bounds so return -1
    {
        return -1;
    }

    dentry_t temp;
    if (read_dentry_by_name(filename, &temp) == -1 || temp.fileType < 0 || temp.fileType > FDSTART) //checks if filename/file exists
    {
        return -1;
    }

    pcb->fdArr[open].sys_calls = function_table[temp.fileType];
    pcb->fdArr[open].inode = temp.iNodeNumber;
    pcb->fdArr[open].filePosition = 0;
    pcb->fdArr[open].flags = 1;

    return open;
}

/*
This function closes whatever needs to be closed.
Input: file descriptor array
Output: 0 if it closes succesfully, -1 if it fails.
*/
int32_t close(int32_t fd)
{
    pcb_t *pcb = get_pcb(process_count); //gets current pcb
    if (fd == NULL)
        return -1;
    if (fd < 0 || fd > SEV || fd == 1) //checks if fd is in bounds
    {
        return -1;
    }
    if (pcb->fdArr[fd].flags) //checks if pcb is running
    {
        pcb->fdArr[fd].flags = 0;
        return 0;
    }
    return -1;
}

/*
This function is used for stdin table in the jump table calls.
Input: fd is the file descriptor array, *buf is the buffer, and nbytes is the number of bytes
Output: -1 always
*/
int32_t invalid_sys_write(int32_t fd, const void *buffer, int32_t nbytes)
{
    return -1;
}

/*
This function is used for stdout table in the jump table calls.
Input: fd is the file descriptor array, *buf is the buffer, and nbytes is the number of bytes
Output: -1 always
*/
int32_t invalid_sys_read(int32_t fd, void *buffer, int32_t nbytes)
{
    return -1;
}

/*
This function takes in the buffer and bytes, copies the arguments from pcb into the buffer, returns 0 on success and -1 on fail;
Input: the buffer and the number of bytes
Output: returns 0 on successful attempt at obtaining arguments, returns -1 on fail.
*/
int32_t getargs(uint8_t *buf, int32_t nbytes)
{
    if (!buf || nbytes <= 0)                // null check 
    {
        return -1;
    }
    
    pcb_t *pcb = get_pcb(process_count);         // get current pcb
    strcpy((int8_t*)buf, (int8_t*)pcb->args);    // copy argument from pcb into buffer
    return 0;
}

/*
This function takes in a pointer to the start of the screen, maps the text mode video memory to a virtual address, and returns 0 on success and -1 on fail;
Input: pointer to the start of the screen
Output: returns -1 on fail and 0 on success
*/
int32_t vidmap(uint8_t **screen_start)
{
    if((uint32_t)screen_start < ONETWOEIGHTMB || (uint32_t)screen_start > (ONETHREETWOMB)){          // make sure screen starts within 128 and 132 MB in user space
        return -1;
    }

    if (screen_start == NULL)
    {
        return -1;                                                  // sanity check
    }
    uint32_t addr = VID;
  //  pcb_t * pcb = get_pcb(terminal[cur_terminal].cur_pcb + 1);
    terminal[cur_terminal].fishCheck = 1;
    
    int x;
    for(x=0;x<ONEKB;x++){
    // if (pcb->terminalID == cur_terminal)
    // {
    //     printf("%u", cur_terminal);
    //     addr = VID;
    // }
    // else if(pcb->terminalID == 0) {
    //     addr = TERMINAL1 + 7;
    // }
    // else if(pcb->terminalID == 1) {
    //     addr = TERMINAL2 + 7;
    // }
    // else if(pcb->terminalID == 2) {
    //     addr = TERMINAL3 + 7;
    // }
        if (x == 0)
        {
            video_table[x] = addr;                                   // set starting index to 
        }
        else {
            video_table[x] = (x*ONETHOUS) | MAXPROCESS;                 // check video table to make sure they have valid values
        }
    }
    page_directory[PDMAX] = ((uint32_t)video_table) | SEV;
    *screen_start = (uint8_t*)(ONETHREETWOMB);                                        // map virtual memory address to physical address
    return 0;
}

/*
This function takes in an integer and a pointer to the handler address and returns 0;
Inputs: signum is an integer and handler_address is a pointer to an address for a handler
Outputs: always 0
*/
int32_t set_handler(int32_t signum, void *handler_address)
{
    return 0;
}

/*
This function takes in no parameters and always returns 0
Inputs: none
Outputs: 0 always
*/
int32_t sigreturn(void)
{
    return 0;
}
