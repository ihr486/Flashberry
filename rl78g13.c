#include "flashberry.h"
#include "util.h"

static uint32_t code_flash_start = 0, code_flash_end = 0;
static uint32_t data_flash_start = 0xF1000, data_flash_end = 0;

typedef enum {
    RL78_COMMAND_PACKET,
    RL78_DATA_BODY,
    RL78_DATA_TRAILER
} rl78_packet_type_t;

static uint8_t *create_command_packet(int size, uint8_t command)
{
    uint8_t *packet = (uint8_t *)malloc(size + 5);

    packet[0] = 0x01;
    packet[1] = (uint8_t)(size + 1);
    packet[2] = command;

    packet[3 + size + 1] = 0x03;
    return packet;
}

static uint8_t *create_data_packet(int size, bool trailer_flag)
{
    uint8_t *packet = (uint8_t *)malloc(size + 4);

    packet[0] = 0x02;
    packet[1] = (uint8_t)size;
    
    if(trailer_flag) {
        packet[2 + size + 1] = 0x03;
    } else {
        packet[2 + size + 1] = 0x17;
    }
    return packet;
}

static void destroy_packet(void *packet)
{
    free(packet);
}

static void send_packet(uint8_t *packet)
{
    int n = (int)(packet[1] - 1) + 1;
    packet[n + 2] = 0;
    for(int i = 0; i < n + 1; i++) {
        packet[n + 2] -= packet[1 + i];
    }
    for(int i = 0; i < n + 4; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");
    uart_write_bytes(packet, n + 4);
    destroy_packet(packet);
}

static uint8_t *receive_packet(void)
{
    uint8_t type = 0;

    while(type != 0x01 && type != 0x02) {
        type = uart_read_byte();
    }

    uint8_t length = uart_read_byte();

    int n = ((length - 1) & 0xFF) + 1;

    uint8_t *packet = (uint8_t *)malloc(n + 4);

    packet[0] = type;
    packet[1] = length;

    uart_read_bytes(&packet[2], n + 2);

    uint8_t checksum = 0;

    for(int i = 1; i < n + 3; i++) {
        checksum += packet[i];
    }

    if(checksum) {
        longjmp(jmp_context, ERROR_CHECKSUM);
    }

    for(int i = 0; i < n + 4; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");

    return packet;
}

static uint8_t check_status(uint8_t code)
{
    switch(code) {
    case 0x04:
        fprintf(stderr, "Target reported status 0x04: \"Unsupported command\".\n");
        break;
    case 0x05:
        fprintf(stderr, "Target reported status 0x05: \"Illegal parameter\".\n");
        break;
    case 0x06:
        return 0x06;
    case 0x07:
        fprintf(stderr, "Target reported status 0x07: \"Checksum mismatch\".\n");
        break;
    case 0x0F:
        fprintf(stderr, "Target reported status 0x0F: \"Verify error\".\n");
        break;
    case 0x10:
        fprintf(stderr, "Target reported status 0x10: \"Protection error\".\n");
        break;
    case 0x15:
        return 0x15;
    case 0x1A:
        fprintf(stderr, "Target reported status 0x1A: \"Erase error\".\n");
        break;
    case 0x1B:
        return 0x1B;
    case 0x1C:
        fprintf(stderr, "Target reported status 0x1C: \"Write error\".\n");
        break;
    default:
        fprintf(stderr, "Target reported unknown status.\n");
        break;
    }
    longjmp(jmp_context, ERROR_PROTOCOL);
}

void rl78g13_reset(void)
{
    uint8_t *command = create_command_packet(0, 0x00);

    send_packet(command);

    uint8_t *status = receive_packet();

    check_status(status[2]);

    destroy_packet(status);

    printf("Reset complete.\n");
}

void rl78g13_baudrate_set(int baudrate, float voltage)
{
    uint8_t *command = create_command_packet(2, 0x9A);

    switch(baudrate) {
    case 115200:
        command[3] = 0;
        break;
    case 250000:
        command[3] = 1;
        break;
    case 500000:
        command[3] = 2;
        break;
    case 1000000:
        command[3] = 3;
        break;
    default:
        longjmp(jmp_context, ERROR_BAUDRATE);
    }

    command[4] = (int)(voltage * 10.0f);

    send_packet(command);

    uint8_t *status = receive_packet();

    check_status(status[2]);

    uart_set_baudrate(baudrate);

    destroy_packet(status);

    printf("Baudrate set complete.\n");
}

void rl78g13_setup(bool single_wire_flag)
{
    gpio_open();
    uart_close();

    gpio_configure(RESET_PIN, GPIO_OUT);
    gpio_configure(TXD_PIN, GPIO_OUT);

    gpio_set_state(RESET_PIN, GPIO_LO);

    delay_ms(1);

    gpio_set_state(TXD_PIN, GPIO_LO);

    delay_ms(1);

    gpio_set_state(RESET_PIN, GPIO_HI);

    delay_ms(1);

    gpio_set_state(TXD_PIN, GPIO_HI);

    delay_ms(1);

    gpio_configure(RESET_PIN, GPIO_IN);
    gpio_configure(TXD_PIN, 4);

    gpio_close();
    uart_open();

    delay_ms(1);

    if(single_wire_flag) {
        uart_write_byte(0x3A);
    } else {
        uart_write_byte(0x00);
    }

    delay_ms(1);

    printf("Setup complete.\n");
}

void rl78g13_silicon_signature(void)
{
    uint8_t *command = create_command_packet(0, 0xC0);

    send_packet(command);

    uint8_t *status = receive_packet();

    check_status(status[2]);

    destroy_packet(status);

    uint8_t *data = receive_packet();

    uint32_t device_code = unpack_LE24(&data[2]);

    code_flash_end = unpack_LE24(&data[15]);
    data_flash_end = unpack_LE24(&data[18]);

    uint8_t device_name[11];
    memcpy(device_name, &data[5], 10);

    device_name[10] = 0;

    printf("Device information:\n\tCode = %06X\n\tName = %s\n\tFirmware version = %01hhu.%01hhu%01hhu\n", device_code, device_name, data[21], data[22], data[23]);
    printf("\tCode Flash : 0x%05X -> 0x%05X\n\tData Flash : 0x%05X -> 0x%05X\n", code_flash_start, code_flash_end, data_flash_start, data_flash_end);
}
