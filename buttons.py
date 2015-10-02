#!/usr/bin/env python2

from pygui.mainwindow import MainWindow
import sys

if len(sys.argv)>1:
    port = int(sys.argv[1])
else:
    port = 9090


window = MainWindow('localhost',port)
window.window.geometry("350x700")
window.Run()
