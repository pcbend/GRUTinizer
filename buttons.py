#!/usr/bin/env python2

from pygui.mainwindow import MainWindow
import sys

#if len(sys.argv)>1:
#    port = int(sys.argv[1])
#else:
#port = 9090


suffix = ".root"
prefix = "-p"
files  = []
port   = 9090
for x in range(1,len(sys.argv)):
    if sys.argv[x].endswith(suffix):
        files.append(sys.argv[x])
    elif sys.argv[x].startswith(prefix):
        if(len(sys.argv[x])>2):
            port = int(sys.argv[x][2:])
        else:
            x+=1
            port = int(sys.argv[x])
        print "port = " + str(port)

#print "files len = " + str(len(files))
#print "files = ", files
window = MainWindow('localhost',port)
for infile in files:
    window.LoadRootFile(infile)


window.Run()
