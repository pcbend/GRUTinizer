#!/usr/bin/env python2

import Tkinter as tk
import ttk

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

from .util import unpack_tdirectory

class HistTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)

    def _setup_GUI(self, frame):
        self.frame = frame
        self._MakeHistView(frame)

    def _MakeHistView(self,parent):
        self.treeview = ttk.Treeview(parent)
        self.treeview.pack(fill=tk.BOTH,expand=True)
        # Map from treeview name to ROOT object
        self.hist_lookup = {}
        self.treeview.bind("<Double-1>", self.OnHistClick)

    def OnHistClick(self,event):
        objects = [self.hist_lookup[i] for i in event.widget.selection()]
        histograms = [h for h in objects if h.InheritsFrom('TH1')]

        color = 1;
        for obj in histograms:
            self.main._draw_single(obj,color,len(histograms))
            if self.main.plotlocation.get()=='Overlay':
                color+=1
                if color == 5:
                    color+=1

    def Insert(self,obj,parent=''):
        if obj.InheritsFrom('TTree'):
            return

        if parent:
            name = parent + '/' + obj.GetName()
        else:
            name = obj.GetName()

        self._insert_single_nonrecursive(obj, parent, name)

        if obj.InheritsFrom('TList'):
            iterable = obj
        elif obj.InheritsFrom('TDirectory'):
            iterable = unpack_tdirectory(obj)
        else:
            iterable = None

        if iterable is not None:
            for obj in iterable:
                self.Insert(obj, name)

    def _insert_single_nonrecursive(self, obj, parent, name):
        if (name in self.treeview.get_children(parent) and
            obj.InheritsFrom('TH1')):
            orig = self.hist_lookup[name]
            obj.Copy(orig) # Copy the new object into the original
            return
        elif name in self.treeview.get_children(parent):
            self.hist_lookup[name] = obj
        else:
            icon = self.main._PickIcon(obj)
            self.hist_lookup[name] = obj
            self.treeview.insert(parent,'end', name, text=obj.GetName(),image=icon)

    def CheckOnlineHists(self):
        if ROOT.online_events:
            self.Insert(ROOT.online_events.GetDirectory())
        if ROOT.online_scalers:
            self.Insert(ROOT.online_scalers.GetDirectory())

    def InsertHist(self, hist):
        dirname = hist.GetDirectory().GetName()

        if dirname in self.treeview.get_children(''):
            icon = self.main._PickIcon(hist)
            name = dirname + '/' + hist.GetName()
            self._insert_single_nonrecursive(hist, dirname, name)
