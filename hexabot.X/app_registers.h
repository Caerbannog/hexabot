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
    const uint8_t * addr;
    const uint8_t size;
} register_t;

typedef struct {
    const uint8_t * args;
    const uint8_t size;
    const void (* procedure)(void); // Function pointer.
} remote_prodecure_t;

extern const register_t registers[];
extern const remote_prodecure_t procedures[];

#define REG_VAR(var)     { (uint8_t *)&(var), sizeof(var) }
#define PROC(var, proc)  { (uint8_t *)&(var), sizeof(var), proc }

#endif	/* APP_REGISTERS_H */