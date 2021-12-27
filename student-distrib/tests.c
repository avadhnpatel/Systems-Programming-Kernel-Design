#include "tests.h"
#include "x86_desc.h"
#include "idt_assembly_linkage.h"
#include "keyboard.h"
#include "rtc.h"
#include "lib.h"
#include "terminal.h"
#include "filesys.h"
#include "paging.h"
#include "system_call.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER \
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result) \
	printf("\n[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure()
{
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

void rtc_handler();
void rtc_test();

/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test()
{
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i)
	{
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL))
		{
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

int division_test(void)
{
	TEST_HEADER;

	int i = 12;
	int j = 0;

	int output;

	output = i / j;

	return FAIL;
}

int overflow_testcase(void)
{
	overflow_test();

	return FAIL;
}

int page_val_test(void){
	TEST_HEADER;
	uint32_t *val = (uint32_t *)0x400001;
	uint32_t x = *val;
	x = x+ 1;
	return 1;
}

int page_invalid_test(void){
	TEST_HEADER;
	uint32_t *inval = (uint32_t *)0x390000;
	uint32_t x = *inval;
	x = x + 1;
	return 0;
}

int syscall_testcase(void)
{
	system_call_test();

	return FAIL;
}

int rtc_testcase(void)
{
	rtc_test();

	return FAIL;
}

// NO TESTS FOR PAGING AS DID NOT FINISH

/* Checkpoint 2 tests */

int terminal_write_testcase(void)
{
	TEST_HEADER;

	uint8_t test_string[13] = "TEAM DANIEL!\n"; // 13 is length of string
	int result = FAIL;
	if (terminal_write(0, test_string, 13) == 13)
		result = PASS;
	return result;
}

int terminal_read_testcase_length(void)
{
	TEST_HEADER;
	printf("press enter to run test\n"); // 128 is max buffer size
	uint8_t test_string[128];
	unsigned int ret;
	memset(test_string, 0, 128); // clear
	ret = terminal_read(0, test_string, 127);
	if ((ret != 128) | (test_string[127] != '\n'))
	{
		return FAIL;
	}
	return PASS;
}

int terminal_read_testcase_strings(void)
{
	TEST_HEADER;

	int8_t test_string[128];
	printf("Type: 'type this' and press enter\n");
	uint32_t ret = terminal_read(0, test_string, 10); // length of string to type
	test_string[ret] = '\0';
	//printf("input_length: %d, input string: %s\n", ret, test_string);
	if (strncmp(test_string, "type this", ret) == 0)
	{
		return PASS;
	}
	return FAIL;
}

int terminal_read_write_test(void)
{
	TEST_HEADER;
	char buf[128];
	TEST_HEADER;
	printf("type q and then enter to quit\n");
	while (1)
	{
		terminal_read(0, buf, 128);
		if (((uint8_t*)buf)[0] == 'q')
		{
			clear();
			return PASS;
		}
		
		terminal_write(0,buf,128);
	}
}

int keyboard_buffer_size(void)
{
	TEST_HEADER;
	terminal_t terminal[3];
	int cur_terminal = 0;
	unsigned int i;
	for (i = 0; i < 200; i++) // emulate 200 keyboard presses
	{
		handle_key(0x11); // print char
	}
	printf("\nBuffer has: %u indices\n", terminal[cur_terminal].buffer_index);
	if (terminal[cur_terminal].buffer_index > 127) // check if it passes max size
	{
		return FAIL;
	}

	return PASS;
}

int read_dentry_idx_testcase(void)
{
	TEST_HEADER;

	dentry_t dentry;
	int32_t ret = read_dentry_by_index(1, &dentry);
	if (ret == 0 && 0 == strncmp(dentry.fileName, "sigtest", THIRTYTWO))
		return PASS;
	else
		return FAIL;
}

int read_dentry_name_testcase(void)
{
	TEST_HEADER;
	dentry_t dentry;
	int32_t ret = read_dentry_by_name((uint8_t *)"frame0.txt", &dentry);
	if (ret == -1)
		return FAIL;
	uint8_t buf[ONEKB];
	int32_t ret_val = read_data(dentry.iNodeNumber, 0, buf, ONEKB);
	unsigned int i;
	for (i = 0; i < ret_val; i++)
	{
		if (buf[i] != '\0')
			printf("%c", buf[i]);
	}
	return PASS;
}

int read_exec_testcase(void)
{
	// TEST_HEADER;
	dentry_t dentry;
	int32_t ret = read_dentry_by_name((uint8_t *)"ls", &dentry);
	if (ret == -1)
		return FAIL;
	uint8_t buf[8000];
	int32_t ret_val = read_data(dentry.iNodeNumber, 0, buf, 8000);
	if(ret_val == -1){
		return FAIL;
	}
	unsigned int i;
	unsigned int count = 0;
	for (i = 0; i < 8000; i++)
	{
		// if (buf[i] != '\0')
		// 	printf("%c", buf[i]);
		if (buf[i] != '\0' && buf[i] != '\n'){
			putc(buf[i], cur_terminal);
			count++;
			if(count%80 == 0)
				putc('\n', cur_terminal);

		}
	}
	printf("\n");
	return PASS;
}

int read_file_testcase(void)
{
	// TEST_HEADER;
	// uint8_t buf[ONEKB];
	// int32_t ret_val = readFile((uint8_t *)"frame1.txt", 0, buf, ONEKB);
	// if (ret_val == -1)
	// {
	// 	return FAIL;
	// }
	// unsigned int i;
	// for (i = 0; i < ret_val; i++)
	// {
	// 	if (buf[i] != '\0')
	// 		printf("%c", buf[i]);
	// }
	// printf("\nfile_name: frame1.txt\n");
	return PASS;
}

int read_dir_testcase(void)
{
	TEST_HEADER;
	unsigned int i;
	uint8_t buf[THIRTYTWO + 1];
	dentry_t dentry;
	int32_t ret = -1;
	buf[THIRTYTWO] = '\0';
	uint8_t *iNode;
	int32_t filesize;

	for (i = 0; i < 17; i++)
	{
		int32_t length = readDirectory(i, buf, THIRTYTWO);
		if (length == 0)
			return FAIL;
		if (length > THIRTYTWO)
			return FAIL;
		buf[length] = '\0';
		ret = read_dentry_by_name(buf, &dentry);
		if (ret == -1)
			return FAIL;
		iNode = (uint8_t *)(mem_address + (1 + dentry.iNodeNumber) * FOURKB);
		filesize = *((uint32_t *)iNode);

		printf("file_name: %s  ,  ", buf);
		printf("file_type: %u , ", dentry.fileType);
		printf("file_size: %d\n", filesize);
	}

	return PASS;
}

int rtc_test_full()
{
	int i;
	uint32_t frequency = 1;

	clear();
	frequency = 1;
	printf("OPEN test: freq = 2 \n");
	i = 16;
	rtc_open(0);
	while (i > 0)
	{
		putc('l', cur_terminal);
		rtc_read(0, &frequency, 4);
		i--;
	}
	clear();

	rtc_close(0);

	int x;
	x = 0;
	for (frequency = 2; frequency <= 1024; frequency *=2)
	{	
		x++;
		clear();
		printf("WRITE READ TEST \n");
		printf("frequency at %d Hz \n", frequency);

		rtc_write(0, &frequency, 4);
		i = 8 * x;
		while (i > 0)
		{
			putc('l', cur_terminal);
			rtc_read(0, &frequency, 4);
			i--;
		}
	}

	clear();
	printf("WRITE corner case test freq = 3 \n ");
	frequency = 3;
	printf("%d \n", rtc_write(0, &frequency, 4));

	return PASS;
}

/* Checkpoint 3 tests */
int execute_testcase(void)
{
	TEST_HEADER;
	int32_t ret_val = execute((uint8_t*)"fish 5");
	if(ret_val == -1)
		return FAIL;
	return PASS;
}



/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests()
{
	// launch your tests here

	// checkpoint 1
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("divide by zero", division_test());
	// TEST_OUTPUT("overflow check test", overflow_testcase());
	// TEST_OUTPUT("system call check test", syscall_testcase());
	// TEST_OUTPUT("rtc check test", rtc_test_full());

	// checkpoint 2
	//TEST_OUTPUT("terminal write test", terminal_write_testcase());
	//TEST_OUTPUT("terminal read test", terminal_read_testcase_strings());
	//TEST_OUTPUT("terminal read test", terminal_read_testcase_length());
	//TEST_OUTPUT("keyboard buffer test", keyboard_buffer_size());
	// TEST_OUTPUT("read dentry index", read_dentry_idx_testcase());
	// TEST_OUTPUT("read executable", read_exec_testcase());
	// read_exec_testcase();
	// TEST_OUTPUT("read dentry name", read_dentry_name_testcase());
	// TEST_OUTPUT("read file test", read_file_testcase());
	// TEST_OUTPUT("read directory test", read_dir_testcase());
	// TEST_OUTPUT("rtc test", rtc_test_full());
	// TEST_OUTPUT("Terminal read/write test", terminal_read_write_test());
	// TEST_OUTPUT("Execute testcase", execute_testcase());
	// TEST_OUTPUT("Paging Valid Test", page_val_test());
	// TEST_OUTPUT("Paging Invalid Test", page_invalid_test());


}
