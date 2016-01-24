#include "flashberry.h"

jmp_buf jmp_context;

void delay_ms(int ms)
{
    clock_t initial_clock = clock();

    while((clock() - initial_clock) * 1000 < ms * CLOCKS_PER_SEC);
}

static bool write_flag = false, verify_flag = false;
static bool single_wire_flag = false, reset_flag = false, check_flag = false;
static float voltage = 5;

static void gpio_reset(void)
{
    gpio_open();

    gpio_configure(RESET_PIN, GPIO_OUT);

    gpio_set_state(RESET_PIN, GPIO_LO);

    delay_ms(1);

    gpio_set_state(RESET_PIN, GPIO_HI);

    gpio_configure(RESET_PIN, GPIO_IN);

    gpio_close();
}

int main(int argc, char * const argv[])
{
    printf("Flashberry v0.1 written by Hiroka Ihara (ihr486)\n");

    int c = 0;
    while(c >= 0) {
        switch(c = getopt(argc, argv, "f:vwl:shcr")) {
        case 'f':
            read_intel_hex(optarg);
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
        case 'r':
            reset_flag = true;
            break;
        case 'c':
            check_flag = true;
            break;
        }
    }

    int status = 0;
    if(!(status = setjmp(jmp_context))) {
        if(check_flag || write_flag || verify_flag) {
            uart_open();

            rl78g13_setup(single_wire_flag);

            rl78g13_baudrate_set(115200, voltage);

            rl78g13_reset();

            rl78g13_silicon_signature();

            if(write_flag) {
                rl78g13_write_all();
            }
            if(verify_flag) {
                rl78g13_verify_all();
            }

            uart_close();

            gpio_reset();
        } else if(reset_flag) {
            gpio_reset();
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
