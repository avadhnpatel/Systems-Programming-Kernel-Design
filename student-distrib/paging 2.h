#include "types.h"
#define ONEKB 1024
#define FOURKB 4096
#define RAND 0xB8
#define VIDADD 0x0B8000
#define RW 0x2             //enables read write
#define P 0x3              // enables read/write and says that the address is from valid pde
#define PAGESPACE 0x400083 // address for page that allocates 4mb for page
#define MEMADD 0x00400000
#define VIDMEMADD 0x000B8000
#define MASK 0x003FF000
#define PDEDEFAULT 0x00000082
#define ADDYONE 0xB9000
#define PGINDONE 0xB9
#define ADDYTWO 0xBA000
#define PGINDTWO 0xBA
#define ADDYTHREE 0xBB000
#define PGINDTHREE 0xBB

//page directory struct
// typedef struct pageDirectory
// {
//     union {
//         uint32_t val;
//         struct
//         { //for 4kb pte
//             uint8_t presK : 1;
//             uint8_t rwK : 1;
//             uint8_t userSuperK : 1;
//             uint8_t writeK : 1;
//             uint8_t cacheDisK : 1;
//             uint8_t accessK : 1;
//             uint8_t reserveK : 1;
//             uint8_t pageSizeK : 1;
//             uint8_t globalPageK : 1;
//             uint8_t availableK : 3;
//             uint32_t ptBaseAddrK : 20;
//         } __attribute__((packed));
//         struct
//         { //for 4mb page
//             uint8_t presM : 1;
//             uint8_t rwM : 1;
//             uint8_t userSuperM : 1;
//             uint8_t writeM : 1;
//             uint8_t cacheDisM : 1;
//             uint8_t accessM : 1;
//             uint8_t dirtyM : 1;
//             uint8_t pageSizeM : 1;
//             uint8_t globalPageM : 1;
//             uint8_t availableM : 3;
//             uint8_t ptAttrIndexM : 1;
//             uint16_t reserveM : 9;
//             uint16_t pageBaseAddM : 10;
//         } __attribute__((packed));
//     };
// } pageDirectory_t;

// typedef struct pageTable
// {
//     union {
//         uint32_t val;
//         struct
//         { //for 4kb pte
//             uint8_t pres : 1;
//             uint8_t rw : 1;
//             uint8_t userSuper : 1;
//             uint8_t write : 1;
//             uint8_t cacheDis : 1;
//             uint8_t access : 1;
//             uint8_t reserve : 1;
//             uint8_t pageSize : 1;
//             uint8_t globalPage : 1;
//             uint8_t available : 3;
//             uint32_t pageBaseAdd : 20;
//         } __attribute__((packed));
//     };
// } pageTable_t;

// extern pageTable_t page_table[ONEKB] __attribute__((aligned(FOURKB)));
// extern pageDirectory_t page_directory[ONEKB] __attribute__((aligned(FOURKB)));
extern uint32_t page_table[ONEKB] __attribute__((aligned(FOURKB)));
extern uint32_t page_directory[ONEKB] __attribute__((aligned(FOURKB)));

void paging_init();
