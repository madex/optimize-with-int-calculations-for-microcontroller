#ifndef ASM_LIB_H
#define ASM_LIB_H

#ifdef  __IAR_SYSTEMS_ICC__
typedef unsigned char  uint8_t;
typedef   signed char   int8_t;
typedef unsigned short uint16_t;
typedef   signed short  int16_t;
typedef unsigned long  uint32_t;
typedef   signed long   int32_t;
#else
#include <stdint.h>
#endif

uint16_t isqrt32(uint32_t n);
uint8_t  isqrt16(uint16_t n);
uint16_t ihypot(int16_t x, int16_t y);

#endif // ASM_LIB_H
