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

static int dropped_lines = 0;


void DebugInit(void)
{
    current_buffer = buffer_1;
    current_buffer_len = 0;
}

void DebugLog(const char * file, int line, const char * format, ...)
{
    const uint8_t buffer_size = DEBUG_BUFFER_SIZE;
    
    uint8_t new_len = current_buffer_len;
    
    new_len +=  snprintf((char *)current_buffer + new_len, buffer_size - new_len, "%s:%d ", file, line);
    if (new_len >= buffer_size) {
        dropped_lines++;
        return;
    }
    
    va_list args;
    va_start(args, format);
    new_len += vsnprintf((char *)current_buffer + new_len, buffer_size - new_len, format, args);
    va_end(args);
    if (new_len >= buffer_size) { // Format errors that cause negative return values are probably caught here too.
        dropped_lines++;
        return;
    }
    
    new_len +=  snprintf((char *)current_buffer + new_len, buffer_size - new_len, "\r\n");
    if (new_len >= buffer_size) {
        dropped_lines++;
        return;
    }
    
    // There was room for the whole line, so take it into account.
    current_buffer_len = new_len;
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
        
        if (dropped_lines) { // The USB layer couln't send the data fast enough. If this is a problem, calling CDCService() more often could help.
            current_buffer[0] = '\n'; // Send an empty line to show that the overflow happened.
            current_buffer_len++;
            dropped_lines = 0;
        }
    }
}
