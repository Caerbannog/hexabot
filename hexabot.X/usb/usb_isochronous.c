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

#include "usb_isochronous.h"
#include <usb/usb_debug.h>
#include <usb/usb.h>
#include <system.h>


iso_frame_t isochronous_frame;

static USB_HANDLE handle_in;


/*
 * This function is called after USB configuration and should (re-)initialize the module.
 */
void IsochronousInitEP(void)
{
    USBEnableEndpoint(ISOCHRONOUS_EP, USB_IN_ENABLED | USB_DISALLOW_SETUP);

    handle_in = NULL;
    memset(&isochronous_frame, 0, sizeof(isochronous_frame)); // Clear data to be sure.
}


void IsochronousSend(void)
{
    if(USBHandleBusy(handle_in)) {
        return;
    }
    
    // TODO
    
    handle_in = USBTxOnePacket(ISOCHRONOUS_EP, (uint8_t *)&isochronous_frame, sizeof(isochronous_frame));
}
