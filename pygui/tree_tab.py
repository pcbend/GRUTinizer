#!/usr/bin/env python2

import Tkinter as tk
import ttk

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

from .tree_structure import Node
from .util import increment_name, unpack_tdirectory

class TreeTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)
        self.object_lookup = {}
        self.param_lookup = {}
        self.active_ttree = None

        self.AddOnlineTrees()

    def _setup_GUI(self, frame):
        self.frame = frame
        self._MakeHistogramControls(frame)
        self._MakeTreeView(frame)

    def _MakeHistogramControls(self,parent):
        self.next_param = 'x'

        frame = tk.Frame(parent)

        frame.columnconfigure(1,weight=1)

        tk.Label(frame,text='Parameter').grid(row=0,column=1)
        tk.Label(frame,text='Bins').grid(row=0,column=2)
        tk.Label(frame,text='Low').grid(row=0,column=3)
        tk.Label(frame,text='High').grid(row=0,column=4)

        enable_x_label = tk.Label(frame,text='X',
                                   anchor=tk.E)
        enable_x_label.grid(row=1,column=0)

        self.x_draw_varexp = tk.StringVar()
        x_varexp = tk.Entry(frame,textvariable=self.x_draw_varexp)
        x_varexp.grid(row=1,column=1,sticky='ew')

        self.x_draw_bins = tk.StringVar(value='4096')
        x_bins = tk.Entry(frame,textvariable=self.x_draw_bins,
                          width=5,text='4096')
        x_bins.grid(row=1,column=2)

        self.x_draw_low = tk.StringVar(value='0')
        x_low = tk.Entry(frame,textvariable=self.x_draw_low,
                         width=5)
        x_low.grid(row=1,column=3)

        self.x_draw_high = tk.StringVar(value='4096')
        x_high = tk.Entry(frame,textvariable=self.x_draw_high,
                          width=5)
        x_high.grid(row=1,column=4)



        y_entry_boxes = []
        def enable_y_parameters():
            state = 'normal' if self.hist2d.get() else 'disabled'
            for entry in y_entry_boxes:
                entry.configure(state=state)

        self.hist2d = tk.IntVar()
        enable_y_box = tk.Checkbutton(frame, text='Y',
                                      variable=self.hist2d,
                                      command=enable_y_parameters)
        enable_y_box.grid(row=2,column=0)


        self.y_draw_varexp = tk.StringVar()
        y_varexp = tk.Entry(frame,textvariable=self.y_draw_varexp)
        y_varexp.grid(row=2,column=1,sticky='ew')
        y_entry_boxes.append(y_varexp)

        self.y_draw_bins = tk.StringVar(value='4096')
        y_bins = tk.Entry(frame,textvariable=self.y_draw_bins,
                          width=5)
        y_bins.grid(row=2,column=2)
        y_entry_boxes.append(y_bins)

        self.y_draw_low = tk.StringVar(value='0')
        y_low = tk.Entry(frame,textvariable=self.y_draw_low,
                         width=5)
        y_low.grid(row=2,column=3)
        y_entry_boxes.append(y_low)

        self.y_draw_high = tk.StringVar(value='4096')
        y_high = tk.Entry(frame,textvariable=self.y_draw_high,
                          width=5)
        y_high.grid(row=2,column=4)
        y_entry_boxes.append(y_high)

        enable_y_parameters()

        frame.pack(fill=tk.X,expand=False)


        # Gate frame
        frame = tk.Frame(parent)
        def enable_gate_box():
            state = 'normal' if self.apply_gate.get() else 'disabled'
            gate_box.configure(state=state)

        self.apply_gate = tk.IntVar(value=0)
        apply_gate = tk.Checkbutton(frame, text='Apply Gate',
                                    variable=self.apply_gate,
                                    command=enable_gate_box)
        apply_gate.pack(side=tk.LEFT)

        self.gate = tk.StringVar(value='')
        gate_box = tk.Entry(frame, textvariable=self.gate)
        gate_box.pack(side=tk.LEFT)
        enable_gate_box()

        frame.pack(fill=tk.X,expand=False)

        # Button frame
        frame = tk.Frame(parent)
        self.hist_name = tk.StringVar(value='hist')
        tk.Label(frame,text='Name:').pack(side=tk.LEFT)
        tk.Entry(frame,textvariable=self.hist_name).pack(side=tk.LEFT)
        tk.Button(frame,text='Add Histogram',
                  command=self.AddHistogram).pack(side=tk.LEFT)
        frame.pack(fill=tk.X,expand=False)

    def _MakeTreeView(self,parent):
        self.tree = ttk.Treeview(parent)
        self.tree.pack(fill=tk.BOTH,expand=True)
        self.tree.bind("<Double-1>", self.OnDoubleClick)

    def OnDoubleClick(self, event):
        selection = event.widget.selection()[0]
        obj = self.object_lookup.get(selection)

        if obj is not None and obj.InheritsFrom('TTree'):
            self.active_ttree = obj
        else:
            self.ParameterSelection(selection)

    def ParameterSelection(self, selection):
        param = self.param_lookup.get(selection)
        if param is None:
            return

        if not self.hist2d.get():
            self.next_param = 'x'

        if self.next_param == 'x':
            self.x_draw_varexp.set(param)
            self.next_param = 'y'
        else:
            self.y_draw_varexp.set(param)
            self.next_param = 'x'

    def AddOnlineTrees(self):
        if ROOT.online_events or ROOT.online_scalers:
            self.tree.insert('', 'end', 'online', text='online')

        if ROOT.online_events:
            self.AddTree('online',ROOT.online_events)
        if ROOT.online_scalers:
            self.AddTree('online',ROOT.online_scalers)

    def AddTree(self, filename, ttree):
        res = ROOT.TOnlineTree.GetObjectStringLeaves(ttree)
        res = filter(None,str(res).split('\n'))

        name = filename + '/' + ttree.GetName()
        tree = Node(name)
        for item in res:
            tree.AddChild(item)

        self.object_lookup[name] = ttree
        self.tree.insert(filename, 'end', name, text=ttree.GetName(),
                         image = self.main._PickIcon(ttree))
        tree.ttk_treeview(self.tree, name, lookup_table=self.param_lookup)

    def AddFile(self, tfile):
        print 'Adding trees from ',tfile.GetName()
        name = tfile.GetName()
        self.object_lookup[name] = tfile
        self.tree.insert('', 'end', name, text=name,
                         image = self.main._PickIcon(tfile))

        objects = {obj.GetName():obj for obj in tfile.GetListOfKeys()
                   if ROOT.TClass(obj.GetClassName()).InheritsFrom('TTree')}

        for obj in objects.values():
            self.AddTree(tfile.GetName(), obj.ReadObj())

    def AddOfflineHistogram(self, ttree):
        dimension = 2 if self.hist2d.get() else 1

        name  = self.hist_name.get()
        Xvar  = self.x_draw_varexp.get()
        Xbins = int(self.x_draw_bins.get())
        Xlow  = float(self.x_draw_low.get())
        Xhigh = float(self.x_draw_high.get())
        Yvar  = self.y_draw_varexp.get()
        Ybins = int(self.y_draw_bins.get())
        Ylow  = float(self.y_draw_low.get())
        Yhigh = float(self.y_draw_high.get())

        if dimension==1:
            new_hist = ROOT.TH1I(name, name,
                                 Xbins, Xlow, Xhigh)
            draw_command = Xvar
        else:
            new_hist = ROOT.TH1I(name, name,
                                 Xbins, Xlow, Xhigh,
                                 Ybins, Ylow, Yhigh)
            draw_command = Yvar + ':' + Xvar

        ttree.Project(name, draw_command)
        new_hist.SetDirectory(ttree.GetDirectory())
        self.main.hist_tab.InsertHist(new_hist)

    def AddOnlineHistogram(self):
        if not ROOT.online_events:
            return

        # TODO: Modify TOnlineTree so this replaces the histogram instead.
        if self.active_ttree.HasHistogram(self.hist_name.get()):
            print 'Histogram "{}" already exists.  Choose a different name'.format(
                self.hist_name.get())
            return

        dimension = 2 if self.hist2d.get() else 1

        if dimension==1:
            self.active_ttree.AddHistogram(self.hist_name.get(),
                                           int(self.x_draw_bins.get()),
                                           float(self.x_draw_low.get()),
                                           float(self.x_draw_high.get()),
                                           self.x_draw_varexp.get())
        else:
            self.active_ttree.AddHistogram(self.hist_name.get(),
                                           int(self.x_draw_bins.get()),
                                           float(self.x_draw_low.get()),
                                           float(self.x_draw_high.get()),
                                           self.x_draw_varexp.get(),
                                           int(self.y_draw_bins.get()),
                                           float(self.y_draw_low.get()),
                                           float(self.y_draw_high.get()),
                                           self.y_draw_varexp.get())

        self.main.RefreshHistograms()
        self.main.hist_tab.CheckOnlineHists()

    def _increment_name(self):
        name = self.hist_name.get()
        self.hist_name.set(increment_name(name))

    def AddHistogram(self):
        if self.active_ttree.InheritsFrom('TOnlineTree'):
            self.AddOnlineHistogram()
        else:
            if self.active_ttree is not None:
                self.AddOfflineHistogram(self.active_ttree)

        self._increment_name()
