#include "flashberry.h"

#define TIMEOUT_MS (2000)

static int port = -1;
static struct termios oldtio;
static const char device[] = "/dev/ttyAMA0";

void uart_open(void)
{
    port = open(device, O_RDWR | O_NONBLOCK | O_NOCTTY);
    if(port < 0) {
        fprintf(stderr, "Failed to open %s.\n", device);
        longjmp(jmp_context, ERROR_IO);
    }

    struct termios newtio;
    tcgetattr(port, &oldtio);
    newtio = oldtio;
    newtio.c_iflag = IGNPAR | IGNBRK;
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VMIN] = 0;
    newtio.c_cc[VTIME] = 0;
    cfsetspeed(&newtio, 115200);
    tcsetattr(port, TCSAFLUSH, &newtio);
}

int uart_read_bytes(void *buf, int n)
{
    clock_t initial_clock = clock();
    for(int i = 0; i < n;) {
        i += read(port, (uint8_t *)buf + i, n - i);

        if((clock() - initial_clock) * 1000 > TIMEOUT_MS * CLOCKS_PER_SEC) {
            longjmp(jmp_context, ERROR_TIMEOUT);
        }
    }
    printf("%d bytes read.\n", n);
    return n;
}

int uart_write_bytes(void *buf, int n)
{
    clock_t initial_clock = clock();
    for(int i = 0; i < n;) {
        i += write(port, (uint8_t *)buf + i, n - i);

        if((clock() - initial_clock) * 1000 > TIMEOUT_MS * CLOCKS_PER_SEC) {
            longjmp(jmp_context, ERROR_TIMEOUT);
        }
    }
    printf("%d bytes written.\n", n);
    return n;
}

uint8_t uart_read_byte(void)
{
    uint8_t ret;
    uart_read_bytes(&ret, 1);
    return ret;
}

void uart_write_byte(uint8_t c)
{
    uart_write_bytes(&c, 1);
}

void uart_close(void)
{
    if(port >= 0) {
        tcsetattr(port, TCSAFLUSH, &oldtio);
        close(port);
    }
}

void uart_set_baudrate(int rate)
{
    struct termios newtio;
    tcgetattr(port, &newtio);
    cfsetspeed(&newtio, rate);
    tcsetattr(port, TCSAFLUSH, &newtio);
}
