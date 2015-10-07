#!/usr/bin/env python2

import Tkinter as tk
import ttk
import re

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

class TCutTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)

        self.cuts = {}

        frame.after_idle(self._repeatedly_check)

    def _setup_GUI(self, frame):
        self.frame = frame
        self._MakeNaming(frame)
        self._MakeTreeView(frame)

    def _repeatedly_check(self):
        self._check_for_tcut()
        self.frame.after(1000,self._repeatedly_check)

    def _check_for_tcut(self):
        # Does CUTG exist?
        cutg = ROOT.gROOT.GetListOfSpecials().FindObject('CUTG')
        if not cutg:
            return

        # Does it have 3 or more points?
        npoints = cutg.GetN()
        if npoints < 3:
            return

        # Is the first point equal to the last point?
        xi = cutg.GetX()[0]
        yi = cutg.GetY()[0]
        xf = cutg.GetX()[npoints-1]
        yf = cutg.GetY()[npoints-1]
        if xi!=xf or yi!=yf:
            return

        # Cut is finished, so grab it.
        ROOT.gROOT.GetListOfSpecials().Remove(cutg)
        name = self.next_name.get()
        cutg.SetName(name)
        self._increment_name()

        self.AddCut(cutg)

    def AddCut(self, cut):
        name = cut.GetName()
        self.cuts[name] = cut
        self.tree.insert('', 'end', name, text=name,
                         image = self.main.icons['tcutg'])

    def StartCut(self):
        ROOT.gROOT.SetEditorMode('CutG')

    def _increment_name(self):
        name = self.next_name.get()
        res = re.search('[0-9]+$', name)
        if res:
            prefix = name[:-len(res.group())]
            number = int(res.group()) + 1
            self.next_name.set(prefix + str(number))
        else:
            self.next_name.set(name + '_1')

    def _MakeNaming(self, parent):
        self.next_name = tk.StringVar(value='tcutg_0')
        frame = tk.Frame(parent)
        tk.Label(frame, text='Name:').pack(side=tk.LEFT)
        tk.Entry(frame, textvariable=self.next_name).pack(side=tk.LEFT)
        tk.Button(frame, text='Make Gate', command=self.StartCut).pack(side=tk.LEFT)
        frame.pack(fill=tk.X,expand=False)

    def _MakeTreeView(self, parent):
        self.tree = ttk.Treeview(parent)
        self.tree.pack(fill=tk.BOTH,expand=True)
        self.tree.bind("<Double-1>",self.GateSelection)

    def GateSelection(self, event):
        print 'Gate was double-clicked'
