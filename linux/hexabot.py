#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""USB interface for Hexabot Controller ver. 0x01"""

import usb.core
import usb.util
import usb.control
import time
import struct


CDC_CONTROL_INTERFACE = 0
CDC_DATA_INTERFACE = 1
COMMAND_INTERFACE = 2
METRICS_INTERFACE = 3

COMMAND_OUT_EP = 3
COMMAND_IN_EP = 3 + 0x80
ISOCHRONOUS_OUT_EP = 4
METRICS_OUT_EP = 5

COMMAND_OUT_SIZE = 64
COMMAND_IN_SIZE  = 64
METRICS_OUT_SIZE = 64

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
    
    
    def send_command(self, cmd):
        """Command format: length of payload (1 byte), payload (n bytes), null (1 byte)
           Command payload: """
        cmd = [len(cmd)] + cmd + [0] # Add len header and terminating null byte
        
        if len(cmd) >= COMMAND_OUT_SIZE:
            raise Exception('Command too long %s' % cmd)
        written = self.dev.write(COMMAND_OUT_EP, cmd,
                                 interface=COMMAND_INTERFACE,
                                 timeout=COMMAND_TIMEOUT)
        if written != len(cmd):
            raise Exception('Wrote %d bytes instead of %d' % (written, len(cmd)))
    
    
    def receive_command(self):
        answer = self.dev.read(COMMAND_IN_EP, COMMAND_IN_SIZE, interface=COMMAND_INTERFACE,
                               timeout=COMMAND_TIMEOUT) # Header
        
        if answer[0] + 2 != len(answer):
            raise Exception('Unexpected header=%d for answer len=%d' % (answer[0], len(answer)))
        
        if answer[-1] != 0:
            raise Exception('Wrong end of packet %d' % answer[-1])
        
        return answer[1:-1].tostring() # Convert payload from array
    
    
    def try_receive(self):
        try:
            return receive()
        except usb.core.USBError as e:
            if e.args == (110, 'Operation timed out'):
                return ''
            else:
                raise
    
    def handle_isochronous():
        pass # TODO
    
    def dump_metrics(self):
        fmt = struct.Struct('<BHf') # little-endian, uint8_t, uint16_t, float
        
        last_realtime = -1
        last_datatime = -1
        
        with open('metrics.txt', 'a') as file_out:
            try:
                while True:
                    raw_metrics = self.dev.read(METRICS_OUT_EP, METRICS_OUT_SIZE,
                                                interface=METRICS_INTERFACE, timeout=0).tostring() # FIXME: work on array to skip tostring()
                    
                    if len(raw_metrics) % fmt.calcsize(METRICS_FORMAT) != 0:
                        raise Exception('Unaligned metrics size %d' % len(raw_metrics))
                    
                    for (m_id, m_datatime, m_value) in fmt.iter_unpack(raw_metrics):
                        if last_realtime == -1: # This will be the reference point
                            last_realtime = time.time()
                            last_datatime = m_datatime
                        
                        # TODO: elif last_datatime < m_datatime: # Rollover
                        #~ else:
                            #~ last_realtime = last_realtime + (m_datatime - last_datatime) * 1 # TODO
                            #~ last_datatime = m_datatime
                            # TODO: if last_realtime < time.time() + ticker_max_delay: we lost track of time
                        
                        line = "%d %f %f\n" % (m_id, last_realtime, m_value)
                        print(line)
                        file_out.write(line)
            except KeyboardInterrupt:
                pass