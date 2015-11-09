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

#include <system.h>
#include <system_config.h>
#include <usb/usb.h>
#include <leds.h>
#include <buttons.h>

#include <adc.h>
#include <uart.h>
#include <spi.h>
#include <i2c.h>
#include <qei32.h>
#include <pps.h>
#include <outcompare.h>
#include <timer.h>
#include <ports.h>

#include <xc.h>

/** CONFIGURATION Bits **********************************************/
// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Initial Oscillator Source Selection Bits (Primary Oscillator (XT, HS, EC) with PLL)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = HS              // Primary Oscillator Mode Select bits (HS Crystal Oscillator Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = OFF            // Peripheral pin select configuration (Allow multiple reconfigurations)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler Bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = OFF              // Brown-out Reset (BOR) Detection Enable bit (BOR is disabled)
#pragma config ALTI2C1 = OFF            // Alternate I2C pins for I2C1 (SDA1/SCK1 pins are selected as the I/O pins for I2C1)
#if defined(__dsPIC33EP256MU810__) || defined(__dsPIC33EP512MU810__)
#pragma config ALTI2C2 = OFF            // Alternate I2C pins for I2C2 (SDA2/SCK2 pins are selected as the I/O pins for I2C2)
#endif

// FICD
#pragma config ICS = PGD1               // ICD Communication Channel Select bits (Communicate on PGEC1 and PGED1)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Auxiliary program memory is not write-protected)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)



#define PIN_INPUT           1
#define PIN_OUTPUT          0
#define ENABLE_PULLUP       1
#define ENABLE_PULDOWN      1


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
void SYSTEM_Initialize( SYSTEM_STATE state )
{
    switch(state)
    {
        case SYSTEM_STATE_USB_START:
            // Configure the device PLL to obtain 60 MIPS operation. The crystal
            // frequency is 8MHz. Divide 8MHz by 2, multiply by 60 and divide by
            // 2. This results in Fosc of 120MHz. The CPU clock frequency is
            // Fcy = Fosc/2 = 60MHz. Wait for the Primary PLL to lock and then
            // configure the auxilliary PLL to provide 48MHz needed for USB
            // Operation.

            PLLFBD = 58;                        /* M  = 60  */
            CLKDIVbits.PLLPOST = 0;             /* N1 = 2   */
            CLKDIVbits.PLLPRE = 0;              /* N2 = 2   */
            OSCTUN = 0;

            /*	Initiate Clock Switch to Primary
             *	Oscillator with PLL (NOSC= 0x3)*/
            __builtin_write_OSCCONH(0x03);
            __builtin_write_OSCCONL(0x01);

            while (OSCCONbits.COSC != 0x3);

            // Configuring the auxiliary PLL, since the primary
            // oscillator provides the source clock to the auxiliary
            // PLL, the auxiliary oscillator is disabled. Note that
            // the AUX PLL is enabled. The input 8MHz clock is divided
            // by 2, multiplied by 24 and then divided by 2. Wait till
            // the AUX PLL locks.

            ACLKCON3 = 0x24C1;
            ACLKDIV3 = 0x7;

            ACLKCON3bits.ENAPLL = 1;
            while(ACLKCON3bits.APLLCK != 1);

            
            LED_Enable(LED_R);
            LED_Enable(LED_Y);
            LED_Enable(LED_G);
            BUTTON_Enable(BUTTON_S1);
            BUTTON_Enable(BUTTON_S2);
            
            
            // TODO BUMP et START sampling with debouncing
            // TODO H-bridge vsense
            
            // Change notification interrupts for software QEI.
            ConfigIntCNPortD(CHANGE_INT_ON & CHANGE_INT_PRI_4 & 0xFFFFFFFF);
            ConfigIntCNPortF(CHANGE_INT_ON & CHANGE_INT_PRI_4 & 0xFFFFFFFF); // Most likely redundant.
            ENABLE_CN_QEI3;
            ENABLE_CN_QEI4;

            
            // Setup for pull-up and pull-down.
            CNPUBbits.CNPUB13 = ENABLE_PULLUP; // START
            CNPUBbits.CNPUB14 = ENABLE_PULLUP; // FIXME: DYNAMIXEL
            CNPUCbits.CNPUC13 = ENABLE_PULLUP; // BUMP_1
            CNPUCbits.CNPUC14 = ENABLE_PULLUP; // BUMP_2
            CNPDDbits.CNPDD6  = ENABLE_PULDOWN; // PUSH_1
            CNPDDbits.CNPDD7  = ENABLE_PULDOWN; // PUSH_2
            
            // UART for debug
            OpenUART1(UART_EN & UART_IDLE_STOP & UART_IrDA_DISABLE & UART_MODE_SIMPLEX
                    & UART_UEN_00 & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD
                    & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN & UART_NO_PAR_8BIT & UART_1STOPBIT,
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED
                    & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS
                    & UART_RX_OVERRUN_CLEAR,
                    Fcy / 16 / 115200 - 1);
            PPSInput(IN_FN_PPS_U1RX, IN_PIN_PPS_RP99);
            //PPSOutput(OUT_FN_PPS_U1TX, OUT_PIN_PPS_RP69); // HW HACK: TX is on GPIO3
            
            // UART for debug
            OpenUART2(UART_EN & UART_IDLE_STOP & UART_IrDA_DISABLE & UART_MODE_SIMPLEX
                    & UART_UEN_00 /*FIXME*/ & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD
                    & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN & UART_NO_PAR_8BIT & UART_1STOPBIT,
                    UART_INT_TX_BUF_EMPTY & UART_IrDA_POL_INV_ZERO & UART_SYNC_BREAK_DISABLED
                    & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS
                    & UART_RX_OVERRUN_CLEAR,
                    Fcy / 16 / 9600 - 1);
            // HW FIXME: PPSInput(IN_FN_PPS_U2RX, IN_PIN_PPS_RP);
            // HW FIXME: PPSOutput(OUT_FN_PPS_U2TX, OUT_PIN_PPS_RP);
            
            /* FIXME: test SPI
            OpenSPI1(ENABLE_SCK_PIN & ENABLE_SDO_PIN & SPI_MODE16_OFF & SPI_SMP_OFF
                    & SPI_CKE_ON & SLAVE_ENABLE_OFF & CLK_POL_ACTIVE_LOW & MASTER_ENABLE_ON
                    & SEC_PRESCAL_1_1 & PRI_PRESCAL_1_1,
                    FRAME_ENABLE_OFF & FRAME_SYNC_INPUT & FRAME_ENABLE_OFF & FRAME_SYNC_EDGE_COINCIDE
                    & FIFO_BUFFER_DISABLE,
                    SPI_ENABLE & SPI_IDLE_STOP & SPI_RX_OVFLOW_CLR);
            */
            
            /* FIXME: test I2C
            OpenI2C1(I2C1_ON & I2C1_IDLE_STOP & I2C1_CLK_REL & I2C1_IPMI_DIS & I2C1_7BIT_ADD
                    & I2C1_SLW_DIS & I2C1_SM_DIS & I2C1_GCALL_DIS & I2C1_STR_DIS
                    & I2C1_NACK & I2C1_ACK_DIS & I2C1_RCV_DIS & I2C1_STOP_DIS & I2C1_RESTART_DIS
                    & I2C1_START_DIS,
                    I2C1_STOP_COND_INT_DIS & I2C1_START_COND_INT_DIS & I2C1_BUF_OVERWRITE_DIS
                    & I2C1_SDA_HOLD_TIME_SEL_100 & I2C1_SLAVE_BUS_COL_DETECT_DIS
                    & I2C1_ADDR_HOLD_DIS & I2C1_DATA_HOLD_DIS);
            */
            
            Open32bitQEI1(QEI_COUNTER_QEI_MODE & QEI_GATE_DISABLE & QEI_COUNT_POSITIVE
                    & QEI_INPUT_PRESCALE_1 & QEI_INDEX_MATCH_NO_EFFECT & QEI_POS_COUNT_INIT_No_EFFECT
                    & QEI_IDLE_STOP & QEI_COUNTER_ENABLE,
                    QEI_QEA_POL_NON_INVERTED & QEI_QEB_POL_NON_INVERTED & QEI_INDX_POL_NON_INVERTED
                    & QEI_HOM_POL_NON_INVERTED & QEI_QEA_QEB_NOT_SWAPPED
                    & QEI_COMPARE_HIGH_OUTPUT_DISABLE & QEI_DIF_FLTR_PRESCALE_1 & QEI_DIG_FLTR_DISABLE // FIXME: test filter with the same encoder on two QEIs with different config
                    & QEI_POS_COUNT_TRIG_DISABLE,
                    QEI_INDEX_INTERRUPT_DISABLE & QEI_HOME_INTERRUPT_DISABLE & QEI_VELO_OVERFLOW_INTERRUPT_DISABLE
                    & QEI_POS_INIT_INTERRUPT_DISABLE & QEI_POS_OVERFLOW_INTERRUPT_DISABLE
                    & QEI_POS_LESS_EQU_INTERRUPT_DISABLE & QEI_POS_GREAT_EQU_INTERRUPT_DISABLE);
            Open32bitQEI2(QEI_COUNTER_QEI_MODE & QEI_GATE_DISABLE & QEI_COUNT_POSITIVE
                    & QEI_INPUT_PRESCALE_1 & QEI_INDEX_MATCH_NO_EFFECT & QEI_POS_COUNT_INIT_No_EFFECT
                    & QEI_IDLE_STOP & QEI_COUNTER_ENABLE,
                    QEI_QEA_POL_NON_INVERTED & QEI_QEB_POL_NON_INVERTED & QEI_INDX_POL_NON_INVERTED
                    & QEI_HOM_POL_NON_INVERTED & QEI_QEA_QEB_NOT_SWAPPED
                    & QEI_COMPARE_HIGH_OUTPUT_DISABLE & QEI_DIF_FLTR_PRESCALE_1 & QEI_DIG_FLTR_DISABLE
                    & QEI_POS_COUNT_TRIG_DISABLE,
                    QEI_INDEX_INTERRUPT_DISABLE & QEI_HOME_INTERRUPT_DISABLE & QEI_VELO_OVERFLOW_INTERRUPT_DISABLE
                    & QEI_POS_INIT_INTERRUPT_DISABLE & QEI_POS_OVERFLOW_INTERRUPT_DISABLE
                    & QEI_POS_LESS_EQU_INTERRUPT_DISABLE & QEI_POS_GREAT_EQU_INTERRUPT_DISABLE); // Same config.
            /* ENC_1
            PPSInput(IN_FN_PPS_QEA1, IN_PIN_PPS_RPI72);
            PPSInput(IN_FN_PPS_QEB1, IN_PIN_PPS_RPI73);
            //*/
            //* ENC_2 MOTOR_R
            PPSInput(IN_FN_PPS_QEA2, IN_PIN_PPS_RPI74);
            PPSInput(IN_FN_PPS_QEB2, IN_PIN_PPS_RPI75);
            //*/
            /* ENC_3
            PPSInput(IN_FN_PPS_QEA1, IN_PIN_PPS_RP67);
            PPSInput(IN_FN_PPS_QEB1, IN_PIN_PPS_RP68);
            //*/
            //* ENC_4 MOTOR_L
            PPSInput(IN_FN_PPS_QEA1, IN_PIN_PPS_RPI96);
            PPSInput(IN_FN_PPS_QEB1, IN_PIN_PPS_RP97);
            //*/
            
            // ADC continuous conversion
            OpenADC1(ADC_MODULE_ON & ADC_IDLE_STOP & ADC_ADDMABM_ORDER & ADC_AD12B_12BIT
                    & ADC_FORMAT_INTG & ADC_SSRC_AUTO & ADC_AUTO_SAMPLING_ON & ADC_MULTIPLE
                    & ADC_SAMP_OFF,
                    ADC_VREF_AVDD_AVSS & ADC_SCAN_ON & ADC_SELECT_CHAN_0 & ADC_ALT_BUF_ON
                    & ADC_ALT_INPUT_OFF & ADC_DMA_ADD_INC_4, // Number of channels to scan.
                    ADC_SAMPLE_TIME_31 & ADC_CONV_CLK_INTERNAL_RC & ADC_CONV_CLK_256Tcy,
                    ADC_DMA_DIS & ADC_DMA_BUF_LOC_128,
                    0, ENABLE_AN1_ANA | ENABLE_AN2_ANA | ENABLE_AN3_ANA | ENABLE_AN4_ANA, 0, 0, 0, 0, 0, 0, 0, 0, 0, // Set most pins to digital instead of analog.
                    SCAN_NONE_16_31,
                    ENABLE_AN1_ANA | ENABLE_AN2_ANA | ENABLE_AN3_ANA | ENABLE_AN4_ANA);
            
            // PWM for Motors
            OpenTimer4(T4_ON & T4_IDLE_STOP & T4_GATE_OFF & T4_PS_1_256 & T4_32BIT_MODE_OFF
                       & T4_SOURCE_INT & T4_INT_PRIOR_0 & T4_INT_OFF, MOTOR_PWM_PERIOD);
            OpenOC1(OC_IDLE_STOP & OC_TIMER4_SRC & OC_FAULTA_IN_DISABLE & OC_FAULTB_IN_DISABLE
                    & OC_FAULTC_IN_DISABLE & 0xff8f & OC_TRIG_CLEAR_SYNC & OC_PWM_EDGE_ALIGN,
                    OC_FAULT_MODE_PWM_CYCLE & OC_PWM_FAULT_OUT_LOW & (~OC_FAULT_PIN_OUT)
                    & OC_OUT_NOT_INVERT & OC_CASCADE_DISABLE & OC_SYNC_ENABLE
                    & OC_TRIGGER_TIMER & OC_DIRN_OUTPUT & OC_SYNC_TRIG_IN_TMR4,
                    0, 0);
            PPSOutput(OUT_FN_PPS_OC1, OUT_PIN_PPS_RP80); // MOTOR_R_PWM
            TRISEbits.TRISE1 = PIN_OUTPUT; // MOTOR_R_DIR1
            TRISEbits.TRISE2 = PIN_OUTPUT; // MOTOR_R_DIR2
            OpenOC2(OC_IDLE_STOP & OC_TIMER4_SRC & OC_FAULTA_IN_DISABLE & OC_FAULTB_IN_DISABLE
                    & OC_FAULTC_IN_DISABLE & 0xff8f & OC_TRIG_CLEAR_SYNC & OC_PWM_EDGE_ALIGN,
                    OC_FAULT_MODE_PWM_CYCLE & OC_PWM_FAULT_OUT_LOW & (~OC_FAULT_PIN_OUT)
                    & OC_OUT_NOT_INVERT & OC_CASCADE_DISABLE & OC_SYNC_ENABLE
                    & OC_TRIGGER_TIMER & OC_DIRN_OUTPUT & OC_SYNC_TRIG_IN_TMR4,
                    0, 0); // Same config.
            PPSOutput(OUT_FN_PPS_OC2, OUT_PIN_PPS_RP85); // MOTOR_L_PWM
            TRISEbits.TRISE4 = PIN_OUTPUT; // MOTOR_L_DIR1
            TRISEbits.TRISE6 = PIN_OUTPUT; // MOTOR_L_DIR2
            
            // PWM for servos
            OpenTimer5(T5_ON & T5_IDLE_STOP & T5_GATE_OFF & T5_PS_1_256 // T5_32BIT_MODE_OFF not applicable
                       & T5_SOURCE_INT & T5_INT_PRIOR_0 & T5_INT_OFF, 0.020 * Fcy / 256);
            OpenOC3(OC_IDLE_STOP & OC_TIMER5_SRC & OC_FAULTA_IN_DISABLE & OC_FAULTB_IN_DISABLE
                    & OC_FAULTC_IN_DISABLE & 0xff8f & OC_TRIG_CLEAR_SYNC & OC_PWM_EDGE_ALIGN,
                    OC_FAULT_MODE_PWM_CYCLE & OC_PWM_FAULT_OUT_LOW & (~OC_FAULT_PIN_OUT)
                    & OC_OUT_NOT_INVERT & OC_CASCADE_DISABLE & OC_SYNC_ENABLE
                    & OC_TRIGGER_TIMER & OC_DIRN_OUTPUT & OC_SYNC_TRIG_IN_TMR5,
                    0, 0);
            PPSOutput(OUT_FN_PPS_OC3, OUT_PIN_PPS_RP87); // SERVO_1
            OpenOC4(OC_IDLE_STOP & OC_TIMER5_SRC & OC_FAULTA_IN_DISABLE & OC_FAULTB_IN_DISABLE
                    & OC_FAULTC_IN_DISABLE & 0xff8f & OC_TRIG_CLEAR_SYNC & OC_PWM_EDGE_ALIGN,
                    OC_FAULT_MODE_PWM_CYCLE & OC_PWM_FAULT_OUT_LOW & (~OC_FAULT_PIN_OUT)
                    & OC_OUT_NOT_INVERT & OC_CASCADE_DISABLE & OC_SYNC_ENABLE
                    & OC_TRIGGER_TIMER & OC_DIRN_OUTPUT & OC_SYNC_TRIG_IN_TMR5,
                    0, 0); // Same config.
            // HW FIXME: pin SERVO_2
            
            break;

        case SYSTEM_STATE_USB_SUSPEND:
            break;

        case SYSTEM_STATE_USB_RESUME:
            break;
    }
}

#if defined(USB_INTERRUPT)
void __attribute__((interrupt,auto_psv)) _USB1Interrupt()
{
    USBDeviceTasks();
}
#endif
