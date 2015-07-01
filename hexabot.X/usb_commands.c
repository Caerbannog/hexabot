
#include <system.h>
#include <usb/usb.h>
#include "usb_commands.h"
#include "app_debug.h"

#include "usb/usb_device_cdc.h"

unsigned char command_in_buffer[COMMAND_IN_EP_SIZE];
uint8_t command_in_buffer_len;

volatile unsigned char command_data_in[COMMAND_IN_EP_SIZE];
volatile unsigned char command_data_out[COMMAND_OUT_EP_SIZE];

USB_HANDLE CommandDataOutHandle;
USB_HANDLE CommandDataInHandle;


void CommandInitEP(void)
{
    command_in_buffer_len = 0;
    
    /*
     * Do not have to init Cnt of IN pipes here.
     * Reason:  Number of BYTEs to send to the host
     *          varies from one transaction to
     *          another. Cnt should equal the exact
     *          number of BYTEs to transmit for
     *          a given IN transaction.
     *          This number of BYTEs will only
     *          be known right before the data is
     *          sent.
     */
    
    USBEnableEndpoint(COMMAND_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP /*TODO*/);

    CommandDataOutHandle = USBRxOnePacket(COMMAND_EP,(uint8_t*)&command_data_out,sizeof(command_data_out));
    CommandDataInHandle = NULL;
}


uint8_t PollCommand(char * buffer, uint8_t len)
{
    int written = 0;
    
    if(!USBHandleBusy(CommandDataOutHandle))
    {
# if 0
        // Debug output
        putUSBUSART(command_data_out, USBHandleGetLength(CommandDataOutHandle));
#else
        /*
         * Adjust the expected number of BYTEs to equal
         * the actual number of BYTEs received.
         */
        if(len > USBHandleGetLength(CommandDataOutHandle))
            len = USBHandleGetLength(CommandDataOutHandle);
        
        /*
         * Copy data from dual-ram buffer to user's buffer
         */
        for( ; written < len; written++)
            buffer[written] = command_data_out[written];
#endif
        
        /*
         * Prepare dual-ram buffer for next OUT transaction
         */

        CommandDataOutHandle = USBRxOnePacket(COMMAND_EP, (uint8_t*)&command_data_out, sizeof(command_data_out));
    }
    
    return written;
}


bool USBCommandEventHandler(USB_EVENT event, void *pdata, uint16_t size)
{
    // Probably useless.
    return true;
}


void SendCommand(char * buffer, uint8_t len)
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
        WARN("command empty");
        return;
    }
    
    LED_Toggle(LED_D2); // TODO: consistent activity LED

    if(len > sizeof(command_data_in)) {
        len = sizeof(command_data_in);
    }

    int i;
    for (i = 0; i < len; i++) {
        command_data_in[i] = buffer[i];
    }

    // Note: there might be a need for a zero length packet state for full buffers.
    // See explanation in USB Specification 2.0: Section 5.8.3
    
    CommandDataInHandle = USBTxOnePacket(COMMAND_EP, (uint8_t*)&command_data_in, len);
    
    USBUnmaskInterrupts();
}
