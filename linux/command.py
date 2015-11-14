#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Send command and read response"""

import usb_controller
import sys
import struct


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: %s <byte1> ..." % sys.argv[0])
        exit(1)
    
    dev = usb_controller.USBController([usb_controller.COMMAND_INTERFACE])
    
    args = [ eval(i) for i in sys.argv[1:]] # Convert logical expressions and hexadecimal
    
    dev.send_command(args)
    answer = dev.receive_command()
    print(' '.join([format(i, '02x') for i in answer]))
    
    formats = []
    if len(answer) == 1:
        formats.append('c') # char
    elif len(answer) == 2:
        formats.append('h') # short
    elif len(answer) == 4:
        formats.append('i') # int
        formats.append('f') # float
    elif len(answer) == 4:
        formats.append('q') # long
        formats.append('d') # double
    
    for f in formats:
        print(struct.unpack(f, answer)[0])
