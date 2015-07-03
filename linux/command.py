#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Send command and read response"""

import hexabot
import sys
import ast


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: %s <byte ...>" % sys.argv[0])
        exit(1)
    
    dev = hexabot.controller([hexabot.COMMAND_INTERFACE])
    
    args = [ eval(i) for i in sys.argv[1:]] # Convert logical expressions and hexadecimal
    
    dev.send_command(args)
    answer = dev.receive_command()
    print(' '.join([format(i, '02x') for i in answer]))
