#include "flashberry.h"

jmp_buf jmp_context;

void delay_ms(int ms)
{
    clock_t initial_clock = clock();

    while((clock() - initial_clock) * 1000 < ms * CLOCKS_PER_SEC);
}

static unsigned int baud = 115200;
static char image[256] = "";
static char target[16] = "none";

static bool erase_flag = false, write_flag = false, verify_flag = false;
static bool single_wire_flag = false;
static float voltage = 5;

int main(int argc, char * const argv[])
{
    int c = 0;
    while(c >= 0) {
        switch(c = getopt(argc, argv, "b:d:f:t:vwel:s")) {
        case 'b':
            baud = atoi(optarg);
            break;
        case 'f':
            strcpy(image, optarg);
            break;
        case 't':
            strcpy(target, optarg);
            break;
        case 'e':
            erase_flag = true;
            break;
        case 'w':
            write_flag = true;
            break;
        case 'v':
            verify_flag = true;
            break;
        case 'l':
            voltage = atof(optarg);
            break;
        case 's':
            single_wire_flag = true;
            break;
        }
    }

    int status = 0;
    if(!(status = setjmp(jmp_context))) {
        if(!strcmp(target, "rl78g13")) {
            rl78g13_setup(single_wire_flag);

            rl78g13_baudrate_set(baud, voltage);

            rl78g13_reset();

            rl78g13_silicon_signature();
        } else {
            longjmp(jmp_context, ERROR_TARGET);
        }
    } else {
        switch(status) {
        case ERROR_TARGET:
            fprintf(stderr, "Unsupported target.\n");
            break;
        case ERROR_TIMEOUT:
            fprintf(stderr, "Target does not respond.\n");
            break;
        case ERROR_CHECKSUM:
            fprintf(stderr, "Wrong checksum on inward packet.\n");
            break;
        }
    }

    return 0;
}
