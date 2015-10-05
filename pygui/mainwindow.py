#!/usr/bin/env python2

import os
import Tkinter as tk
from tkFileDialog import askopenfilename
import ttk

import ROOT
import math

from pygui.run_command import run_command
from tree_structure import Node


#Fix ROOT TCanvases, which don't redraw when they should
def fix_tcanvases():
    for canvas in ROOT.gROOT.GetListOfCanvases():
        canvas.Update()

def is_int(string):
    try:
        int(string)
        return True
    except ValueError:
        return False

def unpack_tdirectory(tdir):
    for key in tdir.GetListOfKeys():
        yield key.ReadObj()

class MainWindow(object):

    def __init__(self,host,port):
        self.host = host
        self.port = port

        self.load_library('libGui.so',False)
        self.load_library('libGROOT.so')
        ROOT.GRootGuiFactory.Init()

        self.window = tk.Tk()
        self._load_icons()

        self.refreshrate  = tk.IntVar(value='-1')
        self.plotlocation = tk.StringVar(value='NewCanvas')

        self.predefinedzones = tk.StringVar(value='1x1')
        self.zone_rows = 1
        self.zone_cols = 1

        self.optstat_name      = tk.BooleanVar(value=True)
        self.optstat_entries   = tk.BooleanVar(value=True)
        self.optstat_mean      = tk.BooleanVar(value=True)
        self.optstat_mean_err  = tk.BooleanVar(value=False)
        self.optstat_rms       = tk.BooleanVar(value=False)
        self.optstat_rms_err   = tk.BooleanVar(value=False)
        self.optstat_underflow = tk.BooleanVar(value=False)
        self.optstat_overflow  = tk.BooleanVar(value=False)
        self.optstat_integral  = tk.BooleanVar(value=True)
        self.optstat_skew      = tk.BooleanVar(value=False)
        self.optstat_skew_err  = tk.BooleanVar(value=False)
        self.optstat_kurt      = tk.BooleanVar(value=False)
        self.optstat_kurt_err  = tk.BooleanVar(value=False)

        self._load_default_style()

        self.canvases = []
        self.files = {}

        self._setup_GUI()

    def _load_icons(self):
        self.icons = {}
        self.icons['h1_t'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','h1_t.gif'))
        self.icons['h2_t'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','h2_t.gif'))
        self.icons['folder_t'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','folder_t.gif'))
        self.icons['tfile'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','rootdb_t.gif'))
        img = tk.PhotoImage(file=os.path.join(os.path.dirname(__file__),'resources','hdb_s.gif'))
        self.window.tk.call('wm','iconphoto',self.window._w,img)

    def _load_default_style(self):
        style = ROOT.TStyle("GRUTStyle","")
        style.SetOptStat(1001111)
        style.SetPalette(1)
        style.SetTitleColor(ROOT.kBlue)
        style.SetStatTextColor(ROOT.kBlue)
        style.SetFuncColor(ROOT.kRed)
        style.SetTitleBorderSize(0)
        style.SetOptFit(1111)
        style.SetPadBorderSize(1)
        style.SetPadBorderMode(1)
        ROOT.gROOT.SetStyle("GRUTStyle")
        ROOT.gROOT.ForceStyle()


    def _setup_GUI(self):
        self.window.geometry('350x700')
        self.window.wm_title("hist-o-matic")
        self._LoadMenuBar()
        self.window.config(menu=self.menubar)

        button = tk.Button(self.window,
                           text='Load Remote File',fg="black",bg="goldenrod",
                           command=self.LoadRemoteFile)
        button.pack(fill=tk.X,expand=False)

        button = tk.Button(self.window,
                           text='Load Local File',fg="black",bg="goldenrod",
                           command=self.LoadLocalFile)
        button.pack(fill=tk.X,expand=False)

        button = tk.Button(self.window,
                           text='Jump into Interpreter',fg="black",bg="goldenrod",
                           command=self.Interpreter)
        button.pack(fill=tk.X,expand=False)

        notebook = ttk.Notebook(self.window)

        tree_page = ttk.Frame(notebook)
        self._MakeHistogramControls(tree_page)
        self._MakeTreeView(tree_page)
        notebook.add(tree_page, text='Tree Viewer')

        hist_page = ttk.Frame(notebook)
        self._MakeHistView(hist_page)
        notebook.add(hist_page, text='Histograms')

        notebook.pack(fill=tk.BOTH,expand=True)

    def _SetOptStat(self):
        stat = ''
        if self.optstat_name.get():
            stat += 'n'
        if self.optstat_entries.get():
            stat += 'e'
        if self.optstat_mean.get():
            stat += 'm'
            if self.optstat_mean_err.get():
                stat += 'M'
        if self.optstat_rms.get():
            stat += 'r'
            if self.optstat_rms_err.get():
                stat += 'R'
        if self.optstat_underflow.get():
            stat += 'u'
        if self.optstat_overflow.get():
            stat += 'o'
        if self.optstat_integral.get():
            stat += 'i'
        if self.optstat_skew.get():
            stat += 's'
            if self.optstat_skew_err.get():
                stat += 'S'
        if self.optstat_kurt.get():
            stat += 'k'
            if self.optstat_kurt_err.get():
                stat += 'K'
        ROOT.gStyle.SetOptStat(stat)

    def _LoadMenuBar(self):
        self.menubar  = tk.Menu(self.window)

        self._MakeFileMenu()
        self._MakeRefreshMenu()
        self._MakeZoneMenu()
        self._MakeOptStatMenu()
        self._MakePlotMenu()
        self._MakeHelpMenu()

    def _MakeFileMenu(self):
        filemenu = tk.Menu(self.menubar,tearoff=0)
        filemenu.add_command(label="New Canvas",command=self.open_canvas)
        filemenu.add_separator()
        filemenu.add_command(label="Close All Canvases",command=self.close_all_canvases)
        filemenu.add_separator()
        filemenu.add_command(label="Open",command=self.hello)
        filemenu.add_command(label="Save",command=self.hello)
        filemenu.add_separator()
        filemenu.add_command(label="Exit",command=self.window.quit)
        self.menubar.add_cascade(label="File",menu=filemenu)

    def _MakeRefreshMenu(self):
        refreshmenu = tk.Menu(self.menubar,tearoff=0)
        refreshmenu.add_checkbutton(label="Off",onvalue=-1,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_command(label="Now!",command=self.RefreshHistograms)
        refreshmenu.add_separator()
        refreshmenu.add_checkbutton(label="1 second",onvalue=1,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_checkbutton(label="2 seconds",onvalue=2,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_checkbutton(label="5 seconds",onvalue=5,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_checkbutton(label="10 seconds",onvalue=10,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_checkbutton(label="30 seconds",onvalue=30,
                                    variable=self.refreshrate,command=self.set_refresh)
        self.menubar.add_cascade(label="Refresh",menu=refreshmenu)

    def _MakeZoneMenu(self):
        zonesmenu = tk.Menu(self.menubar,tearoff=0)
        zonesmenu.add_checkbutton(label="1 x 1",onvalue='1x1',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="1 x 2",onvalue='1x2',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="1 x 3",onvalue='1x3',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="2 x 1",onvalue='2x1',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="2 x 2",onvalue='2x2',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="3 x 3",onvalue='3x3',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="4 x 4",onvalue='4x4',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="12 x 12",onvalue='12x12',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="13 x 13",onvalue='13x13',
                                  variable=self.predefinedzones,command=self.set_zones)
        self.menubar.add_cascade(label="Zones",menu=zonesmenu)

    def set_zones(self,zones=""):
        if not zones:
            zones = self.predefinedzones.get()
        zones = zones.lower()
        zones.replace(" ","")
        zones = zones.split("x")
        self.zone_cols = int(zones[0])
        self.zone_rows = int(zones[1])
        print("zones set to " + str(self.zone_cols) + " x " + str(self.zone_rows))

    def _MakeOptStatMenu(self):
        optstatmenu = tk.Menu(self.menubar,tearoff=0)
        optstatmenu.add_checkbutton(label="Name",onvalue=1,variable=self.optstat_name)
        optstatmenu.add_checkbutton(label="Entries",onvalue=1,variable=self.optstat_entries)
        optstatmenu.add_checkbutton(label="Mean",onvalue=1,variable=self.optstat_mean)
        optstatmenu.add_checkbutton(label="Mean Error",onvalue=1,variable=self.optstat_mean_err)
        optstatmenu.add_checkbutton(label="RMS",onvalue=1,variable=self.optstat_rms)
        optstatmenu.add_checkbutton(label="RMS Error",onvalue=1,variable=self.optstat_rms_err)
        optstatmenu.add_checkbutton(label="Underflows",onvalue=1,variable=self.optstat_underflow)
        optstatmenu.add_checkbutton(label="Overflows",onvalue=1,variable=self.optstat_overflow)
        optstatmenu.add_checkbutton(label="Integral",onvalue=1,variable=self.optstat_integral)
        optstatmenu.add_checkbutton(label="Skew",onvalue=1,variable=self.optstat_skew)
        optstatmenu.add_checkbutton(label="Skew Error",onvalue=1,variable=self.optstat_skew_err)
        optstatmenu.add_checkbutton(label="Kurtosis",onvalue=1,variable=self.optstat_kurt)
        optstatmenu.add_checkbutton(label="Kurtosis Error",onvalue=1,variable=self.optstat_kurt_err)
        self.menubar.add_cascade(label="OptStats",menu=optstatmenu)

    def _MakePlotMenu(self):
        plotmenu = tk.Menu(self.menubar,tearoff=0)
        plotmenu.add_checkbutton(label="New Canvas",onvalue='NewCanvas',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Next Pad",onvalue='NextPad',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (replace)",onvalue='Replace',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (overlay)",onvalue='Overlay',
                                 variable=self.plotlocation)
        self.menubar.add_cascade(label="Plot",menu=plotmenu)

    def _MakeHelpMenu(self):
        helpmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="Send Help",menu=helpmenu)

    def _MakeTreeView(self,parent):
        self.tree = ttk.Treeview(parent)
        self.tree.pack(fill=tk.BOTH,expand=True)
        self._LoadTree()
        self.tree.bind("<Double-1>", self.ParameterSelection)

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
        self.run_command(command)
        self.RefreshHistograms()

    def RefreshHistograms(self):
        online_hists = self.run_command('if(online_events) {'
                                        '   gResponse = online_events->GetHistograms();'
                                        '}')
        if online_hists is not None:
            online_hists.SetName('online_hists')
            self._insert_to_hist_tree(online_hists)

    def _MakeHistView(self,parent):
        self.hists = ttk.Treeview(parent)
        self.hists.pack(fill=tk.BOTH,expand=True)
        # Map from index to ROOT object
        # Indices increase sequentially
        self.hist_indices = {}
        self.hists.bind("<Double-1>", self.OnHistClick)

    def OnHistClick(self,event):
        hist_indices = [int(s) for s in event.widget.selection()
                        if is_int(s)]
        objects = [self.hist_indices[i] for i in hist_indices
                   if i in self.hist_indices]
        histograms = [h for h in objects if h.InheritsFrom('TH1')]

        color = 1;
        for obj in histograms:
            self._draw_single(obj,color,len(hist_indices))
            if self.plotlocation.get()=='Overlay':
                color+=1
                if color == 5:
                    color+=1


    def _draw_single(self,hist,color=1,nselected=1):
        canvas_exists = bool(filter(None,self.canvases))
        if (not canvas_exists or
            self.plotlocation.get()=='NewCanvas' or
            not ROOT.gPad):
            self.open_canvas(columns = self.zone_cols, rows = self.zone_rows)

        currentnumber = ROOT.gPad.GetNumber()
        if currentnumber>0:
            ROOT.gPad.GetCanvas().cd(currentnumber+1)
            if ROOT.gPad.GetNumber() == currentnumber:
                ROOT.gPad.GetCanvas().cd(1)

        opt = []
        if self.plotlocation.get() == 'Overlay':
            opt.append('same')

        if hist.GetDimension() > 1:
            opt.append('colz')
        self._SetOptStat()
        hist.SetLineColor(color)
        hist.Draw(' '.join(opt))
        fix_tcanvases()


    def run_command(self, command):
        return run_command(command, self.host, self.port)

    def LoadRemoteFile(self):
        self.run_command('TGRUTint::instance()->OpenFileDialog()')

    def LoadLocalFile(self):
        filename = askopenfilename(filetypes=(("ROOT File", "*.root"),))
        if not filename:
            return
        self.LoadRootFile(filename)

    def LoadRootFile(self,filename):
        tfile = ROOT.TFile(filename)
        self.files[filename] = tfile
        self._insert_to_hist_tree(tfile)

    def _insert_to_hist_tree(self,obj,tree_id=''):
        if obj.Class().InheritsFrom('TH2'):
            icon = self.icons['h2_t']
        elif obj.Class().InheritsFrom('TH1'):
            icon = self.icons['h1_t']
        elif obj.Class().InheritsFrom('TFile'):
            icon = self.icons['tfile']
        elif obj.Class().InheritsFrom('TDirectory'):
            icon = self.icons['folder_t']
        else:
            icon = ''

        index = len(self.hist_indices)
        self.hist_indices[index] = obj
        tree_id = self.hists.insert(tree_id,'end',index, text=obj.GetName(),image=icon)

        if obj.Class().InheritsFrom('TList'):
            iterable = obj
        elif hasattr(obj,'GetListOfKeys'):
            iterable = unpack_tdirectory(obj)
        else:
            iterable = None

        if iterable is not None:
            for obj in iterable:
                self._insert_to_hist_tree(obj, tree_id)

    def Interpreter(self):
        try:
            import ptpython.repl; ptpython.repl.embed(globals(), locals())
        except ImportError:
            pass
        else:
            return

        try:
            import IPython; IPython.embed()
        except ImportError:
            pass
        else:
            return

        import code; code.interact(local=locals())

    def _LoadTree(self):
        res = self.run_command('if(online_events) {'
                               '    gResponse = online_events->GetObjectStringLeaves();'
                               '}')
        if res is None:
            return

        res = filter(None,str(res).split('\n'))
        tree = Node('')
        for item in res:
            tree.AddChild(item)
        tree.ttk_treeview(self.tree)

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

    def Run(self):
        self.window.mainloop()

    def load_library(self, library_name, grut_lib = True):
        if grut_lib:
            library_name = os.path.join(os.path.dirname(__file__),
                                        '..','libraries',library_name)
        ROOT.gSystem.Load(library_name)

    def hello(self):
        print "hello!"

    def close_all_canvases(self):
        canvases = ROOT.gROOT.GetListOfCanvases()
        for canvas in canvases:
            canvas.Close()


    def open_canvas(self,title="",columns=-1,rows=-1,topx=0,topy=0,width=0,height=0):
        if not title:
            title = "canvas" + str(len(self.canvases))

        if columns==-1 or rows==-1:
            columns = self.zone_cols
            rows = self.zone_rows

        if width*height == 0:
            canvas = ROOT.GCanvas(title,title)
        else:
            canvas = ROOT.GCanvas(title,title,topx,topy,width,height);
        canvas.cd()

        if columns*rows == 1:
            self.plotlocation.set('NewCanvas')
        else:
            self.plotlocation.set('NextPad')
            canvas.Divide(columns,rows)
            canvas.cd(columns*rows)
        canvas.Modified()
        canvas.Update()

        self.canvases.append(canvas)

    def set_refresh(self):
        print("refresh = " + str(self.refreshrate.get()))
