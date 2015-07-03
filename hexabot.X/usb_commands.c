
#include <system.h>
#include <usb/usb.h>
#include "usb_commands.h"
#include "app_debug.h"

#include "usb/usb_device_cdc.h"

volatile uint8_t command_data_in[COMMAND_IN_EP_SIZE];
volatile uint8_t command_data_out[COMMAND_OUT_EP_SIZE];

USB_HANDLE CommandDataOutHandle;
USB_HANDLE CommandDataInHandle;


void CommandInitEP(void)
{
    USBEnableEndpoint(COMMAND_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP /*TODO*/);

    CommandDataOutHandle = USBRxOnePacket(COMMAND_EP,(uint8_t*)&command_data_out,sizeof(command_data_out));
    CommandDataInHandle = NULL;
}


uint8_t CommandOutPoll(uint8_t * buffer, uint8_t max_len)
{
    if(USBHandleBusy(CommandDataOutHandle)) {
        return 0;
    }
    
    int received = USBHandleGetLength(CommandDataOutHandle);
    uint8_t len = command_data_out[0];
    if(received > max_len + 1 /*+2 if we didn't copy the null byte*/) {
        ERROR("received=%d more than max_len=%d", received, max_len);
        received = 0;
    }
    else if (len + 2 != received) {
        ERROR("len=%d but received=%d", len, received);
        received = 0;
    }
    else if (command_data_out[received] != 0) {
        ERROR("no end of packet for len=%d", len);
        received = 0;
    }
    else {
        // Copy data from dual-ram buffer to user's buffer
        uint8_t i;
        for(i = 0; i < len + 1; i++) { // Include the terminating null byte.
            buffer[i] = command_data_out[i + 1];
        }
    }

    // Prepare dual-ram buffer for next OUT transaction
    CommandDataOutHandle = USBRxOnePacket(COMMAND_EP, (uint8_t*)&command_data_out, sizeof(command_data_out));

    return len;
}


bool CommandEventHandler(USB_EVENT event, void *pdata, uint16_t size)
{
    // Probably useless.
    return true;
}


void CommandInSend(uint8_t * buffer, uint8_t len)
{
    USBMaskInterrupts();

    if(USBHandleBusy(CommandDataInHandle))
    {
        USBUnmaskInterrupts();
        ERROR("command dropped");
        return;
    }

    if (len == 0) {
        USBUnmaskInterrupts();
        ERROR("empty command");
        return;
    }
    
    LED_Toggle(LED_D2); // TODO: consistent activity LED

    if(len + 2 > sizeof(command_data_in)) {
        USBUnmaskInterrupts();
        ERROR("command too long");
        return;
    }

    uint8_t i;
    command_data_in[0] = len; // Header
    for (i = 0; i < len; i++) {
        command_data_in[i + 1] = buffer[i];
    }
    command_data_in[len + 1] = '\0';

    // Note: there might be a need for a zero length packet state for full buffers.
    // See explanation in USB Specification 2.0: Section 5.8.3
    
    CommandDataInHandle = USBTxOnePacket(COMMAND_EP, (uint8_t*)&command_data_in, len + 2);
    
    USBUnmaskInterrupts();
}
