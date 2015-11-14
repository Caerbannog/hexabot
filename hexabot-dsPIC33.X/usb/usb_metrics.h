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

#ifndef USB_METRICS_H
#define	USB_METRICS_H

#include "usb_config.h"
#include <stdint.h>


typedef enum {
    METRIC_SENSE_R,
    METRIC_SENSE_L,
    METRIC_AN3,
    METRIC_VBAT,
    
    METRIC_TICKER,
    METRIC_DEBUG_1,
    METRIC_DEBUG_2,
    METRIC_DEBUG_3,
    METRIC_DEBUG_4,
    METRIC_X,
    METRIC_Y,
    METRIC_THETA,
    METRIC_DISTANCE,
    METRIC_SPEED,
    
    METRICS_COUNT
} metric_id_t;

typedef struct __attribute__ ((__packed__)) {
    uint32_t time;
    float value;
    uint8_t metric_id;
} metric_t;


void MetricsInitEP(void);
void MetricsAppend(metric_id_t id, float value);
void MetricsService(void);


#endif	/* USB_METRICS_H */
