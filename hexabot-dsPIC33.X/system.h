/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#ifndef SYSTEM_H
#define SYSTEM_H

#include <xc.h>
#include <stdbool.h>

#include "adc.h"
#include "buttons.h"
#include "leds.h"

#include "io_mapping.h"

#define MAIN_RETURN int

/*** System States **************************************************/
typedef enum
{
    SYSTEM_STATE_USB_START,
    SYSTEM_STATE_USB_SUSPEND,
    SYSTEM_STATE_USB_RESUME
} SYSTEM_STATE;

/*********************************************************************
* Function: void SYSTEM_Initialize( SYSTEM_STATE state )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  SYSTEM_STATE - the state to initialize the system into
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize( SYSTEM_STATE state );


/*********************************************************************
* Function: void SYSTEM_Tasks(void)
*
* Overview: Runs system level tasks that keep the system running
*
* PreCondition: System has been initalized with SYSTEM_Initialize()
*
* Input: None
*
* Output: None
*
********************************************************************/
//void SYSTEM_Tasks(void);
#define SYSTEM_Tasks()

#define Fcy                  60000000
#define MOTOR_PWM_PERIOD     ((uint16_t)(0.00005 * Fcy / 8 /*prescaler*/)) // FIXME: try prescale of 1, test if power changes with frequency.
#define TIMER23_TICK_S       (256 /*prescaler*/ / (float)Fcy)

#if 1 // Software QEI for external encoders.
  #define QEI3_PINS          ENC3_PINS
  #define QEI4_PINS          ENC1_PINS
  #define ENABLE_CN_QEI3     EnableCND3; EnableCND4; // ENC3
  #define ENABLE_CN_QEI4     EnableCND8; EnableCND9; // ENC1
#else // Software QEI for motor encoders.
  #define QEI3_PINS          ENC2_PINS
  #define QEI4_PINS          ENC4_PINS
  #define ENABLE_CN_QEI3     CNENDbits.CNIED10 = 1 /* Typo in the header for EnableCND10. */; EnableCND11; // ENC2 MOTOR_R
  #define ENABLE_CN_QEI4     EnableCNF0; EnableCNF1; // ENC4 MOTOR_L
#endif

#define BOUNDS(low, x, high) ({\
    __typeof__(low) __low = (low);\
    __typeof__(x) __x = (x); \
    __typeof__(high) __high = (high);\
    min(__low, max(__x, __high));\
    }) // bounds/saturate/clamp/clip

#endif //SYSTEM_H
