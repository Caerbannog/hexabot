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

/** INCLUDES *******************************************************/
#include "app.h"
#include "app_led_usb_status.h"
#include "app_registers.h"
#include <usb/usb_commands.h>
#include <usb/usb_metrics.h>
#include <usb/usb_isochronous.h>
#include <usb/usb_debug.h>
#include <usb/usb_config.h>
#include <usb/usb.h>
#include <system.h>
#include <timer.h>
#include <outcompare.h>

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <uart.h>
#include <qei32.h>
#include <math.h>


#include <stdlib.h>

/** VARIABLES ******************************************************/

static uint8_t readBuffer[CDC_DATA_OUT_EP_SIZE];
static uint8_t writeBuffer[CDC_DATA_IN_EP_SIZE];

/*********************************************************************
* Overview: Initializes the demo code
*
* PreCondition: None
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_Initialize()
{
    DebugInit();
    CDCInitEP();
    CommandInitEP();
    MetricsInitEP();
    IsochronousInitEP();
    OpenTimer23(T2_ON & T2_IDLE_STOP & T2_GATE_OFF & T2_PS_1_256 & T2_32BIT_MODE_ON
               & T2_SOURCE_INT & T2_INT_PRIOR_0 & T2_INT_OFF, 0xFFFFFFFF); // Will overflow after 5.09 hours (prescaler * period / fcy / 3600).
}
#define TIMER23_TICK_S          (256 /*prescaler*/ / (float)Fcy)
#define MOTOR_TICKS_PER_METER   57000 // Approximate
// encoder ticks per motor turn 6.25*500 = 3125, belt ratio=?
#define REVERSED_MOTOR   (-1) // Reversed because motors are in opposite directions.

/*********************************************************************
* Overview: Keeps the demo running.
*
* PreCondition: The demo should have been initialized and started via
*   the APP_Initialize() and APP_Start() demos
*   respectively.
*
* Input: None
*
* Output: None
*
********************************************************************/
void APP_Tasks()
{
#if 0 // UART1 test
    while (DataRdyUART1()) {
        char c = getcUART1();
        if (!BusyUART1()) {
            WriteUART1(c);
        }
    }
#endif

#if 0 // ADC test
    static unsigned int i = 0;
    i++;
    if (i >= 5000) { // FIXME: read a timer instead
        i = 0;
        if (_AD1IF) {
            _AD1IF = 0;
            // TODO: aliasing problem? when the PWM is off, the voltage is useless
            //       solution: either sample when PWM is on, or add a low pass filter on VSENSE
            // TODO: sensitivity problem, need higher resistance
            MetricsAppend(METRIC_SENSE_R, ReadADC1(0));
            MetricsAppend(METRIC_SENSE_L, ReadADC1(1));
            MetricsAppend(METRIC_AN3,     ReadADC1(2));
            MetricsAppend(METRIC_VBAT,    ReadADC1(3));
        }
    }
#endif
    
#if 0 // Servo test
    SetDCOC3PWM(1, (0.002 * servo1 / 256 + 0.0005) * (Fcy / 256)); // 0 => 0.5ms ; 255 => 2.5ms
#endif

#if 0 // Watch absolute encoder values.
    MetricsAppend(7, Read32bitQEI1PositionCounter());
    MetricsAppend(7, Read32bitQEI2PositionCounter());
#endif

#if 1 // PID control
    static unsigned long last_time_asserv = 0;
    unsigned long new_time_asserv = ReadTimer23();
    float elapsed_asserv = (float)(new_time_asserv - last_time_asserv) * TIMER23_TICK_S;
    if (elapsed_asserv > control_loop_interval) { // Update asserv
        last_time_asserv = new_time_asserv;
        
        unsigned int ticks_r = Read32bitQEI1VelocityCounter();
        float r_speed = (*(int*)&ticks_r) / elapsed_asserv / (float)MOTOR_TICKS_PER_METER;
        
        static float r_speed_err_P_previous = 0;
        static float r_speed_err_I = 0;
        
        float r_speed_err_P = r_target_speed - r_speed;
        float r_speed_err_D = (r_speed_err_P - r_speed_err_P_previous) / elapsed_asserv;
        
        r_speed_err_P_previous = r_speed_err_P;
        
        r_control_speed = r_speed_err_P * KP + r_speed_err_I * KI + r_speed_err_D * KD;
        r_speed_err_I += r_speed_err_P * elapsed_asserv;
        if (r_speed_err_I * KI > 0.999) { // Arbitrary threshold. The integrate error shouldn't accumulate to infinity.
            r_speed_err_I = 0.999;
        }
        else if (r_speed_err_I * KI < -0.999) {
            r_speed_err_I = -0.999;
        }
        
        /*
        MetricsAppend(0, ticks_r);
        MetricsAppend(1, r_target_speed);
        MetricsAppend(2, r_speed);
        MetricsAppend(3, r_speed_err_P);
        MetricsAppend(4, r_control_speed);
        //*/
        
        unsigned int ticks_l = Read32bitQEI2VelocityCounter();
        float l_speed = (REVERSED_MOTOR * *(int*)&ticks_l) / elapsed_asserv / (float)MOTOR_TICKS_PER_METER;
        
        static float l_speed_err_P_previous = 0;
        static float l_speed_err_I = 0;
        
        float l_speed_err_P = l_target_speed - l_speed;
        float l_speed_err_D = (l_speed_err_P - l_speed_err_P_previous) / elapsed_asserv;
        
        l_speed_err_P_previous = l_speed_err_P;
        
        l_control_speed = l_speed_err_P * KP + l_speed_err_I * KI + l_speed_err_D * KD;
        l_speed_err_I += l_speed_err_P * elapsed_asserv;
        if (l_speed_err_I * KI > 0.999) { // Arbitrary threshold. The integrate error shouldn't accumulate to infinity.
            l_speed_err_I = 0.999;
        }
        else if (l_speed_err_I * KI < -0.999) {
            l_speed_err_I = -0.999;
        }
        
        /*
        MetricsAppend(0, ticks_l);
        MetricsAppend(1, l_target_speed);
        MetricsAppend(2, l_speed);
        MetricsAppend(3, l_speed_err_P);
        MetricsAppend(4, l_control_speed);
        //*/
    }
#elif 0 // QEI test
    int vel1 = Read32bitQEI1VelocityCounter();
    int vel2 = Read32bitQEI2VelocityCounter();
    if (vel1 != 0) {
        MetricsAppend(4, vel1);
    }
    if (vel2 != 0) {
        MetricsAppend(5, vel2);
    }
#endif

#if 1 // Motor control
  #if 1 // Convert floats and directions.
    motor_r_dir = (r_control_speed < 0); // REVERSED_MOTOR
    int pwm_r = abs(r_control_speed * 255);
    if (pwm_r > 255)
        pwm_r = 255;
    motor_r_pwm = 255 - pwm_r;
    
    motor_l_dir = (l_control_speed >= 0);
    int pwm_l = abs(l_control_speed * 255);
    if (pwm_l > 255)
        pwm_l = 255;
    motor_l_pwm = 255 - pwm_l;
  #endif

    // TODO: turn OFF motors when USB link is broken: heartbeat ?
    // TODO: command to reset the MCU
    SetDCOC1PWM(1, MOTOR_PWM_PERIOD / 256.0 * motor_r_pwm);
    LATEbits.LATE1 = (motor_r_dir == 0);
    LATEbits.LATE2 = (motor_r_dir != 0);
    SetDCOC2PWM(1, MOTOR_PWM_PERIOD / 256.0 * motor_l_pwm);
    LATEbits.LATE4 = (motor_l_dir == 0);
    LATEbits.LATE6 = (motor_l_dir != 0);
#endif

#if 1 // Odometry test
    static int k = 0;
    if (++k % odometry_resolution == 0) {
        int r_ticks = Read32bitQEI4VelocityCounter() * REVERSED_MOTOR;
        int l_ticks = Read32bitQEI3VelocityCounter();
        
        float delta_d = odometry_r_arc / 2 * r_ticks
                      + odometry_l_arc / 2 * l_ticks;
        
        odometry_theta += odometry_r_arc * r_ticks * (1 + odometry_rotation_imbalance) / half_wheel_distance
                        - odometry_l_arc * l_ticks * (1 - odometry_rotation_imbalance) / half_wheel_distance;
        
        odometry_x += delta_d * cos(odometry_theta);
        odometry_y += delta_d * sin(odometry_theta);
        odometry_d += delta_d;
        
        static int m = 0;
        if (++m % 25 == 0) {
            MetricsAppend(11, odometry_x);
            MetricsAppend(12, odometry_y);
            MetricsAppend(13, odometry_theta);
            MetricsAppend(14, odometry_d);
        }
    }
#endif
    
    if (BUTTON_IsPressed(BUTTON_S1)) {
        //LOG("PRESSED S1");
        static int i = 0;
        i++;
        if (i % 100 == 0) {
            MetricsAppend(METRIC_DEBUG_1, i);
        }
    }
    
    if (BUTTON_IsPressed(BUTTON_S2)) {
        char * str = "COMMAND_IN";
        CommandInSend((uint8_t *)str, strlen(str));
    }
    
    if (false) {
        IsochronousSend();
    }
    
    uint8_t buffer[CDC_DATA_OUT_EP_SIZE];
    uint8_t received = CommandOutPoll(buffer, sizeof(buffer));
    if (received > 0) {
        //LOG("received=%d", received);
        //putUSBUSART(buffer, received);
        register_command(buffer, received);
    }

    /* Check to see if there is a transmission in progress, if there isn't, then
     * we can see about performing an echo response to data received.
     */
    if( USBUSARTIsTxTrfReady() == true)
    {
        uint8_t i;
        uint8_t numBytesRead;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));

        /* For every byte that was read... */
        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer[i])
            {
                /* If we receive new line or line feed commands, just echo
                 * them direct.
                 */
                case 0x0A:
                case 0x0D:
                    writeBuffer[i] = readBuffer[i];
                    break;

                /* If we receive something else, then echo it plus one
                 * so that if we receive 'a', we echo 'b' so that the
                 * user knows that it isn't the echo enabled on their
                 * terminal program.
                 */
                default:
                    writeBuffer[i] = readBuffer[i] + 1;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            /* After processing all of the received data, we need to send out
             * the "echo" data now.
             */
            putUSBUSART(writeBuffer,numBytesRead);
        }
    }

    DebugService();
    CDCService();
    CommandService();
    MetricsService();
}
