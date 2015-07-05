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

#include "usb_commands.h"
#include <usb/usb_debug.h>
#include <usb/usb.h>
#include <system.h>


static volatile uint8_t buffer_in[COMMAND_IN_EP_SIZE];
static volatile uint8_t buffer_out[COMMAND_OUT_EP_SIZE];

static USB_HANDLE handle_out;
static USB_HANDLE handle_in;


/*
 * This function is called after USB configuration and should (re-)initialize the module.
 */
void CommandInitEP(void)
{
    USBEnableEndpoint(COMMAND_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    handle_out = USBRxOnePacket(COMMAND_EP,(uint8_t*)&buffer_out,sizeof(buffer_out));
    handle_in = NULL;
}


uint8_t CommandOutPoll(uint8_t * buffer, uint8_t max_len)
{
    if(USBHandleBusy(handle_out)) {
        return 0;
    }
    
    int received = USBHandleGetLength(handle_out);
    uint8_t len = buffer_out[0];
    if(received > max_len + 1 /*+2 if we didn't copy the null byte*/) {
        ERROR("received=%d more than max_len=%d", received, max_len);
        received = 0;
    }
    else if (len + 2 != received) {
        ERROR("len=%d but received=%d", len, received);
        received = 0;
    }
    else if (buffer_out[received] != 0) {
        ERROR("no end of packet for len=%d", len);
        received = 0;
    }
    else {
        // Copy data from dual-ram buffer to user's buffer
        uint8_t i;
        for(i = 0; i < len + 1; i++) { // Include the terminating null byte.
            buffer[i] = buffer_out[i + 1];
        }
    }

    // Prepare dual-ram buffer for next OUT transaction
    handle_out = USBRxOnePacket(COMMAND_EP, (uint8_t*)&buffer_out, sizeof(buffer_out));

    return len;
}


bool CommandEventHandler(USB_EVENT event, void *pdata, uint16_t size)
{
    // Probably useless.
    return true;
}


void CommandInSend(uint8_t * buffer, uint8_t len)
{
    if (len == 0) {
        ERROR("empty command");
        return;
    }

    if(len + 2 > sizeof(buffer_in)) {
        ERROR("command too long");
        return;
    }
    
    USBMaskInterrupts();

    if(USBHandleBusy(handle_in))
    {
        USBUnmaskInterrupts();
        ERROR("command dropped");
        return;
    }

    uint8_t i;
    buffer_in[0] = len; // Header
    for (i = 0; i < len; i++) {
        buffer_in[i + 1] = buffer[i];
    }
    buffer_in[len + 1] = '\0';

    // Note: there might be a need for a zero length packet state for full buffers.
    // See explanation in USB Specification 2.0: Section 5.8.3
    
    handle_in = USBTxOnePacket(COMMAND_EP, (uint8_t *)buffer_in, len + 2);
    
    USBUnmaskInterrupts();
}


void CommandService(void)
{
    
}
