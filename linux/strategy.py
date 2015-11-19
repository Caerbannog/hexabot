#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import usb_controller

def dead_zone_characterization():
    # TODO:
    # set pid_interval to 9999
    # try lots of l/r_control_speed values
    # read POS1CNTL and POS2CNTL to estimate actual speed
    # graph the output
    pass

class Strategy():
    def __init__(self):
        self.dev = usb_controller.USBController([usb_controller.COMMAND_INTERFACE])
    
    def calibration(self):
        pass
    
    def run(self):
        while True:
            self.dev.send_command([...])
            answer = dev.receive_command()
            print(' '.join([format(i, '02x') for i in answer]))


if __name__ == "__main__":
    Strategy().run()
