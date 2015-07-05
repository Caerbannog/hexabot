/*
  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <p33EP512MU810.h>

#include "app_registers.h"

uint16_t vbat = 0;
float Kp = 0;
float Ki = 0;
float Kd = 0;

uint8_t echo_args[16] = {};
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
    // 18
};

const remote_prodecure_t procedures[] = {
    { /*registers are not handled by a dedicated function*/ },
    // 1
    PROC(echo_args, 0),
    PROC(SPI_args, 0),
    PROC(I2C_args, 0)
    // 4
};
