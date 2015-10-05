#!/usr/bin/env python2

import optparse
import sys

from pygui.mainwindow import MainWindow

parser = optparse.OptionParser()
parser.add_option('-p', dest='port', default=9090, type='int')
(options, args) = parser.parse_args()

port = options.port
files = [f for f in args if f.endswith('.root')]

window = MainWindow('localhost',port)
for infile in files:
    window.LoadRootFile(infile)

window.Run()
