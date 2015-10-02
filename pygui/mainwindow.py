#!/usr/bin/env python2

import Tkinter as tk
import ttk
from pygui.run_command import run_command

from tree_structure import Node
from ROOT import *

class MainWindow(object):

    def __init__(self,host,port):
        self.host = host
        self.port = port

        gROOT.ProcessLine("gSystem->Load(\"libGpad.so\")")
        gROOT.ProcessLine("gSystem->Load(\"/home/users/gretina/packages/GRUTinizer/libraries/libGROOT.so\")")

        self.window = tk.Tk()
        self.refreshrate  = tk.IntVar()
        self.refreshrate.set(-1)
        self.plotlocation = tk.IntVar()
        self.plotlocation.set(1)

        self._setup_GUI()


    def hello(self):
        print "hello!"

    def open_canvas(self):
        gROOT.ProcessLine("new GCanvas")

    def set_refresh(self):
        print("refresh = " + str(self.refreshrate.get()))

    def set_plotlocation(self):
        print("plot location = " + str(self.plotlocation.get()))

    def _LoadMenuBar(self):
        self.menubar  = tk.Menu(self.window)

        self.filemenu = tk.Menu(self.menubar,tearoff=0)
        self.filemenu.add_command(label="New Canvas",command=self.open_canvas)
        self.filemenu.add_separator()
        self.filemenu.add_command(label="Open",command=self.hello)
        self.filemenu.add_command(label="Save",command=self.hello)
        self.filemenu.add_separator()
        self.filemenu.add_command(label="Exit",command=self.window.quit)
        self.menubar.add_cascade(label="File",menu=self.filemenu)

        self.refreshmenu = tk.Menu(self.menubar,tearoff=0)
        self.refreshmenu.add_checkbutton(label="Off",onvalue=-1,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.refreshmenu.add_command(label="Now!",command=self.hello)
        self.refreshmenu.add_separator()
        self.refreshmenu.add_checkbutton(label="1 second",onvalue=1,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.refreshmenu.add_checkbutton(label="2 seconds",onvalue=2,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.refreshmenu.add_checkbutton(label="5 seconds",onvalue=5,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.refreshmenu.add_checkbutton(label="10 seconds",onvalue=10,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.refreshmenu.add_checkbutton(label="30 seconds",onvalue=30,
                                         variable=self.refreshrate,command=self.set_refresh)
        self.menubar.add_cascade(label="Refresh",menu=self.refreshmenu)
        
        self.zonesmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="Zones",menu=self.zonesmenu)
        
        self.optstatmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="OptStats",menu=self.optstatmenu)

        self.plotmenu = tk.Menu(self.menubar,tearoff=0)
        self.plotmenu.add_checkbutton(label="New Canvas",onvalue=1,
                                      variable=self.plotlocation,command=self.set_plotlocation)
        self.plotmenu.add_checkbutton(label="Next Pad",onvalue=2,
                                      variable=self.plotlocation,command=self.set_plotlocation)
        self.plotmenu.add_checkbutton(label="Current Pad (replace)",onvalue=3,
                                      variable=self.plotlocation,command=self.set_plotlocation)
        self.plotmenu.add_checkbutton(label="Current Pad (overlay)",onvalue=4,
                                      variable=self.plotlocation,command=self.set_plotlocation)
        self.menubar.add_cascade(label="Plot",menu=self.plotmenu)

        self.helpmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="Send Help",menu=self.helpmenu)

    def run_command(self, command):
        return run_command(command, self.host, self.port)

    def LoadFile(self):
        self.run_command('TGRUTint::instance()->OpenFileDialog()')

    def _LoadTree(self):
        res = self.run_command('if(online_events) {'
                               '    gResponse = online_events->GetObjectStringLeaves();'
                               '}')
        res = filter(None,str(res).split('\n'))
        tree = Node('')
        for item in res:
            tree.AddChild(item)
        tree.ttk_treeview(self.tree)

    def ParameterSelection(self, event):
        print 'You clicked "{}"'.format(event.widget.selection()[0])
        #import ptpython.repl; ptpython.repl.embed(globals(), locals())

    def _setup_GUI(self):
        self.window.wm_title("hist-o-matic")
        self._LoadMenuBar()
        self.window.config(menu=self.menubar)
        #title = tk.Label(self.window, text='Hitsomatic',
        #                 fg='black', bg='blue',
        #                 font='Helvetica 24 bold')
        #title.pack(fill=tk.X,expand=False)


        button = tk.Button(self.window,
                           text='Load File',fg="black",bg="goldenrod",
                           command=self.LoadFile)
        button.pack(fill=tk.X,expand=False)

        # button = tk.Button(self.window,
        #                    text='',fg="black",bg="goldenrod",
        #                    command=self.LoadTree)
        # button.pack(fill=tk.X,expand=False)

        self.tree = ttk.Treeview(self.window)
        self.tree.pack(fill=tk.BOTH,expand=True)
        self._LoadTree()
        self.tree.bind("<Double-1>", self.ParameterSelection)


    def Run(self):
        self.window.mainloop()
