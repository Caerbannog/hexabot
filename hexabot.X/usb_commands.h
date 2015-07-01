
#ifndef USB_COMMANDS_H
#define	USB_COMMANDS_H

#include "usb_config.h"


void CommandInitEP(void);
uint8_t PollCommand(char * buffer, uint8_t len);
bool USBCommandEventHandler(USB_EVENT event, void *pdata, uint16_t size);
void SendCommand(char * buffer, uint8_t len);


#endif	/* USB_COMMANDS_H */

