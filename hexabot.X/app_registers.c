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

#include "app_registers.h"
#include <usb/usb_commands.h>
#include <usb/usb_debug.h>
#include <string.h>
#include <xc.h>

#ifndef PORTA // 806 dsPIC
uint16_t PORTA = 0;
uint16_t LATA  = 0;
uint16_t TRISA = 0;
#endif

uint16_t vbat = 0;
float KP = 1.4; // 1/(m/s)
float KI = 1.0;
float KD = 0;

uint8_t echo_args[16] = {};
uint8_t SPI_args[16] = {};
uint8_t I2C_args[16] = {};

uint8_t servo1 = 127;

uint8_t motor_r_pwm = 127;
uint8_t motor_r_dir = 1;
uint8_t motor_l_pwm = 127;
uint8_t motor_l_dir = 1;

float r_target_speed = 0;
float l_target_speed = 0;
float r_control_speed = 0;
float l_control_speed = 0;
float control_loop_interval = .01;

volatile unsigned long qei3_position = 0;
volatile unsigned long qei4_position = 0;
volatile unsigned int qei3_errors = 0;
volatile unsigned int qei4_errors = 0;

float odometry_x = 0;
float odometry_y = 0;
float odometry_theta = 0;
float odometry_d = 0;
int odometry_resolution = 100;

float odometry_r_arc = 15.0 / 4096;
float odometry_l_arc = 15.0 / 4096;
float odometry_rotation_imbalance = 0;
float half_wheel_distance = 10;

const register_t registers[] = {
    // 0
    REG_VAR(PORTA), // Input.
    REG_VAR(PORTB),
    REG_VAR(PORTC),
    REG_VAR(PORTD),
    REG_VAR(PORTE),
    REG_VAR(PORTF),
    REG_VAR(PORTG),
    // 7
    REG_VAR(LATA), // Output.
    REG_VAR(LATB),
    REG_VAR(LATC),
    REG_VAR(LATD),
    REG_VAR(LATE),
    REG_VAR(LATF),
    REG_VAR(LATG),
    // 14
    REG_VAR(TRISA), // Pin direction.
    REG_VAR(TRISB),
    REG_VAR(TRISC),
    REG_VAR(TRISD),
    REG_VAR(TRISE),
    REG_VAR(TRISF),
    REG_VAR(TRISG),
    // 21
    REG_VAR(OC1RS), // MOTOR_R_PWM
    REG_VAR(OC2RS), // MOTOR_L_PWM
    REG_VAR(OC3RS), // SERVO_1
    REG_VAR(OC4RS), // SERVO_2
    // 25
    REG_VAR(ADC1BUF0), // SENSE_R
    REG_VAR(ADC1BUF1), // SENSE_L
    REG_VAR(ADC1BUF2), // AN3
    REG_VAR(ADC1BUF3), // VBAT
    // 29
    REG_VAR(POS1CNTL), // Low 16 bits of position counter for QEI1.
    REG_VAR(POS2CNTL), // Same for QEI2.
    // 31
    REG_VAR(KP),
    REG_VAR(KI),
    REG_VAR(KD),
    // 34
    REG_VAR(motor_r_pwm),
    REG_VAR(motor_r_dir),
    REG_VAR(motor_l_pwm),
    REG_VAR(motor_l_dir),
    // 38
    REG_VAR(r_target_speed),
    REG_VAR(l_target_speed),
    REG_VAR(r_control_speed),
    REG_VAR(l_control_speed),
    REG_VAR(control_loop_interval),
    // 43
    REG_VAR(qei3_position),
    REG_VAR(qei4_position),
    REG_VAR(qei3_errors),
    REG_VAR(qei4_errors),
    // 47
    REG_VAR(odometry_x),
    REG_VAR(odometry_y),
    REG_VAR(odometry_theta),
    REG_VAR(odometry_d),
    REG_VAR(odometry_resolution),
    // 52
    REG_VAR(odometry_r_arc),
    REG_VAR(odometry_l_arc),
    REG_VAR(odometry_rotation_imbalance),
    REG_VAR(half_wheel_distance),
};

const uint8_t register_count = sizeof(registers) / sizeof(*registers);

void register_command(uint8_t * buffer, uint8_t received)
{
    // FIXME: assert sizeof register
    uint8_t reg = buffer[0];
    if (reg >= register_count) {
        ERROR("reg=%d", reg);
    }
    else {
        uint8_t reg_size = registers[reg].size;
        if (received == 1) {
            memcpy(buffer, registers[reg].addr, reg_size);
            CommandInSend(buffer, reg_size);
            return;
        }
        /*
        else if (received - 1 > reg_size) {
            ERROR("reg=%d size smaller than received=%d", reg, received);
        }
        */
        else if (received - 1 != reg_size) {
            ERROR("reg=%d size different from received=%d", reg, received);
        }
        else {
            memcpy(registers[reg].addr, buffer + 1, received - 1);
        }
    }
    buffer[0] = 0xFF;
    CommandInSend(buffer, 1); // TODO ACK
}
