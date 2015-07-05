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

/** INCLUDES *******************************************************/
#include "app.h"
#include "app_led_usb_status.h"
#include <usb/usb_commands.h>
#include <usb/usb_metrics.h>
#include <usb/usb_isochronous.h>
#include <usb/usb_debug.h>
#include <usb/usb_config.h>
#include <usb/usb.h>
#include <system.h>

#include <stdint.h>
#include <string.h>
#include <stddef.h>




/** VARIABLES ******************************************************/

static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];

/*********************************************************************
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_Initialize()
{
    DebugInit();
    CDCInitEP();
    CommandInitEP();
    MetricsInitEP();
    IsochronousInitEP();
}

/*********************************************************************
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_Initialize() and APP_Start() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_Tasks()
{
    if (BUTTON_IsPressed(BUTTON_S1)) {
        //LOG("PRESSED S1");
        static int i = 0;
        i++;
        if (i % 100 == 0) {
            MetricsAppend(METRIC_DEBUG_1, i);
        }
    }
    
    if (BUTTON_IsPressed(BUTTON_S2)) {
        char * str = "COMMAND_IN";
        CommandInSend((uint8_t *)str, strlen(str));
    }
    
    uint8_t buffer[CDC_DATA_OUT_EP_SIZE];
    uint8_t received = CommandOutPoll(buffer, sizeof(buffer));
    if (received > 0) {
        //LOG("received=%d", received);
        //putUSBUSART(buffer, received);
        CommandInSend(buffer, received);
    }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    if( USBUSARTIsTxTrfReady() == true)
    {
        uint8_t i;
        uint8_t numBytesRead;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));

        /* For every byte that was read... */
        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer[i])
            {
                /* If we receive new line or line feed commands, just echo
                 * them direct.
                 */
                case 0x0A:
                case 0x0D:
                    writeBuffer[i] = readBuffer[i];
                    break;

                /* If we receive something else, then echo it plus one
                 * so that if we receive 'a', we echo 'b' so that the
                 * user knows that it isn't the echo enabled on their
                 * terminal program.
                 */
                default:
                    writeBuffer[i] = readBuffer[i] + 1;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            /* After processing all of the received data, we need to send out
             * the "echo" data now.
             */
            putUSBUSART(writeBuffer,numBytesRead);
        }
    }

    DebugService();
    CDCService();
    CommandService();
    MetricsService();
}
