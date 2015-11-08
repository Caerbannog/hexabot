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

typedef struct {
    uint8_t * const addr;
    const uint8_t size;
} register_t;

typedef struct {
    const uint8_t * args;
    const uint8_t size;
    const void (* procedure)(void); // Function pointer.
} remote_prodecure_t;

extern const register_t registers[];
extern const uint8_t register_count;
extern const remote_prodecure_t procedures[];

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
extern float control_loop_interval;


#define REG_VAR(var)     { (uint8_t *)&(var), sizeof(var) }
#define PROC(var, proc)  { (uint8_t *)&(var), sizeof(var), proc }


void register_command(uint8_t * buffer, uint8_t received);

#endif	/* APP_REGISTERS_H */
