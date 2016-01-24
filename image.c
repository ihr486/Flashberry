#include "flashberry.h"

image_block_t *block_list = NULL, *cursor = NULL;

static void image_set_byte(uint32_t address, uint8_t byte)
{
    if(cursor && cursor->address <= address && address < cursor->address + BLOCK_SIZE) {
        cursor->data[address % BLOCK_SIZE] = byte;
    } else {
        if(!cursor || cursor->address > address) {
            cursor = block_list;
        }
        image_block_t *prev = NULL;
        for(; cursor; cursor = cursor->next) {
            if(cursor->address <= address && address < cursor->address + BLOCK_SIZE) {
                cursor->data[address % BLOCK_SIZE] = byte;
                return;
            }
            if(cursor->address > address) break;
            prev = cursor;
        }
        image_block_t *new_block = (image_block_t *)malloc(sizeof(image_block_t));
        if(!prev) {
            block_list = new_block;
            new_block->next = NULL;
        } else {
            new_block->next = prev->next;
            prev->next = new_block;
        }
        memset(new_block->data, BLANK_BYTE, BLOCK_SIZE);
        new_block->data[address % BLOCK_SIZE] = byte;
    }
}

const image_block_t *find_first_block(uint32_t low, uint32_t high)
{
    if(!cursor || cursor->address >= high) {
        cursor = block_list;
    }

    for(; cursor; cursor = cursor->next) {
        if(low <= cursor->address && cursor->address < high) {
            return cursor;
        }
    }
    return NULL;
}

static inline uint32_t read_hex4(FILE *fp)
{
    char c = fgetc(fp);

    if('0' <= c && c <= '9') {
        return c - '0';
    } else if('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else if('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

static inline uint32_t read_hex8(FILE *fp)
{
    return (read_hex4(fp) << 4) | read_hex4(fp);
}

static inline uint32_t read_hex16(FILE *fp)
{
    return (read_hex8(fp) << 8) | read_hex8(fp);
}

static inline uint32_t read_hex24(FILE *fp)
{
    return (read_hex8(fp) << 16) | read_hex16(fp);
}

static inline uint32_t read_hex32(FILE *fp)
{
    return (read_hex16(fp) << 16) | read_hex16(fp);
}

int read_intel_hex(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(!fp) return -1;

    uint32_t linear_address = 0, segment_address = 0;

    while(1) {
        int c = fgetc(fp);

        if(c == EOF) break;

        if(c == ':') {
            uint8_t data_length = read_hex8(fp);

            uint16_t offset = read_hex16(fp);

            uint8_t record_type = read_hex8(fp);

            switch(record_type) {
            case 0:
                for(unsigned int i = 0; i < data_length; i++) {
                    image_set_byte(linear_address + segment_address + offset + i, read_hex8(fp));
                }
                break;
            case 2:
                segment_address = read_hex16(fp) << 4;
                break;
            case 4:
                linear_address = read_hex16(fp) << 16;
                break;
            }
        }
    }

    fclose(fp);
    return 0;
}
