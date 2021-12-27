#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
#include "lib.h"

#define THIRTYTWO 32
#define TWENTYFOUR 24
#define FIFTYTWO 52
#define SIXTYTHREE 63
#define ONEKB 1024
#define DBSIZE 1023
#define FOURKB 4096

//struct for each directry entry
typedef struct
{
    int8_t fileName[THIRTYTWO];
    int32_t fileType;
    int32_t iNodeNumber;
    int8_t reserved[TWENTYFOUR];
} dentry_t;

//struct for boot block
typedef struct
{
    int32_t dirEntry;
    int32_t iNodes;
    int32_t dataBlocks;
    int8_t reserved[FIFTYTWO];
    dentry_t dirEntries[SIXTYTHREE];
} bootBlock;

//struct for index nodes
typedef struct
{
    int32_t size;
    int32_t dataBlockArr[DBSIZE];
} iNode;

//struct for data blocks
typedef struct
{
    int32_t dataBlockArr[ONEKB];
} dataBlock;

extern uint8_t *mem_address;
extern bootBlock *boot; // boot block

int32_t openDirectory(const uint8_t *fileName);
int32_t readDirectory(int32_t offset, void *buf, int32_t length);
int32_t writeDirectory(int32_t fd, const void *buf, int32_t nbytes);
int32_t closeDirectory(int32_t file);

int32_t openFile(const uint8_t *fileName);
int32_t readFile(int32_t fd, void *buf, int32_t length);
int32_t writeFile(int32_t fd, const void *buf, int32_t nbytes);
int32_t closeFile(int32_t file);

extern int32_t filesystem_init(bootBlock *memory_address);
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);

#endif /* _FILESYS_H */
