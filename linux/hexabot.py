#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""USB interface for Hexabot Controller ver. 0x01"""

import usb.core
import usb.util
import usb.control
import time
import struct
import array


CDC_CONTROL_INTERFACE = 0
CDC_DATA_INTERFACE = 1
COMMAND_INTERFACE = 2
ISOCHRONOUS_INTERFACE = 3
METRICS_INTERFACE = 4

COMMAND_OUT_EP = 3
COMMAND_IN_EP = 3 + 0x80
ISOCHRONOUS_IN_EP = 4 + 0x80
METRICS_IN_EP = 5 + 0x80

COMMAND_OUT_SIZE = 64
COMMAND_IN_SIZE  = 64
ISOCHRONOUS_IN_SIZE = 128 # TODO: update
METRICS_IN_SIZE  = 64

COMMAND_TIMEOUT = 1000
INTERACTION_TIMEOUT = 200 # used to respond to KeyboardInterrupt


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
        
        # Replace floats by their byte representation.
        cmd_bytes = array.array('B')
        for c in cmd:
            if isinstance(c, int):
                cmd_bytes.append(c)
            elif isinstance(c, float):
                cmd_bytes.fromstring(struct.pack('f', c)) # Float on four bytes.
            else:
                raise ValueError("Unexpected type %s for arg %s" % (type(c), c))
        cmd = cmd_bytes
        
        cmd.insert(0, len(cmd)) # Add len header
        cmd.append(0) # Add terminating null byte
        
        if len(cmd) >= COMMAND_OUT_SIZE:
            raise Exception('Command too long %s' % cmd)
        written = self.dev.write(COMMAND_OUT_EP, cmd,
                                 interface=COMMAND_INTERFACE,
                                 timeout=COMMAND_TIMEOUT)
        #~ print(' '.join([format(i, '02x') for i in cmd]))
        if written != len(cmd):
            raise Exception('Wrote %d bytes instead of %d' % (written, len(cmd)))
    
    
    def receive_command(self):
        answer = self.dev.read(COMMAND_IN_EP, COMMAND_IN_SIZE, interface=COMMAND_INTERFACE,
                               timeout=COMMAND_TIMEOUT) # Header
        #~ print(' '.join([format(i, '02x') for i in answer]))
        
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
    
    
    def dump_isochronous(self):
        while True:
            try:
                frame = self.dev.read(ISOCHRONOUS_IN_EP, ISOCHRONOUS_IN_SIZE,
                                      interface=ISOCHRONOUS_INTERFACE,
                                      timeout=INTERACTION_TIMEOUT)
                if frame:
                    print(' '.join([format(i, '02x') for i in frame]))
            except KeyboardInterrupt:
                return
            except usb.core.USBError as e:
                if e.args == (110, 'Operation timed out'):
                    continue
                else:
                    raise
            
    
    def dump_metrics(self):
        TIMER_FREQ = 256. / 60e6
        
        fmt = struct.Struct('<LfB') # little-endian, uint32_t, float, uint8_t
        
        try:
            while True: # Flush pending transfers to get accurate timings.
                raw_metrics = self.dev.read(METRICS_IN_EP, METRICS_IN_SIZE,
                                            interface=METRICS_INTERFACE,
                                            timeout=1)
        except:
            pass # Assume timeout.
        
        reference_realtime = -1
        reference_timer = -1
        
        with open('../metrics.txt', 'a') as file_out:
            while True:
                try:
                    raw_metrics = self.dev.read(METRICS_IN_EP, METRICS_IN_SIZE,
                                                interface=METRICS_INTERFACE,
                                                timeout=INTERACTION_TIMEOUT)
                    
                    if len(raw_metrics) % fmt.size != 0:
                        raise Exception('Unaligned metrics size %d' % len(raw_metrics))
                    
                    for (m_timer, m_value, m_id) in fmt.iter_unpack(raw_metrics):
                        if reference_realtime == -1: # This will be the reference point
                            reference_realtime = time.time()
                            reference_timer = m_timer
                        
                        realtime = reference_realtime + (m_timer - reference_timer) * TIMER_FREQ
                        
                        line = "%d %f %.2f" % (m_id, realtime, m_value)
                        print(line)
                        file_out.write(line + "\n")
                        file_out.flush()
                except KeyboardInterrupt:
                    return
                except usb.core.USBError as e:
                    if e.args == (110, 'Operation timed out'):
                        continue
                    else:
                        raise
