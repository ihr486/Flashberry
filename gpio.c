#include "flashberry.h"

typedef volatile struct
{
    uint32_t GPFSEL0;
    uint32_t GPFSEL1;
    uint32_t GPFSEL2;
    uint32_t GPFSEL3;
    uint32_t GPFSEL4;
    uint32_t GPFSEL5;
    uint32_t reserved0;
    uint32_t GPSET0;
    uint32_t GPSET1;
    uint32_t reserved1;
    uint32_t GPCLR0;
    uint32_t GPCLR1;
} GPIO_Struct;

static GPIO_Struct *GPIO;

void gpio_open(void)
{
    int fd = open("/dev/mem", O_RDWR|O_SYNC);
    if(fd < 0) {
        fprintf(stderr, "Failed to open /dev/mem.\n");
        longjmp(jmp_context, ERROR_IO);
    }

    GPIO = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
    if(GPIO == MAP_FAILED) {
        fprintf(stderr, "Failed to map GPIO registers.\n");
        longjmp(jmp_context, ERROR_IO);
    }

    close(fd);
}

void gpio_close(void)
{
    munmap((void *)GPIO, 4096);
}

void gpio_configure(int pin, int function)
{
    if(0 <= pin && pin < 10) {
        GPIO->GPFSEL0 &= ~(7 << (pin * 3));
        GPIO->GPFSEL0 |= (function & 7) << (pin * 3);
    } else if(10 <= pin && pin < 20) {
        GPIO->GPFSEL1 &= ~(7 << ((pin - 10) * 3));
        GPIO->GPFSEL1 |= (function & 7) << ((pin - 10) * 3);
    } else if(20 <= pin && pin < 30) {
        GPIO->GPFSEL2 &= ~(7 << ((pin - 20) * 3));
        GPIO->GPFSEL2 |= (function & 7) << ((pin - 20) * 3);
    } else if(30 <= pin && pin < 40) {
        GPIO->GPFSEL3 &= ~(7 << ((pin - 30) * 3));
        GPIO->GPFSEL3 |= (function & 7) << ((pin - 30) * 3);
    } else if(40 <= pin && pin < 50) {
        GPIO->GPFSEL4 &= ~(7 << ((pin - 40) * 3));
        GPIO->GPFSEL4 |= (function & 7) << ((pin - 40) * 3);
    } else if(50 <= pin && pin <= 53) {
        GPIO->GPFSEL5 &= ~(7 << ((pin - 50) * 3));
        GPIO->GPFSEL5 |= (function & 7) << ((pin - 50) * 3);
    }
}

void gpio_set_state(int pin, int state)
{
    if(state) {
        if(0 <= pin && pin < 32) {
            GPIO->GPSET0 = 1 << pin;
        } else if(32 <= pin && pin <= 53) {
            GPIO->GPSET1 = 1 << (pin - 32);
        }
    } else {
        if(0 <= pin && pin < 32) {
            GPIO->GPCLR0 = 1 << pin;
        } else if(32 <= pin && pin <= 53) {
            GPIO->GPCLR1 = 1 << (pin - 32);
        }
    }
}
