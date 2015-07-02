#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""Read real time metrics over a bulk endpoint"""

import hexabot


if __name__ == "__main__":
    dev = hexabot.controller([hexabot.METRICS_INTERFACE])

    dev.dump_metrics()
