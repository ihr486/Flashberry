#include "flashberry.h"

static const char *static_format(const char *format, ...)
{
    static char buffer[64];

    va_list ap;

    va_start(ap, format);

    vsnprintf(buffer, sizeof(buffer), format, ap);

    va_end(ap);

    return buffer;
}

void gpio_open(int pin)
{
    int fd = open("/sys/class/gpio/export", O_WRONLY);

    const char *name = static_format("%d", pin);

    write(fd, name, strlen(name));

    close(fd);
}

void gpio_set_direction(int pin, int direction)
{
    int fd = open(static_format("/sys/class/gpio/gpio%d/direction", pin), O_WRONLY);

    if(direction) {
        write(fd, "out", 3);
    } else {
        write(fd, "in", 2);
    }

    close(fd);
}

int gpio_read(int pin)
{
    int fd = open(static_format("/sys/class/gpio/gpio%d/value", pin), O_RDONLY);

    char state;

    read(fd, &state, 1);

    return (state == '0') ? 0 : 1;
}

void gpio_write(int pin, int state)
{
    int fd = open(static_format("/sys/class/gpio/gpio%d/value", pin), O_WRONLY);

    if(state) {
        write(fd, "1", 1);
    } else {
        write(fd, "0", 1);
    }

    close(fd);
}

void gpio_close(int pin)
{
    int fd = open("/sys/class/gpio/unexport", O_WRONLY);

    const char *name = static_format("%d", pin);

    write(fd, name, strlen(name));

    close(fd);
}
