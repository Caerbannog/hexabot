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

#include "app_procedures.h"
#include <usb/usb_commands.h>
#include <usb/usb_debug.h>
#include <string.h>
#include <stddef.h>
#include <xc.h>

#define proc_return(buffer, len)  CommandInSend(buffer, len) // Has to be called by every procedure. FIXME: test that delayed returns work.
#define proc_success()            { uint8_t res = COMMAND_SUCCESS; proc_return(&res, 1); }
#define proc_error()              { uint8_t res = COMMAND_ERROR;   proc_return(&res, 1); }

uint8_t echo_args[16] = {};
uint8_t SPI_args[16] = {};
uint8_t I2C_args[16] = {};


void echo_proc(int n)
{
    proc_return(echo_args, n);
}

void reset_proc(int n)
{
    __asm__ volatile ("reset");
}

void spi_proc(int n)
{
    // FIXME
    proc_success();
}

void i2c_proc(int n)
{
    // FIXME
    proc_success();
}

const procedure_t procedures[] = {
    // 0
    PROC(echo_args, echo_proc), // Returns the same arguments.
    { NULL, 0, reset_proc }, // Reset the microcontroller.
    // 2
    PROC(SPI_args, spi_proc), // 
    PROC(I2C_args, i2c_proc), // 
    // 4
};

const uint8_t procedure_count = sizeof(procedures) / sizeof(*procedures);


void procedure_command(uint8_t * buffer, uint8_t received)
{
    if (received == 0) {
        ERROR("no procedure");
        proc_error();
    }
    else {
        uint8_t proc = buffer[0];
        if (proc >= procedure_count) {
            ERROR("proc=%d", proc);
            proc_error();
        }
        else {
            uint8_t arg_size = procedures[proc].arg_size;
            if (received - 1 > arg_size) {
                ERROR("proc=%d arg size=%d < %d", proc, arg_size, received - 1);
                proc_error();
            }
            else {
                memcpy(procedures[proc].args, buffer + 1, received - 1);
                procedures[proc].procedure(received - 1);
            }
        }
    }
}
