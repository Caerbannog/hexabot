
#include "app_registers.h"

uint8_t test1[5] = {};
uint8_t test2[5] = {1, 2, 3, 4, 5};

register_t registers[] = {
    REG_VAR(test1),
    REG_VAR(test2)
};
