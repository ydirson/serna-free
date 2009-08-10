#!/usr/bin/python

import sys, os

if len(sys.argv) > 1:
    sys.path.append(sys.argv[1])

import sipconfig

cfg = sipconfig.Configuration()

print cfg.default_sip_dir
