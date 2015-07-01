
#include "app_debug.h"
#include "usb/usb_device_cdc.h"
#include <stdio.h>
#include <stdarg.h>


static char buffer[64];

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
