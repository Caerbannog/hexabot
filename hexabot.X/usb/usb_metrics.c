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


// Ping-pong buffers to handle short bursts. Ping-pong is not enabled at the lower USB layer.
static volatile uint8_t buffer_1[METRICS_IN_EP_SIZE];
static volatile uint8_t buffer_2[METRICS_IN_EP_SIZE];

static volatile uint8_t * current_buffer;
static uint8_t current_buffer_len;

static USB_HANDLE handle_in;


/*
 * This function is called after USB configuration and should (re-)initialize the module.
 */
void MetricsInitEP(void)
{
    USBEnableEndpoint(METRICS_EP, USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    current_buffer = buffer_1;
    current_buffer_len = 0;
    handle_in = NULL;
}


void MetricsAppend(metric_id_t id, float value)
{
    metric_t metric = { id, value };
    // TODO time
    MetricsAppendRaw((uint8_t *)&metric, sizeof(metric));
}


/*
 * Append a value to the buffer so that the service can send it as soon as possible.
 */
void MetricsAppendRaw(uint8_t * buffer, uint8_t len)
{
    if (current_buffer_len + len > METRICS_IN_EP_SIZE) { // Buffer full.
        ERROR("metrics dropped %d bytes", len);
        return;
    }
    
    memcpy((uint8_t *)current_buffer + current_buffer_len, buffer, len);
    current_buffer_len += len;
}


/*
 * This function should be called periodically to push buffered data to the host.
 */
void MetricsService(void)
{
    if (current_buffer_len == 0) {
        return; // No pending data.
    }
    
    USBMaskInterrupts();

    if(USBHandleBusy(handle_in)) {
        // The previous transaction is not complete.
        USBUnmaskInterrupts();
    }
    else {
        // Note: there might be a need for a zero length packet state for full buffers.
        // See explanation in USB Specification 2.0: Section 5.8.3
        
        handle_in = USBTxOnePacket(METRICS_EP, (uint8_t *)current_buffer, current_buffer_len);
    
        USBUnmaskInterrupts();
        
        // Swap the buffers.
        if (current_buffer == buffer_1) {
            current_buffer = buffer_2;
        }
        else {
            current_buffer = buffer_1;
        }
        current_buffer_len = 0;
    }
}
