#ifndef THINKRNL_STDINT_H
#define THINKRNL_STDINT_H

// Defines a bunch of integer types.
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647

#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483648)

#define UINT8_MAX 255
#define UINT16_MAX 65535
#define UINT32_MAX 4294967296U

#endif