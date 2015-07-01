#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Send commands to Hexabot Controller ver. 0x01"""

import usb.core
import usb.util
import usb.control
import time

COMMAND_TIMEOUT = 1000
COMMAND_INTERFACE = 2
COMMAND_OUT_EP = 3
COMMAND_OUT_SIZE = 64
COMMAND_IN_EP = 3 + 0x80
COMMAND_IN_SIZE = 64 # FIXME: smaller size in read() calls for non blocking?
ISOCHRONOUS_EP = 4

class controller:
    
    def __init__(self):
        self.dev = usb.core.find(idVendor=0x04D8, idProduct=0x000A)
        if self.dev is None:
            raise ValueError('Device not found')

        # Detach kernel driver if necessary
        interface = usb.util.find_descriptor(self.dev.get_active_configuration(),
                                             bInterfaceNumber=COMMAND_INTERFACE)
        if self.dev.is_kernel_driver_active(interface):
            self.dev.detach_kernel_driver(interface)
            print('Kernel driver detached')
        usb.util.claim_interface(self.dev, interface)
    
    def send(self, cmd):
        if len(cmd) >= COMMAND_OUT_SIZE:
            raise ValueError("command too long %s" % cmd)
        return self.dev.write(COMMAND_OUT_EP, cmd,
                              interface=COMMAND_INTERFACE,
                              timeout=COMMAND_TIMEOUT)
    
    def receive(self):
        try:
            s = self.dev.read(COMMAND_IN_EP, COMMAND_IN_SIZE,
                              interface=COMMAND_INTERFACE, timeout=0).tostring()
            return s.decode("utf-8")
        except usb.core.USBError as e:
            if e.args == (110, 'Operation timed out'):
                return ''
            raise

dev = controller()

dev.send("out1")
while True:
    print(dev.receive())
