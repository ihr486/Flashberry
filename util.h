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

inline void pack_LE16(uint8_t *buf, uint16_t val)
{
    buf[0] = val & 0x00FF;
    buf[1] = (val >> 8) & 0x00FF;
}

inline void pack_LE24(uint8_t *buf, uint32_t val)
{
    buf[0] = val & 0x0000FF;
    buf[1] = (val >> 8) & 0x0000FF;
    buf[2] = (val >> 16) & 0x0000FF;
}

inline void pack_LE32(uint8_t *buf, uint32_t val)
{
    buf[0] = val & 0x000000FF;
    buf[1] = (val >> 8) & 0x000000FF;
    buf[2] = (val >> 16) & 0x000000FF;
    buf[3] = (val >> 24) & 0x000000FF;
}

#endif
