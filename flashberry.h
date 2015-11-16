#ifndef FLASHBERRY_H
#define FLASHBERRY_H

#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>

void gpio_open(int pin);
void gpio_set_direction(int pin, int direction);
int gpio_read(int pin);
void gpio_write(int pin, int state);
void gpio_close(int pin);

#endif
