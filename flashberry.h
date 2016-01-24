#ifndef FLASHBERRY_H
#define FLASHBERRY_H

#define _XOPEN_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
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
#include <sys/mman.h>

#define GPIO_IN (0)
#define GPIO_OUT (1)
#define GPIO_LO (0)
#define GPIO_HI (1)

void gpio_open(void);
void gpio_close(void);
void gpio_configure(int pin, int function);
void gpio_set_state(int pin, int state);

void uart_open(void);
void uart_close(void);
int uart_read_bytes(void *buf, int n);
int uart_write_bytes(void *buf, int n);
uint8_t uart_read_byte(void);
void uart_write_byte(uint8_t c);

#define BLOCK_SIZE (1024)
#define BLANK_BYTE (0xFF)

typedef struct image_block_tag {
    struct image_block_tag *next;
    uint32_t address;
    uint8_t data[BLOCK_SIZE];
} image_block_t;

int read_intel_hex(const char *filename);
const image_block_t *find_first_block(uint32_t low, uint32_t high);

extern jmp_buf jmp_context;

void delay_ms(int ms);

void rl78g13_setup(float voltage, bool single_wire_flag);
void rl78g13_write_all(void);
void rl78g13_verify_all(void);

#define RESET_PIN (4)
#define TXD_PIN (14)

enum {
    ERROR_TARGET = 1,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM,
    ERROR_BAUDRATE,
    ERROR_PROTOCOL,
    ERROR_IO
};

#endif
