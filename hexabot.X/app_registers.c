
#include <p33EP512MU810.h>

#include "app_registers.h"

uint16_t vbat = 0;
float Kp = 0;
float Ki = 0;
float Kd = 0;
uint8_t porta;
uint8_t portb;
uint8_t lata;
uint8_t latb;

uint8_t SPI_args[16] = {};
uint8_t I2C_args[16] = {};

const register_t registers[] = {
    // 0
    REG_VAR(PORTA),
    REG_VAR(PORTB),
    REG_VAR(PORTC),
    REG_VAR(PORTD),
    REG_VAR(PORTE),
    REG_VAR(PORTF),
    REG_VAR(PORTG),
    // 7
    REG_VAR(LATA),
    REG_VAR(LATB),
    REG_VAR(LATC),
    REG_VAR(LATD),
    REG_VAR(LATE),
    REG_VAR(LATF),
    REG_VAR(LATG),
    // 14
    REG_VAR(vbat),
    REG_VAR(Kp),
    REG_VAR(Ki),
    REG_VAR(Kd),

    // TODO: filler to start commands at a constant register address
    // 17
    REG_VAR(SPI_args),
    REG_VAR(I2C_args)
};
