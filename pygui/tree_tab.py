#!/usr/bin/env python2

import Tkinter as tk
import ttk

from .tree_structure import Node

class TreeTab(object):

    def __init__(self, main, frame):
        self.main = main
        self._setup_GUI(frame)

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
        self._LoadTree()
        self.tree.bind("<Double-1>", self.ParameterSelection)

    def ParameterSelection(self, event):
        param = event.widget.selection()[0]
        if event.widget.get_children(param):
            return

        if not self.hist2d.get():
            self.next_param = 'x'

        if self.next_param == 'x':
            self.x_draw_varexp.set(param)
            self.next_param = 'y'
        else:
            self.y_draw_varexp.set(param)
            self.next_param = 'x'

    def _LoadTree(self):
        res = self.main.run_command('if(online_events) {'
                                    '    gResponse = online_events->GetObjectStringLeaves();'
                                    '}')
        if res is None:
            return

        res = filter(None,str(res).split('\n'))
        tree = Node('')
        for item in res:
            tree.AddChild(item)
        tree.ttk_treeview(self.tree)

    def AddHistogram(self):
        commands = {1:
                    'if(online_events) {{'
                    '    online_events->AddHistogram("{name}",'
                    '                                {Xbins},{Xlow},{Xhigh},"{Xexp}");'
                    '}}',
                    2:
                    'if(online_events) {{'
                    '    online_events->AddHistogram("{name}",'
                    '                                {Xbins},{Xlow},{Xhigh},"{Xexp}",'
                    '                                {Ybins},{Ylow},{Yhigh},"{Yexp}");'
                    '}}',
        }

        dimension = 2 if self.hist2d.get() else 1
        command = commands[dimension]

        args = {'name'  : self.hist_name.get(),
                'Xbins' : self.x_draw_bins.get(),
                'Xlow'  : self.x_draw_low.get(),
                'Xhigh' : self.x_draw_high.get(),
                'Xexp'  : self.x_draw_varexp.get(),
                'Ybins' : self.y_draw_bins.get(),
                'Ylow'  : self.y_draw_low.get(),
                'Yhigh' : self.y_draw_high.get(),
                'Yexp'  : self.y_draw_varexp.get(),
            }
        command = command.format(**args)
        self.main.run_command(command)
        self.main.RefreshHistograms()
