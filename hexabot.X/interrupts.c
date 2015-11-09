/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"
#include "app_registers.h"
#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */
#include <xc.h>

/******************************************************************************/
/* Interrupt Vector Options                                                   */
/******************************************************************************/
/*                                                                            */
/* Refer to the C30 (MPLAB C Compiler for PIC24F MCUs and dsPIC33F DSCs) User */
/* Guide for an up to date list of the available interrupt options.           */
/* Alternately these names can be pulled from the device linker scripts.      */
/*                                                                            */
/* dsPIC33E Primary Interrupt Vector Names:                                   */
/*                                                                            */
/* _INT0Interrupt     _IC4Interrupt      _U4TXInterrupt                       */
/* _IC1Interrupt      _IC5Interrupt      _SPI3ErrInterrupt                    */
/* _OC1Interrupt      _IC6Interrupt      _SPI3Interrupt                       */
/* _T1Interrupt       _OC5Interrupt      _OC9Interrupt                        */
/* _DMA0Interrupt     _OC6Interrupt      _IC9Interrupt                        */
/* _IC2Interrupt      _OC7Interrupt      _PWM1Interrupt                       */
/* _OC2Interrupt      _OC8Interrupt      _PWM2Interrupt                       */
/* _T2Interrupt       _PMPInterrupt      _PWM3Interrupt                       */
/* _T3Interrupt       _DMA4Interrupt     _PWM4Interrupt                       */
/* _SPI1ErrInterrupt  _T6Interrupt       _PWM5Interrupt                       */
/* _SPI1Interrupt     _T7Interrupt       _PWM6Interrupt                       */
/* _U1RXInterrupt     _SI2C2Interrupt    _PWM7Interrupt                       */
/* _U1TXInterrupt     _MI2C2Interrupt    _DMA8Interrupt                       */
/* _AD1Interrupt      _T8Interrupt       _DMA9Interrupt                       */
/* _DMA1Interrupt     _T9Interrupt       _DMA10Interrupt                      */
/* _NVMInterrupt      _INT3Interrupt     _DMA11Interrupt                      */
/* _SI2C1Interrupt    _INT4Interrupt     _SPI4ErrInterrupt                    */
/* _MI2C1Interrupt    _C2RxRdyInterrupt  _SPI4Interrupt                       */
/* _CM1Interrupt      _C2Interrupt       _OC10Interrupt                       */
/* _CNInterrupt       _QEI1Interrupt     _IC10Interrupt                       */
/* _INT1Interrupt     _DCIEInterrupt     _OC11Interrupt                       */
/* _AD2Interrupt      _DCIInterrupt      _IC11Interrupt                       */
/* _IC7Interrupt      _DMA5Interrupt     _OC12Interrupt                       */
/* _IC8Interrupt      _RTCCInterrupt     _IC12Interrupt                       */
/* _DMA2Interrupt     _U1ErrInterrupt    _DMA12Interrupt                      */
/* _OC3Interrupt      _U2ErrInterrupt    _DMA13Interrupt                      */
/* _OC4Interrupt      _CRCInterrupt      _DMA14Interrupt                      */
/* _T4Interrupt       _DMA6Interrupt     _OC13Interrupt                       */
/* _T5Interrupt       _DMA7Interrupt     _IC13Interrupt                       */
/* _INT2Interrupt     _C1TxReqInterrupt  _OC14Interrupt                       */
/* _U2RXInterrupt     _C2TxReqInterrupt  _IC14Interrupt                       */
/* _U2TXInterrupt     _QEI2Interrupt     _OC15Interrupt                       */
/* _SPI2ErrInterrupt  _U3ErrInterrupt    _IC15Interrupt                       */
/* _SPI2Interrupt     _U3RXInterrupt     _OC16Interrupt                       */
/* _C1RxRdyInterrupt  _U3TXInterrupt     _IC16Interrupt                       */
/* _C1Interrupt       _USB1Interrupt     _ICDInterrupt                        */
/* _DMA3Interrupt     _U4ErrInterrupt    _PWMSpEventMatchInterrupt            */
/* _IC3Interrupt      _U4RXInterrupt     _PWMSecSpEventMatchInterrupt         */
/*                                                                            */
/* For alternate interrupt vector naming, simply add 'Alt' between the prim.  */
/* interrupt vector name '_' and the first character of the primary interrupt */
/* vector name.  There is no Alternate Vector or 'AIVT' for the 33E family.   */
/*                                                                            */
/* For example, the vector name _ADC2Interrupt becomes _AltADC2Interrupt in   */
/* the alternate vector table.                                                */
/*                                                                            */
/* Example Syntax:                                                            */
/*                                                                            */
/* void __attribute__((interrupt,auto_psv)) <Vector Name>(void)               */
/* {                                                                          */
/*     <Clear Interrupt Flag>                                                 */
/* }                                                                          */
/*                                                                            */
/* For more comprehensive interrupt examples refer to the C30 (MPLAB C        */
/* Compiler for PIC24 MCUs and dsPIC DSCs) User Guide in the                  */
/* <C30 compiler instal directory>/doc directory for the latest compiler      */
/* release.  For XC16, refer to the MPLAB XC16 C Compiler User's Guide in the */
/* <XC16 compiler instal directory>/doc folder.                               */
/*                                                                            */
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

#define ERR   3

#define ENC1_PINS   (portd >> 8)
#define ENC2_PINS   (portd >> 10)
#define ENC3_PINS   (portd >> 3)
#define ENC4_PINS   (portf >> 0)


const char soft_qei_lut[16] = {
// http://electronics.stackexchange.com/a/99949
// https://github.com/machinekit/machinekit/blob/master/src/hal/drivers/hal_pru_generic/encoder.c
//
// LUT used to decide when/how to modify count value
// LUT index value consists of 4-bits:
// B_new A_new B_old A_old
//
// PRU only does unsigned math, so newcout = count + LUT_Value - 1
//      LUT = -1 : Count--
//      LUT =  0 : No change
//      LUT =  1 : Count++
//      LUT =  3 : ERROR
//
//                   New New Old Old
// Quadrature   B A | B   A   B   A
//  x4 Mode    ====================
     0,      // 0 0 | 0   0   0   0
    -1,      // 0 - | 0   0   0   1
    +1,      // - 0 | 0   0   1   0
   ERR,      // - - | 0   0   1   1
    +1,      // 0 + | 0   1   0   0
     0,      // 0 1 | 0   1   0   1
   ERR,      // - + | 0   1   1   0
    -1,      // - 1 | 0   1   1   1
    -1,      // + 0 | 1   0   0   0
   ERR,      // + - | 1   0   0   1
     0,      // 1 0 | 1   0   1   0
    +1,      // 1 - | 1   0   1   1
   ERR,      // + + | 1   1   0   0
    +1,      // + 1 | 1   1   0   1
    -1,      // 1 + | 1   1   1   0
     0,      // 1 1 | 1   1   1   1
};

void __attribute__((interrupt,auto_psv)) _CNInterrupt()
{
    char increment;
    
    unsigned int portd = PORTD;
    unsigned int __attribute__((unused)) portf = PORTF; // Unused if ENC4 is not linked to a software QEI.
    _CNIF = 0; // Clear interrupt.
    
    static unsigned char qei3_lut_index = -1;
    static unsigned char qei4_lut_index = -1;
    
    qei3_lut_index = ((qei3_lut_index & 0x03) << 2) | (QEI3_PINS & 0x03);
    qei4_lut_index = ((qei4_lut_index & 0x03) << 2) | (QEI4_PINS & 0x03);
    
    increment = soft_qei_lut[qei3_lut_index];
    if (increment == ERR) { // Probably trigged on boot.
        qei3_errors++;
    }
    else {
        qei3_position += increment;
    }
    
    increment = soft_qei_lut[qei4_lut_index];
    if (increment == ERR) { // Probably trigged on boot.
        qei4_errors++;
    }
    else {
        qei4_position += increment;
    }
}

unsigned long Read32bitQEI3VelocityCounter()
{
    _CNIE = 0;
    unsigned long position = qei3_position;
    qei3_position = 0;
    _CNIE = 1;
    return position;
}

unsigned long Read32bitQEI4VelocityCounter()
{
    _CNIE = 0;
    unsigned long position = qei4_position;
    qei4_position = 0;
    _CNIE = 1;
    return position;
}
