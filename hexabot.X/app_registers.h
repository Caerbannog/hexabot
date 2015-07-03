
#ifndef APP_REGISTERS_H
#define	APP_REGISTERS_H

#include <stdint.h>

typedef struct {
    const uint8_t * addr;
    const uint8_t size;
} register_t;

extern const register_t registers[];

#define REG_VAR(var)     { (uint8_t *)&(var), sizeof(var) }

#endif	/* APP_REGISTERS_H */
