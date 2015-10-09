#!/usr/bin/env grut-python-exec

import os
import sys
import warnings
import threading

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
warnings.filterwarnings(action='ignore',category=RuntimeWarning,
                        message='creating converter.*')
sys.argv = sys.argv[1:] # ROOT appends "python" to the beginning of sys.argv
sys.argv.append('grut-view.py')

sys.path.append(os.environ['GRUTSYS'])
from pygui.mainwindow import MainWindow

window = MainWindow()
#window.Run()

ident = threading.current_thread().ident
