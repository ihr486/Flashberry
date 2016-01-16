#ifndef FLASHBERRY_H
#define FLASHBERRY_H

#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <time.h>
#include <setjmp.h>

#define GPIO_IN (0)
#define GPIO_OUT (1)
#define GPIO_LO (0)
#define GPIO_HI (1)

void gpio_open(int pin);
void gpio_set_direction(int pin, int direction);
int gpio_read(int pin);
void gpio_write(int pin, int state);
void gpio_close(int pin);

#define BLOCK_SIZE (1024)
#define BLANK_BYTE (0xFF)

typedef struct image_block_tag {
    struct image_block_tag *next;
    uint32_t address;
    uint8_t data[BLOCK_SIZE];
} image_block_t;

int read_intel_hex(const char *filename);
int read_s_record(const char *filename);
const image_block_t *find_first_block(uint32_t low, uint32_t high);

extern jmp_buf jmp_context;

void delay_ms(int ms);

void rl78g13_setup(int port, bool single_wire_flag);

#define RESET_PIN (10)

enum {
    ERROR_TARGET = 1,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM
};

#endif
