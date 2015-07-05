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

#include "usb_debug.h"
#include "usb/usb_device_cdc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>


// Ping-pong buffers to handle short bursts. Ping-pong is not enabled at the lower USB layer.
static volatile uint8_t buffer_1[DEBUG_BUFFER_SIZE];
static volatile uint8_t buffer_2[DEBUG_BUFFER_SIZE];

static volatile uint8_t * current_buffer;
static uint8_t current_buffer_len;


void DebugInit(void)
{
    current_buffer = buffer_1;
    current_buffer_len = 0;
}


void DebugLog(const char * file, int line, const char * format, ...)
{
    const uint8_t buffer_size = DEBUG_BUFFER_SIZE;
    
    current_buffer_len +=  snprintf((char *)current_buffer + current_buffer_len, buffer_size - current_buffer_len, "%s:%d ", file, line);
    if (current_buffer_len >= buffer_size) {
        current_buffer_len = buffer_size;
        return;
    }
    
    va_list args;
    va_start(args, format);
    current_buffer_len += vsnprintf((char *)current_buffer + current_buffer_len, buffer_size - current_buffer_len, format, args); // FIXME check negative return
    va_end(args);
    if (current_buffer_len >= buffer_size) {
        current_buffer_len = buffer_size;
        return;
    }
    
    current_buffer_len +=  snprintf((char *)current_buffer + current_buffer_len, buffer_size - current_buffer_len, "\r\n");
    if (current_buffer_len >= buffer_size) {
        current_buffer_len = buffer_size;
        return;
    }
}


void DebugService(void)
{
    if(USBUSARTIsTxTrfReady() && current_buffer_len) {
        putUSBUSART((uint8_t *)current_buffer, current_buffer_len);
        
        if (current_buffer == buffer_1) {
            current_buffer = buffer_2;
        }
        else {
            current_buffer = buffer_1;
        }
        current_buffer_len = 0;
    }
}
