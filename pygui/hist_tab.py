#!/usr/bin/env python2

import Tkinter as tk
import ttk

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

from .util import unpack_tdirectory, update_tcanvases, TKeyDict

class HistTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)

        self.CheckOnlineHists()
        self.main.window.after_idle(self._PeriodicHistogramCheck)

    def _setup_GUI(self, frame):
        self.frame = frame
        self._MakeHistView(frame)

    def _MakeHistView(self,parent):
        self.treeview = ttk.Treeview(parent)
        self.treeview.pack(fill=tk.BOTH,expand=True)
        # Map from treeview name to ROOT object
        self.hist_lookup = TKeyDict()
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

        update_tcanvases()

    def _hist_patterns(self):
        output = []
        for obj in self.hist_lookup.values():
            if hasattr(obj, 'hist_pattern'):
                output.append(obj.hist_pattern)
        return output

    def _load_online_patterns(self, patterns):
        for pattern in patterns:
            if (ROOT.online_events and
                ROOT.online_events.GetName() == pattern['treename']):
                tree = ROOT.online_events
            elif (ROOT.online_scalers and
                ROOT.online_scalers.GetName() == pattern['treename']):
                tree = ROOT.online_scalers
            else:
                continue

            if 'Yvar' not in pattern:
                # 1-d histogram
                tree.AddHistogram(pattern['name'],
                                  pattern['Xbins'],
                                  pattern['Xlow'],
                                  pattern['Xhigh'],
                                  pattern['Xvar'],
                                  pattern['gate'],
                                  )
            else:
                # 2-d histogram
                tree.AddHistogram(pattern['name'],
                                  pattern['Xbins'],
                                  pattern['Xlow'],
                                  pattern['Xhigh'],
                                  pattern['Xvar'],
                                  pattern['Ybins'],
                                  pattern['Ylow'],
                                  pattern['Yhigh'],
                                  pattern['Yvar'],
                                  pattern['gate'],
                                  )

        # Bring all the histograms into the list
        self.CheckOnlineHists()

    def Insert(self,obj,parent='',online_tree=None):
        if (obj.InheritsFrom('TKey') and
            not ROOT.TClass(obj.GetClassName()).InheritsFrom('TH1')):
            obj = obj.ReadObj()

        if obj.InheritsFrom('TTree'):
            return

        if parent:
            name = parent + '/' + obj.GetName()
        else:
            name = obj.GetName()

        self._insert_single_nonrecursive(obj, parent, name, online_tree)

        if obj.InheritsFrom('TList'):
            iterable = obj
        elif obj.InheritsFrom('TDirectory'):
            iterable = obj.GetListOfKeys()
            if not iterable:
                iterable = obj.GetList()
        else:
            iterable = None

        if iterable is not None:
            for obj in iterable:
                self.Insert(obj, name, online_tree)

    def _setup_online_hist_pattern(self, hist, online_tree):
        hist_pattern = online_tree.GetHistPattern(hist.GetName())
        if not hist_pattern:
            return

        hist_pattern = hist_pattern.split('\n')
        pattern = {
            'treename': hist_pattern[0],
            'name'  :hist_pattern[1],
            'gate'  :hist_pattern[2],
            'Xvar'  :hist_pattern[3],
            'Xbins' :int(hist_pattern[4]),
            'Xlow'  :float(hist_pattern[5]),
            'Xhigh' :float(hist_pattern[6]),
            }

        if len(hist_pattern) > 7:
            params_2d = {
                'Yvar'  :hist_pattern[7],
                'Ybins' :int(hist_pattern[8]),
                'Ylow'  :float(hist_pattern[9]),
                'Yhigh' :float(hist_pattern[10]),
                }
            pattern.update(**params_2d)

        hist.hist_pattern = pattern

    def _insert_single_nonrecursive(self, obj, parent, name, online_tree=None):
        is_histogram = (obj.InheritsFrom('TKey') and
                        ROOT.TClass(obj.GetClassName()).InheritsFrom('TH1'))

        if (online_tree is not None and
            is_histogram):
            self._setup_online_hist_pattern(obj, online_tree)

        if (is_histogram and
            name in self.hist_lookup and
            not self.hist_lookup.is_tkey(name)):
            # If the histogram has already been read, copy it in
            orig = self.hist_lookup[name]
            obj.Copy(orig)
            orig.SetDirectory(0)
        else:
            # TKeyDict makes a Clone, so the TOnlineTree updating won't
            # require an update of the canvas.
            self.hist_lookup[name] = obj

        if name not in self.treeview.get_children(parent):
            icon = self.main._PickIcon(obj)
            self.treeview.insert(parent,'end', name, text=obj.GetName(),image=icon)

    def _PeriodicHistogramCheck(self):
        self.CheckOnlineHists()
        self.main.window.after(5000, self._PeriodicHistogramCheck)

    def CheckOnlineHists(self):
        if ROOT.online_events:

            self.Insert(ROOT.online_events.GetDirectory(),
                        online_tree=ROOT.online_events)
        if ROOT.online_scalers:
            self.Insert(ROOT.online_scalers.GetDirectory(),
                        online_tree=ROOT.online_scalers)

    def GetFormatOnline(self, hist):
        pass

    def InsertHist(self, hist):
        dirname = hist.GetDirectory().GetName()

        if dirname in self.treeview.get_children(''):
            icon = self.main._PickIcon(hist)
            name = dirname + '/' + hist.GetName()
            self._insert_single_nonrecursive(hist, dirname, name)
