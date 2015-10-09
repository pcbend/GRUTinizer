#!/usr/bin/env grut-python-exec

import os
import sys
import warnings
import threading
import Tkinter as tk

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
warnings.filterwarnings(action='ignore',category=RuntimeWarning,
                        message='creating converter.*')
sys.argv = sys.argv[1:] # ROOT appends "python" to the beginning of sys.argv
sys.argv.append('grut-view.py')

sys.path.append(os.environ['GRUTSYS'])
from pygui.mainwindow import MainWindow

ident = threading.current_thread().ident

def update():
    try:
        if threading.current_thread().ident == ident:
            window.Update()
    except tk.TclError:
        pass
    except Exception as e:
        print e

window = MainWindow()
#window.Run()
