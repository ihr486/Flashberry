#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

inline uint16_t unpack_LE16(const uint8_t *buf)
{
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

inline uint32_t unpack_LE24(const uint8_t *buf)
{
    return (uint32_t)unpack_LE16(buf) | ((uint32_t)buf[2] << 16);
}

inline uint32_t unpack_LE32(const uint8_t *buf)
{
    return (uint32_t)unpack_LE16(buf) | ((uint32_t)unpack_LE16(buf + 2) << 16);
}

#endif
