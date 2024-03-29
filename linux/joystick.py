#!/usr/bin/env python3
# https://gist.github.com/rdb/8864666

# Released by rdb under the Unlicense (unlicense.org)
# Based on information from:
# https://www.kernel.org/doc/Documentation/input/joystick-api.txt

import sys
import os, struct, array
from fcntl import ioctl

# Iterate over the joystick devices.
print('Available devices:')

for fn in os.listdir('/dev/input'):
    if fn.startswith('js'):
        print('  /dev/input/%s' % (fn))

# We'll store the states here.
axis_states = {}
button_states = {}

# These constants were borrowed from linux/input.h
axis_names = {
    0x00 : 'x',
    0x01 : 'y',
    0x02 : 'z',
    0x03 : 'rx',
    0x04 : 'ry',
    0x05 : 'rz',
    0x06 : 'trottle',
    0x07 : 'rudder',
    0x08 : 'wheel',
    0x09 : 'gas',
    0x0a : 'brake',
    0x10 : 'hat0x',
    0x11 : 'hat0y',
    0x12 : 'hat1x',
    0x13 : 'hat1y',
    0x14 : 'hat2x',
    0x15 : 'hat2y',
    0x16 : 'hat3x',
    0x17 : 'hat3y',
    0x18 : 'pressure',
    0x19 : 'distance',
    0x1a : 'tilt_x',
    0x1b : 'tilt_y',
    0x1c : 'tool_width',
    0x20 : 'volume',
    0x28 : 'misc',
}

button_names = {
    0x120 : 'trigger',
    0x121 : 'thumb',
    0x122 : 'thumb2',
    0x123 : 'top',
    0x124 : 'top2',
    0x125 : 'pinkie',
    0x126 : 'base',
    0x127 : 'base2',
    0x128 : 'base3',
    0x129 : 'base4',
    0x12a : 'base5',
    0x12b : 'base6',
    0x12f : 'dead',
    0x130 : 'a',
    0x131 : 'b',
    0x132 : 'c',
    0x133 : 'x',
    0x134 : 'y',
    0x135 : 'z',
    0x136 : 'tl',
    0x137 : 'tr',
    0x138 : 'tl2',
    0x139 : 'tr2',
    0x13a : 'select',
    0x13b : 'start',
    0x13c : 'mode',
    0x13d : 'thumbl',
    0x13e : 'thumbr',

    0x220 : 'dpad_up',
    0x221 : 'dpad_down',
    0x222 : 'dpad_left',
    0x223 : 'dpad_right',

    # XBox 360 controller uses these codes.
    0x2c0 : 'dpad_left',
    0x2c1 : 'dpad_right',
    0x2c2 : 'dpad_up',
    0x2c3 : 'dpad_down',
}

axis_map = []
button_map = []

# Open the joystick device.
fn = '/dev/input/js0'
print('Opening %s...' % fn)
jsdev = open(fn, 'rb')

# Get the device name.
buf = bytearray(63)
ioctl(jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
print(buf.decode('utf8'))

# Get number of axes and buttons.
buf = array.array('B', [0])
ioctl(jsdev, 0x80016a11, buf) # JSIOCGAXES
num_axes = buf[0]

buf = array.array('B', [0])
ioctl(jsdev, 0x80016a12, buf) # JSIOCGBUTTONS
num_buttons = buf[0]

# Get the axis map.
buf = array.array('B', [0] * 0x40)
ioctl(jsdev, 0x80406a32, buf) # JSIOCGAXMAP

for axis in buf[:num_axes]:
    axis_name = axis_names.get(axis, 'unknown(0x%02x)' % axis)
    axis_map.append(axis_name)
    axis_states[axis_name] = 0.0

# Get the button map.
buf = array.array('H', [0] * 200)
ioctl(jsdev, 0x80406a34, buf) # JSIOCGBTNMAP

for btn in buf[:num_buttons]:
    btn_name = button_names.get(btn, 'unknown(0x%03x)' % btn)
    button_map.append(btn_name)
    button_states[btn_name] = 0

print('%d axes found: %s' % (num_axes, ', '.join(axis_map)))
print('%d buttons found: %s' % (num_buttons, ', '.join(button_map)))

def preview_loop():
    # Main event loop
    while True:
        evbuf = jsdev.read(8)
        if evbuf:
            time, value, type, number = struct.unpack('IhBB', evbuf)

            if type & 0x80:
                 print("(initial)", end="")

            if type & 0x01:
                button = button_map[number]
                if button:
                    button_states[button] = value
                    if value:
                        print("%s pressed" % (button))
                    else:
                        print("%s released" % (button))

            if type & 0x02:
                axis = axis_map[number]
                if axis:
                    fvalue = value / 32767.0
                    axis_states[axis] = fvalue
                    print("%s: %.3f" % (axis, fvalue))

import time
import threading
left_speed = 0.0
right_speed = 0.0
command_pending = True
def control_loop():
    global dev, command_pending, right_speed, left_speed # TODO class
    
    fw_speed = 0
    rot_speed = 0

    while True:
        evbuf = jsdev.read(8)
        if evbuf:
            t, value, type, number = struct.unpack('IhBB', evbuf)

            if type & 0x80:
                 print("(initial)", end="")

            if type & 0x01:
                button = button_map[number]
                if button:
                    button_states[button] = value
                    if value:
                        print("%s pressed" % (button))
                    else:
                        print("%s released" % (button))

            if type & 0x02:
                axis = axis_map[number]
                if axis:
                    fvalue = value / 32767.0
                    axis_states[axis] = fvalue
                    print("%s: %.3f" % (axis, fvalue))
                    
                    if axis == 'x':
                        rot_speed = -fvalue
                    elif axis == 'y':
                        fw_speed = -fvalue
                    else:
                        continue
                    
                    right = fw_speed + rot_speed
                    left  = fw_speed - rot_speed
                    
                    speed_limiter = max(1.0, left, right, -left, -right)
                    right_speed = right / speed_limiter
                    left_speed  = left / speed_limiter
                    
                    command_pending = True

def command_loop(): # Throttle when the joystick sends too many events. FIXME: improve with events.
    global command_pending, dev
    while True:
        while not command_pending:
            time.sleep(.05)
        print("left=%s right=%s" % (left_speed, right_speed))

        command_pending = False
        for (reg, value) in [(38, right_speed), (39, left_speed)]:
            print("cmd=%s" % [reg, value])
            dev.send_command([reg, value])
            answer = dev.receive_command()
            #~ print(' '.join([format(i, '02x') for i in answer]))


if __name__ == '__main__':
    if 'preview' in sys.argv:
        preview_loop()
    else:
        import usb_controller
        dev = usb_controller.USBController([usb_controller.COMMAND_INTERFACE])

        thread = threading.Thread(target=control_loop)
        thread.setDaemon(True)
        thread.start()
    
        command_loop() # Crash main thread on USB error.
