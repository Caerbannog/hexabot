
#ifndef USB_COMMANDS_H
#define	USB_COMMANDS_H

#include "usb_config.h"

void CommandInitEP(void);
bool USBCommandEventHandler(USB_EVENT event, void *pdata, uint16_t size);
void CommandInService(void);


extern uint8_t command_out_len;
extern USB_HANDLE lastTransmission;

//extern uint8_t cdc_trf_state;
//extern POINTER pCDCSrc;
extern uint8_t command_in_len;
//extern uint8_t cdc_mem_type;

//extern const uint8_t configDescriptor1[];


#endif	/* USB_COMMANDS_H */

