#include "types.h"
#ifndef RTC_H
#define RTC_H

#define RTC_IO          0x70
#define CMOS_IO         0x71

extern void rtc_init(void);
extern void rtc_handler(void);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
