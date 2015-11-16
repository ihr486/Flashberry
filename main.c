#include "flashberry.h"

int main(int argc, char * const argv[])
{
    unsigned int baud = 115200;
    const char *device = "/dev/ttyAMA0";
    const char *image = "-";
    const char *target = "none";

    int c = 0;
    while(c >= 0) {
        switch(c = getopt(argc, argv, "b:d:f:t:")) {
        case 'b':
            baud = atoi(optarg);
            break;
        case 'd':
            device = optarg;
            break;
        case 'f':
            image = optarg;
            break;
        case 't':
            target = optarg;
            break;
        }
    }

    int port = open(device, O_RDWR);
    if(!port) {
        fprintf(stderr, "Failed to open %s.\n", device);
        return 1;
    }

    struct termios newtio, oldtio;
    ioctl(port, TCGETS, &oldtio);
    newtio = oldtio;
    newtio.c_iflag = 0;
    newtio.c_cflag = (B115200 | CS8 | CLOCAL | CREAD);
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    ioctl(port, TCSETS, &newtio);

    if(!strcmp(target, "rl78g13")) {
        gpio_open(10);

        gpio_set_direction(10, 1);

        gpio_write(10, 0);

        tcsendbreak(port, 2);

        gpio_write(10, 1);

        gpio_close(10);
    } else if(!strcmp(target, "none")) {
        fprintf(stderr, "Please specify a target device.\n");
        return 1;
    } else {
        fprintf(stderr, "Unknown target: %s.\n", target);
        return 1;
    }

    return 0;
}
