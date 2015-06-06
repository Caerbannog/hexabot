#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Dump isochronous frames"""

import hexabot


if __name__ == "__main__":
    dev = hexabot.controller([hexabot.ISOCHRONOUS_INTERFACE])
    
    dev.dump_isochronous()
