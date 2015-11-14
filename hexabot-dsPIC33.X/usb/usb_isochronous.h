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

#ifndef USB_ISOCHRONOUS_H
#define	USB_ISOCHRONOUS_H

#include "usb_config.h"
#include <stdint.h>


typedef struct {
    int test1;
    int test2;
    int test3;
} iso_frame_t;

extern iso_frame_t isochronous_frame;


void IsochronousInitEP(void);
void IsochronousSend(void);


#endif	/* USB_ISOCHRONOUS_H */
