
#ifndef USB_COMMANDS_H
#define	USB_COMMANDS_H

#include "usb_config.h"


void CommandInitEP(void);
uint8_t CommandOutPoll(uint8_t * buffer, uint8_t len);
bool CommandEventHandler(USB_EVENT event, void *pdata, uint16_t size);
void CommandInSend(uint8_t * buffer, uint8_t len);


#endif	/* USB_COMMANDS_H */
