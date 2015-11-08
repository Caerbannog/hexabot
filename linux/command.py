#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Send command and read response"""

import hexabot
import sys
import struct


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: %s <byte ...>" % sys.argv[0])
        exit(1)
    
    dev = hexabot.controller([hexabot.COMMAND_INTERFACE])
    
    args = [ eval(i) for i in sys.argv[1:]] # Convert logical expressions and hexadecimal
    
    arg_bytes = []
    for b in args:
        if isinstance(b, int):
            arg_bytes.append(b)
        elif isinstance(b, float):
            arg_bytes += list(struct.pack('f', b)) # Float on four bytes.
        else:
            raise ValueError("Unexpected type %s for arg %s" % (type(b), b))
    
    dev.send_command(arg_bytes)
    answer = dev.receive_command()
    print(' '.join([format(i, '02x') for i in answer]))
