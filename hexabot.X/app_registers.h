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

#ifndef APP_REGISTERS_H
#define	APP_REGISTERS_H

#include <stdint.h>
#include <stdbool.h>

#define REG_VAR(var)     { (uint8_t *)&(var), sizeof(var) }


typedef struct {
    uint8_t * const addr;
    const uint8_t size;
} register_t;

extern uint8_t servo1;

extern float KP;
extern float KI;
extern float KD;

extern uint8_t motor_r_pwm;
extern uint8_t motor_r_dir;
extern uint8_t motor_l_pwm;
extern uint8_t motor_l_dir;

extern float r_target_speed;
extern float l_target_speed;
extern float r_control_speed;
extern float l_control_speed;
extern float control_loop_interval;

extern unsigned long qei3_position;
extern unsigned long qei4_position;
extern volatile unsigned int qei3_errors;
extern volatile unsigned int qei4_errors;

extern float odometry_x;
extern float odometry_y;
extern float odometry_theta;
extern float odometry_d;
extern int odometry_resolution;
extern float odometry_r_arc;
extern float odometry_l_arc;
extern float odometry_rotation_imbalance;
extern float half_wheel_distance;

extern float motor_ticks_per_meter;
extern bool stop_motors;


void register_command(uint8_t * buffer, uint8_t received);

#endif	/* APP_REGISTERS_H */
