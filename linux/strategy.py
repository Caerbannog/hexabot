#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import time
import usb_controller


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
