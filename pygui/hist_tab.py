#!/usr/bin/env python2

import Tkinter as tk
import ttk

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
        if obj.Class().InheritsFrom('TH2'):
            icon = self.main.icons['h2_t']
        elif obj.Class().InheritsFrom('TH1'):
            icon = self.main.icons['h1_t']
        elif obj.Class().InheritsFrom('TFile'):
            icon = self.main.icons['tfile']
        elif obj.Class().InheritsFrom('TDirectory'):
            icon = self.main.icons['folder_t']
        else:
            icon = ''

        name = parent + '/' + obj.GetName()

        if (name in self.treeview.get_children(parent) and
            obj.Class().InheritsFrom('TH1')):
            orig = self.hist_lookup[name]
            obj.Copy(orig) # Copy the new object into the original
            return
        elif name in self.treeview.get_children(parent):
            self.hist_lookup[name] = obj
        else:
            self.hist_lookup[name] = obj
            self.treeview.insert(parent,'end', name, text=obj.GetName(),image=icon)

        if obj.Class().InheritsFrom('TList'):
            iterable = obj
        elif hasattr(obj,'GetListOfKeys'):
            iterable = unpack_tdirectory(obj)
        else:
            iterable = None

        if iterable is not None:
            for obj in iterable:
                self.Insert(obj, name)


def unpack_tdirectory(tdir):
    for key in tdir.GetListOfKeys():
        yield key.ReadObj()
