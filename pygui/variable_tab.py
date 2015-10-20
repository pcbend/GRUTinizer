#!/usr/bin/env python2

import Tkinter as tk
import ttk

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

class VariableTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)
        self.variables = {}

    def _setup_GUI(self, frame):
        self.frame = frame
        self._MakeButtons(frame)
        self._MakeTreeView(frame)

    def _MakeButtons(self, parent):
        # Text entries
        frame = tk.Frame(parent)
        tk.Label(frame, text='Name').grid(row=0, column=0)
        tk.Label(frame, text='Value').grid(row=0, column=1)
        self.var_name = tk.StringVar()
        tk.Entry(frame, textvariable=self.var_name).grid(row=1, column=0)
        self.var_value = tk.StringVar()
        tk.Entry(frame, textvariable=self.var_value).grid(row=1, column=1)
        frame.pack(fill=tk.X,expand=False)

        # Buttons
        frame = tk.Frame(parent)
        tk.Button(frame, text='Set/Replace',
                  command=self.OnSetReplaceVariable_Click).pack(side=tk.LEFT)
        tk.Button(frame, text='Delete',
                  command=self.OnDeleteVariable_Click).pack(side=tk.LEFT)
        frame.pack(fill=tk.X,expand=False)

    def _MakeTreeView(self, parent):
        self.treeview = ttk.Treeview(parent, columns=('value',))
        self.treeview.heading('#0', text='Name')
        self.treeview.heading('value', text='Value')
        self.treeview.pack(fill=tk.BOTH,expand=True)
        self.treeview.bind("<Double-1>",self.VariableSelection)

    def SetReplaceVariable(self, name, value):
        if ROOT.online_events:
            ROOT.online_events.SetVariable(name, value)
        self.variables[name] = value

        if name in self.treeview.get_children():
            self.treeview.item(name, values=(str(value),))
        else:
            self.treeview.insert('','end',name, text=name, values=(str(value),))

    def DeleteVariable(self, name):
        if ROOT.online_events:
            ROOT.online_events.RemoveVariable(name)

        self.variables.pop(name, None)
        self.treeview.delete(name)

    def OnSetReplaceVariable_Click(self):
        name = self.var_name.get()
        if not name:
            return

        try:
            value = float(self.var_value.get())
        except ValueError:
            print 'Could not convert "{}" to float'.format(self.var_value.get())
            return

        self.SetReplaceVariable(name, value)

    def OnDeleteVariable_Click(self):
        selection = self.treeview.selection()
        if not selection:
            return
        self.DeleteVariable(selection[0])

    def VariableSelection(self, event):
        name = event.widget.selection()[0]
        self.var_name.set(name)
        self.var_value.set(str(self.variables[name]))
