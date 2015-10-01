#!/usr/bin/env python2

import Tkinter as tk
import ttk
from pygui.run_command import run_command

from tree_structure import Node

class MainWindow(object):

    def __init__(self,host,port):
        self.host = host
        self.port = port
        self._setup_GUI()

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
        self.window = tk.Tk()
        self.window.wm_title("grut command")
        title = tk.Label(self.window, text='Command Window',
                         fg='black', bg='blue',
                         font='Helvetica 24 bold')
        title.pack(fill=tk.X,expand=False)


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
