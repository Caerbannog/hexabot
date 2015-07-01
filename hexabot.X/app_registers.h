
#ifndef APP_REGISTERS_H
#define	APP_REGISTERS_H

#include <stdint.h>

typedef struct {
    uint8_t * addr;
    uint8_t size;
} register_t;

extern register_t registers[];

#define REG_VAR(var)     { (uint8_t *)&(var), sizeof(var) }

#endif	/* APP_REGISTERS_H */
