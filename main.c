#define _XOPEN_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>

int main(int argc, char * const argv[])
{
    unsigned int baud = 115200;
    const char *device = "/dev/ttyAMA0";
    const char *image = "-";
    const char *target = "rl78g13";

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
    newtio.c_cflag = (B115200 | CS8 | CLOCAL | CREAD);
    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    ioctl(port, TCSETS, &newtio);

    if(!strcmp(target, "rl78g13")) {
    } else {
        fprintf(stderr, "Unknown target: %s.\n", target);
        return 1;
    }

    return 0;
}
