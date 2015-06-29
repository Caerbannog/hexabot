
#include <system.h>
#include <usb/usb.h>
#include "usb_commands.h"


volatile unsigned char command_data_in[COMMAND_IN_EP_SIZE];
volatile unsigned char command_data_out[COMMAND_OUT_EP_SIZE];

uint8_t command_out_len;            // total rx length
uint8_t command_in_len;            // total tx length

USB_HANDLE CommandDataOutHandle;
USB_HANDLE CommandDataInHandle;


void CommandInitEP(void)
{
    command_out_len = 0;
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

    //cdc_trf_state = CDC_TX_READY;
}


bool USBCommandEventHandler(USB_EVENT event, void *pdata, uint16_t size)
{
// TODO: what purpose?
#if 0
    switch( (uint16_t)event )
    {
        case EVENT_TRANSFER_TERMINATED:
            if(pdata == CDCDataOutHandle)
            {
                CDCDataOutHandle = USBRxOnePacket(CDC_DATA_EP,(uint8_t*)&cdc_data_rx,sizeof(cdc_data_rx));
            }
            if(pdata == CDCDataInHandle)
            {
                //flush all of the data in the CDC buffer
                cdc_trf_state = CDC_TX_READY;
                cdc_tx_len = 0;
            }
            break;
        default:
            return false;
    }
#endif
    return true;
}


void CommandInService(void)
{
    uint8_t byte_to_send;
    uint8_t i;

    USBMaskInterrupts();

    //CDCNotificationHandler();

    if(USBHandleBusy(CommandDataInHandle))
    {
        USBUnmaskInterrupts();
        return;
    }

    // TODO state machine here
    static long time = 0;
    if (time < 0) {
    }
    else if (time > 10000000) {
        time = -1;
        /*
         * First, have to figure out how many byte of data to send.
         */
        if(command_in_len > sizeof(command_data_in))
            byte_to_send = sizeof(command_data_in);
        else
            byte_to_send = command_in_len;

        /*
         * Subtract the number of bytes just about to be sent from the total.
         */
        command_in_len = command_in_len - byte_to_send;
        
        
        char * str = "helloworld\n";
        i = byte_to_send;
        while(i)
        {
            command_data_in[i] = str[i];
            i--;
        }

        /*
         * Lastly, determine if a zero length packet state is necessary.
         * See explanation in USB Specification 2.0: Section 5.8.3
         */
        if(command_in_len == 0)
        {
            if(byte_to_send == CDC_DATA_IN_EP_SIZE)
                ;
            else
                ;
        }
        CommandDataInHandle = USBTxOnePacket(COMMAND_EP, (uint8_t*)&command_data_in, byte_to_send);
    }
    
    
    USBUnmaskInterrupts();
}
