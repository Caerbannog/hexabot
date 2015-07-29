Hexabot Sources
===============

Overview
--------
Design files and source code of a mobile robot for the [Eurobot competition](http://www.eurobot.org/).

The controller board is built in [KiCad](http://www.kicad-pcb.org/)
around a [dsPIC33EP256MU806](https://www.microchip.com/products/dsPIC33EP256MU806) (QFN, motor control, USB).
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
sudo pip3 install pyusb==1.0.0b1 autobahn==0.10.4 pyinotify==0.9.6
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
### Hexabot Controller ###
The board handles various functions: motor control, odometry, sensors and servomotors.
It has two layers, 10cm*10cm and mostly 0806 SMT components.

You can buy 10 PCBs for 32$ at [SeeedStudio](http://www.seeedstudio.com/service/index.php?r=pcb).
Then we hand-soldered ours.

TODO:

- board functions

![Layout v1](kicad/v1-layout.png)

### dsPIC Software ###
Refer to the source code in [hexabot.X](hexabot.X/).

TODO:

- code architecture
- role of endpoints
- wire protocols

### Linux Software ###
Refer to the source code in [linux](linux/).

TODO

- high level AI

Contributing
------------
Pull requests are welcome on the project page:
<https://github.com/Caerbannog/hexabot>

Licenses
--------
- Hardware design files are published under the CERN OHL v.1.2.
- Source code is published under the GPLv3 or later.
- Note that Microchip's USB library and the CDC-basic demo have a more restrictive license.
