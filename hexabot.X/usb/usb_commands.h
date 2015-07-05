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

#ifndef USB_COMMANDS_H
#define	USB_COMMANDS_H

#include "usb_config.h"
#include <usb/usb_common.h>
#include <stdint.h>


void CommandInitEP(void);
uint8_t CommandOutPoll(uint8_t * buffer, uint8_t len);
bool CommandEventHandler(USB_EVENT event, void *pdata, uint16_t size);
void CommandInSend(uint8_t * buffer, uint8_t len);
void CommandService(void);


#endif	/* USB_COMMANDS_H */
