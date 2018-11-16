#ifndef COMMON_H__INCLUDED
#define COMMON_H__INCLUDED

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "types.h"

#ifndef __cplusplus
typedef byte bool;
#define false ((byte)(0))
#define true  ((byte)(1))
#endif

#define nop() __asm__ __volatile__ ("nop")

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define SWAP16(x) ((x >> 8) | (x << 8))
#endif//COMMON_H__INCLUDED
