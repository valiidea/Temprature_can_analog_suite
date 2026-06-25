#ifndef FIRMWARE_TYPES_H
#define FIRMWARE_TYPES_H

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef unsigned long uintptr_t;

_Static_assert(sizeof(uint8_t) == 1, "uint8_t must be 8 bits");
_Static_assert(sizeof(int16_t) == 2, "int16_t must be 16 bits");
_Static_assert(sizeof(int32_t) == 4, "int32_t must be 32 bits");
_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 32 bits");
_Static_assert(sizeof(uintptr_t) == 4, "uintptr_t must be 32 bits on Cortex-M3");

#endif
