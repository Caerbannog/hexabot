#!/usr/bin/python3

"""Send commands to Hexabot Controller ver. 0x01"""

import usb.core
import usb.util
import usb.control

INTERFACE_NUMBER = 2
COMMAND_EP = 3
COMMAND_OUT_SIZE = 64
COMMAND_IN_SIZE = 5 # FIXME: smaller size in read() calls for non blocking?
ISOCHRONOUS_EP = 4
TIMEOUT = 2000


dev = usb.core.find(idVendor=0x04D8, idProduct=0x000A)
if dev is None:
    raise ValueError('Device not found')

# Detach kernel driver if necessary
itf = usb.util.find_descriptor(dev.get_active_configuration(), bInterfaceNumber=INTERFACE_NUMBER)
if dev.is_kernel_driver_active(itf):
	dev.detach_kernel_driver(itf)
	print('Kernel driver detached')
usb.util.claim_interface(dev, itf)

data = "Test123"
print("Wrote %d bytes" % dev.write(COMMAND_EP, data, interface=INTERFACE_NUMBER, timeout=TIMEOUT))

while True:
	try:
		print("Read: %s" % dev.read(COMMAND_EP, COMMAND_IN_SIZE, interface=INTERFACE_NUMBER, timeout=TIMEOUT))
	except Exception as e:
		print("Except: %s" % e)
