#include "filesys.h"
#include "types.h"
#include "lib.h"
#include "system_call.h"

bootBlock *boot;      //boot block
uint8_t *mem_address; // memory address for boot block

/*
This function initializes our bootblock
Input: The memory address of the bootblock
Output: it always returns 0, but it initializes the boot block.
*/
int32_t filesystem_init(bootBlock *memory_address)
{
    mem_address = (uint8_t *)memory_address;
    boot = memory_address; // sets the boot block to the first mem address
    return 0;
}

/*
This function finds the file by the name and puts it inside of a directory entry. 
It returns 0 if the file exists, and -1 if it does not.
Input: fname is the name of the file. dentry is an empty directory entry that will be filled. 
Output: 0 if the file exists, -1 if the file does not exist.
*/
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
    if (!boot || !fname) //checks if name or boot block doesnt exist
    {
        return -1;
    }
    int8_t temp[THIRTYTWO];
    strcpy(temp, (int8_t *)fname); //copies name into the array
    int x;
    uint32_t length = strlen((int8_t *)fname);
    for (x = length; x < THIRTYTWO; x++)
    {
        temp[x] = 0x0; // sets all values to 0 initially
    }
    if (strlen((int8_t *)fname) >= THIRTYTWO) // checks if the length is less than 32 characters
    {
        temp[THIRTYTWO] = (int8_t)('\0');
    }
    int y;
    for (y = 0; y < THIRTYTWO; y++)
    {
        if (0 == strncmp(boot->dirEntries[y].fileName, temp, THIRTYTWO - 1)) // checks if filename is the same as the temp
        {
            *dentry = boot->dirEntries[y]; // sets the name of the directory entry of the boot block
            return 0;
        }
    }
    return -1;
}

/*
This function finds if the file exists at a certain index and puts it into an empty directory entry.
Input: index is the index of the file you are looking for. dentry is an emtpy directory entry.
Output: 0 if the index of the file exists. -1 if it does not. 
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry)
{
    if (!boot || !dentry) // checks if boot block is empty
    {
        return -1;
    }
    if (index < 0 || index >= boot->dirEntry) // checks if index exists or if its within the bounds
    {
        return -1;
    }
    *dentry = boot->dirEntries[index]; //sets the index of the directory entry
    return 0;
}

/*
This function reads a file into the buffer.
Inputs: inode is the index node of the file. offset is the number of bytes that is needed to offset. 
        buf is the buffer. length is the number of bytes needed to be read.
Output: it outputs the number of bytes that are read or -1 if the file doesn't exist.
*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length)
{
    if (inode < 0 || offset < 0 || length < 0 || buf == NULL) // checks if inode, length, buffer, and offset exists
        return -1;
    uint32_t i;
    iNode *iNodePtr;
    uint32_t bOff = offset / FOURKB; // finds offset using modulus
    iNodePtr = (iNode *)boot + (1 + inode);
    uint32_t totalBytes = 0;

    if (iNodePtr->size - offset < length) // readjusts length if it is out of bounds
        length = iNodePtr->size - offset;
    uint32_t bNum = (length + offset) / FOURKB; // finds which block to start at

    for (i = bOff; i <= bNum; i++)
    {
        uint32_t end = FOURKB;
        uint32_t begin = 0; //iterates through to find the positions using this variable
        uint32_t data = iNodePtr->dataBlockArr[i];
        dataBlock *dataBlockArr = (1 + boot->iNodes + data) + (dataBlock *)boot;
        if ((i + 1) * FOURKB > offset + length) //updates last block if it increases
            end = (length + offset) - (FOURKB * i);

        if (i * FOURKB < offset) // if the offset is within the bounds then copy it
            begin = offset - (FOURKB * i);
        memcpy(totalBytes + (char *)buf, (char *)dataBlockArr + begin, -1 * (begin) + end); //copies the block into memory
        totalBytes = (end - begin) + totalBytes;                                            //updates the number of bytes
    }
    return totalBytes;
}

/*
this function opens the file in the filesystem.
Inputs: 
Outputs: it outputs 0 if the filename exists, and -1 if it doesnt
*/
int32_t openFile(const uint8_t *fileName)
{
    dentry_t fileDentry;
    if (read_dentry_by_name(fileName, &fileDentry) == 0) //checks if directory entry with fileName exists
    {
        return 0;
    }
    return -1; // this means that the name did not exist
}

/*
this function reads the file in the filesystem
Inputs: inode is the index node of the file. offset is the number of bytes that is needed to offset. 
        buf is the buffer. length is the number of bytes needed to be read.
Output: returns 0 if the file was read succesfully, and -1 otherwise
*/
int32_t readFile(int32_t fd, void *buf, int32_t length)
{
    if (boot == NULL) // checks if boot block exists
        return -1;
    if (buf == NULL)
        return -1;
    pcb_t* curr_pcb = get_pcb(process_count);
    fd_t * file_descriptor = &curr_pcb->fdArr[fd];
        
    int32_t bytes = 0;
    bytes = read_data(file_descriptor->inode, file_descriptor->filePosition, buf, length); // finds the number of bytes needed
    if (bytes <= 0)
        return 0;
    file_descriptor->filePosition += bytes;

    return bytes;
}

/*
This function writes the file but in reality does not do anything.
Inputs: inode, offset, buf, length
Outputs: -1 always 
*/
int32_t writeFile(int32_t fd, const void *buf, int32_t nbytes)
{
    return -1;
}

/*
This function closes the file but in reality does not do anything.
Inputs: file which is the file itself
Outputs: 0 always 
*/
int32_t closeFile(int32_t file)
{
    return 0;
}

/*
This function opens the directory given a file name. It uses read_dentry_by_name.
Input: filename is the name of the directory that needs to be opened.
Output: returns 0 if the diredctory exists, otherwise -1.
*/
int32_t openDirectory(const uint8_t *fileName)
{
    dentry_t dirDentry;
    if (read_dentry_by_name(fileName, &dirDentry) == 0) //checks if directory entry with fileName exists
    {
        return 0;
    }
    return -1; // name doesnt exist
}

/*
This function reads the next file from the directory.
Inputs: offset is the number of bytes that is needed to offset. 
        buf is the buffer. length is the number of bytes needed to be read.
Output: -1 if there is a failure while reading, otherwise it returns the number of bytes read.
*/
int32_t readDirectory(int32_t fd, void *buf, int32_t nbytes)
{
    buf = (uint8_t*)(buf); 
    memset(buf, 0x20, nbytes); // set all to zero
    if (boot == NULL || !buf) // checks if offset is within bounds, bootblock exists, and buffer exists
        return -1;

    pcb_t* curr_pcb;
    curr_pcb = get_pcb(process_count);
    uint32_t directory_index = curr_pcb->fdArr[fd].filePosition;
    curr_pcb->fdArr[fd].filePosition += 1;

    // printf(" name : %s",boot->dirEntries[*directory_index].fileName);

    strncpy((int8_t *)buf, (int8_t *)(boot->dirEntries[directory_index].fileName), nbytes); // copies the filename to the buffer
    // printf("%u\n", strlen(dentry.fileName));
    int length = strlen(buf);
    if (length > 32)
    {
        length = 32;
    }        
    return length;
}


/*
This function writes the directory but in reality does not do anything.
Inputs: inode, offset, buf, length
Outputs: -1 always 
*/
int32_t writeDirectory(int32_t fd, const void *buf, int32_t nbytes)
{
    return -1;
}

/*
This function closes the directory but in reality does not do anything.
Inputs: file which is the file itself
Outputs: 0 always 
*/
int32_t closeDirectory(int32_t file)
{
    return 0;
}
