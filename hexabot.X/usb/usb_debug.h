
#ifndef APP_DEBUG_H
#define	APP_DEBUG_H

#include <usb/usb_config.h>


#define WARN(str_then_args...)        LOG("WARNING " str_then_args)
#define ERROR(str_then_args...)       LOG("ERROR " str_then_args)
#define LOG(args...)                  DebugLog(__FILE__, __LINE__, args)

#define DEBUG_BUFFER_SIZE   (CDC_DATA_IN_EP_SIZE - 1 /*this might avoid zero length packets*/ + 1 /*vsnprintf will always leave room for a null*/)


void DebugInit(void);
void DebugLog(const char * file, int line, const char * format, ...);
void DebugService(void);

#endif	/* APP_DEBUG_H */
