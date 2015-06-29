
#include <system.h>
#include <usb/usb.h>
#include "usb_commands.h"


volatile unsigned char command_data_in[COMMAND_IN_EP_SIZE];
volatile unsigned char command_data_out[COMMAND_OUT_EP_SIZE];

uint8_t command_out_len;            // total rx length

//POINTER pCDCSrc;            // Dedicated source pointer
//POINTER pCDCDst;            // Dedicated destination pointer
uint8_t command_in_len;            // total tx length
//uint8_t cdc_mem_type;          // _ROM, _RAM

USB_HANDLE CommandDataOutHandle;
USB_HANDLE CommandDataInHandle;


void CommandInitEP(void)
{
#if 0
    //Abstract line coding information
    line_coding.dwDTERate   = 19200;      // baud rate
    line_coding.bCharFormat = 0x00;             // 1 stop bit
    line_coding.bParityType = 0x00;             // None
    line_coding.bDataBits = 0x08;               // 5,6,7,8, or 16

    cdc_rx_len = 0;

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
    USBEnableEndpoint(CDC_COMM_EP,USB_IN_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    USBEnableEndpoint(CDC_DATA_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    CDCDataOutHandle = USBRxOnePacket(CDC_DATA_EP,(uint8_t*)&cdc_data_rx,sizeof(cdc_data_rx));
    CDCDataInHandle = NULL;

    #if defined(USB_CDC_SUPPORT_DSR_REPORTING)
        CDCNotificationInHandle = NULL;
        mInitDTSPin();  //Configure DTS as a digital input
        SerialStateBitmap.byte = 0x00;
        OldSerialStateBitmap.byte = !SerialStateBitmap.byte;    //To force firmware to send an initial serial state packet to the host.
        //Prepare a SerialState notification element packet (contains info like DSR state)
        SerialStatePacket.bmRequestType = 0xA1; //Always 0xA1 for this type of packet.
        SerialStatePacket.bNotification = SERIAL_STATE;
        SerialStatePacket.wValue = 0x0000;  //Always 0x0000 for this type of packet
        SerialStatePacket.wIndex = CDC_COMM_INTF_ID;  //Interface number
        SerialStatePacket.SerialState.byte = 0x00;
        SerialStatePacket.Reserved = 0x00;
        SerialStatePacket.wLength = 0x02;   //Always 2 bytes for this type of packet
        CDCNotificationHandler();
    #endif

    #if defined(USB_CDC_SUPPORT_DTR_SIGNALING)
        mInitDTRPin();
    #endif

    #if defined(USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL)
        mInitRTSPin();
        mInitCTSPin();
    #endif

    cdc_trf_state = CDC_TX_READY;
#endif
}


bool USBCommandEventHandler(USB_EVENT event, void *pdata, uint16_t size)
{
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
#if 0
    uint8_t byte_to_send;
    uint8_t i;

    USBMaskInterrupts();

    CDCNotificationHandler();

    if(USBHandleBusy(CDCDataInHandle))
    {
        USBUnmaskInterrupts();
        return;
    }

    /*
     * Completing stage is necessary while [ mCDCUSartTxIsBusy()==1 ].
     * By having this stage, user can always check cdc_trf_state,
     * and not having to call mCDCUsartTxIsBusy() directly.
     */
    if(cdc_trf_state == CDC_TX_COMPLETING)
        cdc_trf_state = CDC_TX_READY;

    /*
     * If CDC_TX_READY state, nothing to do, just return.
     */
    if(cdc_trf_state == CDC_TX_READY)
    {
        USBUnmaskInterrupts();
        return;
    }

    /*
     * If CDC_TX_BUSY_ZLP state, send zero length packet
     */
    if(cdc_trf_state == CDC_TX_BUSY_ZLP)
    {
        CDCDataInHandle = USBTxOnePacket(CDC_DATA_EP,NULL,0);
        //CDC_DATA_BD_IN.CNT = 0;
        cdc_trf_state = CDC_TX_COMPLETING;
    }
    else if(cdc_trf_state == CDC_TX_BUSY)
    {
        /*
         * First, have to figure out how many byte of data to send.
         */
        if(cdc_tx_len > sizeof(cdc_data_tx))
            byte_to_send = sizeof(cdc_data_tx);
        else
            byte_to_send = cdc_tx_len;

        /*
         * Subtract the number of bytes just about to be sent from the total.
         */
        cdc_tx_len = cdc_tx_len - byte_to_send;

        pCDCDst.bRam = (uint8_t*)&cdc_data_tx; // Set destination pointer

        i = byte_to_send;
        if(cdc_mem_type == USB_EP0_ROM)            // Determine type of memory source
        {
            while(i)
            {
                *pCDCDst.bRam = *pCDCSrc.bRom;
                pCDCDst.bRam++;
                pCDCSrc.bRom++;
                i--;
            }//end while(byte_to_send)
        }
        else
        {
            while(i)
            {
                *pCDCDst.bRam = *pCDCSrc.bRam;
                pCDCDst.bRam++;
                pCDCSrc.bRam++;
                i--;
            }
        }

        /*
         * Lastly, determine if a zero length packet state is necessary.
         * See explanation in USB Specification 2.0: Section 5.8.3
         */
        if(cdc_tx_len == 0)
        {
            if(byte_to_send == CDC_DATA_IN_EP_SIZE)
                cdc_trf_state = CDC_TX_BUSY_ZLP;
            else
                cdc_trf_state = CDC_TX_COMPLETING;
        }//end if(cdc_tx_len...)
        CDCDataInHandle = USBTxOnePacket(CDC_DATA_EP,(uint8_t*)&cdc_data_tx,byte_to_send);

    }//end if(cdc_tx_sate == CDC_TX_BUSY)

    USBUnmaskInterrupts();
#endif
}
