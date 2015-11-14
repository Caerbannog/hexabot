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

#ifndef APP_PROCEDURES_H
#define	APP_PROCEDURES_H

#include <stdint.h>

#define PROC(var, proc)    { (uint8_t *)&(var), sizeof(var), proc }


typedef struct {
    uint8_t * const args;
    const uint8_t arg_size;
    void (* const procedure)(int); // Function pointer.
} procedure_t;


void procedure_command(uint8_t * buffer, uint8_t received);

#endif	/* APP_PROCEDURES_H */

