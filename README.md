Hexabot Sources
===============

Overview
--------
Design files and source code of a mobile robot for the [Eurobot competition](http://www.eurobot.org/).

The controller board is built in KiCad around a [dsPIC33EP256MU806](https://www.microchip.com/products/dsPIC33EP256MU806) (QFN, motor control, USB).
You can control it from an embedded Linux like the Raspberry Pi.

Python scripts use [PyUSB](https://walac.github.io/pyusb/) to communicate with the various interfaces (commands, odometry, metrics, debug logs).

Setup
-----
Download and install the [MPLABX IDE](http://www.microchip.com/mplabx/) and the [XC16 compiler](http://www.microchip.com/compilers/).

Install Python 3 and debugging tools like Wireshark.
For Ubuntu 15.04 you can run:
```bash
git clone https://github.com/Caerbannog/hexabot.git
sudo apt-get install python3-pip wireshark
sudo pip3 install pyusb==1.0.0b1
sudo adduser $USER dialout
sudo adduser $USER wireshark
echo usbmon | sudo tee -a /etc/modules
sudo cp hexabot.rules /etc/udev/rules.d/
sudo pip install miniterm.py
```

Show the debug console:
`miniterm.py /dev/ttyACM* 9600 -D`

Documentation
-------------
TODO:

- board functions
- code architecture
- wire protocol
- high level IA

Contributing
------------
Pull requests are welcome on the project page:
<https://github.com/Caerbannog/hexabot>

Licenses
--------
- Hardware design files are published under the CERN OHL v.1.2.
- Source code is published under the GPLv3 or later.
- Note that Microchip's USB library and the CDC-basic demo have a more restrictive license.
