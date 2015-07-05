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
#include <stdio.h>
#include <stdarg.h>


static char buffer[64];

// TODO: bufferiser pour pouvoir envoyer des logs plus souvent !!!

void debug_log(const char * file, int line, const char * format, ...)
{
    const int buffer_size = sizeof(buffer);
    int offset = 0;
    
    offset += snprintf(buffer, buffer_size, "%s:%d ", file, line);
    if (offset > buffer_size) {
        offset = buffer_size;
    }
    
    va_list args;
    va_start(args, format);
    offset += vsnprintf(buffer + offset, buffer_size - offset, format, args); // FIXME check negative return
    va_end(args);
    if (offset > buffer_size) {
        offset = buffer_size;
    }
    
    offset += snprintf(buffer + offset, buffer_size - offset, "\r\n");
    if (offset > buffer_size) {
        offset = buffer_size;
    }
    
    if(USBUSARTIsTxTrfReady()) {
        putUSBUSART((uint8_t *)buffer, offset + 1 /*null char*/);
    }
}
