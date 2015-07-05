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

#include "usb_metrics.h"
#include "app_debug.h"
#include <system.h>
#include <usb/usb.h>

static volatile uint8_t metrics_in[METRICS_IN_EP_SIZE];

static USB_HANDLE MetricsInHandle;


void MetricsInitEP(void)
{
    USBEnableEndpoint(METRICS_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    MetricsInHandle = NULL;
}


void MetricsService(void)
{
    
}


void MetricsInSend(uint8_t * buffer, uint8_t len)
{
    MetricsService();
}
