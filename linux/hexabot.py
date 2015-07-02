#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""USB interface for Hexabot Controller ver. 0x01"""

import usb.core
import usb.util
import usb.control
import time


# Use CDC with: miniterm.py /dev/ttyACM* 9600
CDC_CONTROL_INTERFACE = 0
CDC_DATA_INTERFACE = 1
COMMAND_INTERFACE = 2
METRICS_INTERFACE = 3

COMMAND_OUT_EP = 3
COMMAND_IN_EP = 3 + 0x80
ISOCHRONOUS_OUT_EP = 4
METRICS_OUT_EP = 5

COMMAND_OUT_SIZE = 64
COMMAND_IN_SIZE = 64 # FIXME: smaller size in read() calls for non blocking?

COMMAND_TIMEOUT = 1000


class controller:
    def __init__(self, interface_numbers):
        self.dev = usb.core.find(idVendor=0x04D8, idProduct=0x000A)
        if self.dev is None:
            raise Exception('Device not found')

        for i in interface_numbers:
            # Detach kernel driver if necessary
            interface = usb.util.find_descriptor(self.dev.get_active_configuration(),
                                                 bInterfaceNumber=i)
            if self.dev.is_kernel_driver_active(interface):
                self.dev.detach_kernel_driver(interface)
                print('Kernel driver detached for %d' % i)
            usb.util.claim_interface(self.dev, interface)
    
    
    def send(self, cmd):
        cmd = [len(cmd)] + cmd + [0] # Add len header and terminating null byte
        if len(cmd) >= COMMAND_OUT_SIZE:
            raise Exception('Command too long %s' % cmd)
        written = self.dev.write(COMMAND_OUT_EP, cmd,
                                 interface=COMMAND_INTERFACE,
                                 timeout=COMMAND_TIMEOUT)
        if written != len(cmd):
            raise Exception('Wrote %d bytes instead of %d' % (written, len(cmd)))
    
    
    def command_read(length, timeout):
        return self.dev.read(COMMAND_IN_EP, length,
                             interface=COMMAND_INTERFACE, timeout=timeout)
    
    
    def receive(self):
        length = self.command_read(1, COMMAND_TIMEOUT) # header
        if length >= COMMAND_IN_SIZE:
            raise Exception('Answer too long %d' % length)
        answer = self.command_read(length + 1, 0) # blocking
        
        if answer[-1] != 0:
            raise Exception('Wrong end of packet %d' % answer[-1])
        
        return s[:-1].tostring() # convert payload from array
    
    
    def try_receive(self):
        try:
            return receive()
        except usb.core.USBError as e:
            if e.args == (110, 'Operation timed out'):
                return ''
            else:
                raise
    
    
    def dump_metrics(self):
        while True:
            s = self.dev.read(COMMAND_IN_EP, COMMAND_IN_SIZE,
                              interface=METRICS_INTERFACE, timeout=0).tostring()
            print(s.decode('utf-8'))
