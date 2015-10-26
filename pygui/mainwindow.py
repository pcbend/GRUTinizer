#!/usr/bin/env python2

import ast
import os
import pprint
import Tkinter as tk
import tkFileDialog
import ttk

import sys

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

from .run_command import run_command
from .tree_tab import TreeTab
from .hist_tab import HistTab
from .tcut_tab import TCutTab
from .variable_tab import VariableTab
from .util import update_tcanvases
from .AnsiColorText import AnsiColorText

class MainWindow(object):

    def __init__(self):

        self.window = tk.Tk()
        self._load_icons()
        self._load_default_style()

        self.canvases = []
        self.files = {}

        self.is_online = ROOT.TGRUTOptions.Get().IsOnline()

        self._setup_GUI()

    def LoadGuiFile(self, filename):
        with open(filename) as f:
            text = f.read()
            settings = ast.literal_eval(text)

        if 'histograms' in settings:
            self.hist_tab._load_online_patterns(settings['histograms'])
        if 'tcuts' in settings:
            self.tcut_tab._load_tcut_patterns(settings['tcuts'])
        if 'compiled_histograms' in settings:
            self.hist_tab._load_compiled_histograms(settings['compiled_histograms']['name'])
        if 'variables' in settings:
            self.variable_tab._load_variable_patterns(settings['variables'])

    def _save_gui_file(self, filename = None):
        if filename is None:
            filename = tkFileDialog.asksaveasfilename(filetypes=(("GUI File", "*.hist"),))

        if not filename:
            return

        if not filename.endswith('.hist'):
            filename += '.hist'

        output = {}
        output['histograms'] = self.hist_tab._hist_patterns()
        output['tcuts'] = self.tcut_tab._tcut_patterns()
        output['variables'] = self.variable_tab._variable_patterns()

        if self.hist_tab._compiled_histogram_filename():
            output['compiled_histograms'] = {'name':self.hist_tab._compiled_histogram_filename()}

        with open(filename,'w') as f:
            pprint.pprint(output, f)

    def _dump_root_file(self, filename = None, include_histograms = True):
        if filename is None:
            filename = tkFileDialog.asksaveasfilename(filetypes=(("ROOT File", "*.root"),))

        if not filename:
            return

        if not filename.endswith('.root'):
            filename += '.root'

        self.RefreshHistograms()
        output = ROOT.TFile(filename,'RECREATE')
        if include_histograms:
            self.hist_tab._dump_to_tfile()
        self.tcut_tab._dump_to_tfile()
        self.variable_tab._dump_to_tfile()
        output.Close()

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
        self.icons['tcutg'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','bld_cut.gif'))
        self.icons['ttree'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','ttree_t.gif'))
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
        ws = self.window.winfo_screenwidth()
        hs = self.window.winfo_screenheight()

        self.window.geometry('450x850+%d+%d' % (ws-475,25))
        self.window.wm_title("hist-o-matic")
        self.window.config(menu=self._MakeMenuBar())

        frame = tk.Frame(self.window)
        button = tk.Button(frame,
                           text='Load Data File',fg="black",bg="goldenrod",
                           command=self.LoadDataFile)
        button.pack(side=tk.LEFT)

        button = tk.Button(frame,
                           text='Load Root File',fg="black",bg="goldenrod",
                           command=self.LoadRootFile)
        button.pack(side=tk.LEFT)

        button = tk.Button(frame,
                           text='Refresh',fg="black",bg="goldenrod",
                           command=self.RefreshHistograms)
        button.pack(side=tk.LEFT)

        button = tk.Button(frame,
                           text='Reset',fg="black",bg="goldenrod",
                           command=self.ResetHistograms)
        button.pack(side=tk.LEFT)

        button = tk.Button(frame,
                           text='Reset All',fg="black",bg="goldenrod",
                           command=self.ResetAllHistograms)
        button.pack(side=tk.LEFT)

        frame.pack(fill=tk.X,expand=False)

        notebook = ttk.Notebook(self.window)

        hist_page = ttk.Frame(notebook)
        self.hist_tab = HistTab(self, hist_page)
        notebook.add(hist_page, text='Histograms')

        tcut_page = ttk.Frame(notebook)
        self.tcut_tab = TCutTab(self, tcut_page)
        notebook.add(tcut_page, text='Gates')

        variable_page = ttk.Frame(notebook)
        self.variable_tab = VariableTab(self, variable_page)
        notebook.add(variable_page, text='Variables')

        tree_page = ttk.Frame(notebook)
        self.tree_tab = TreeTab(self, tree_page)
        notebook.add(tree_page, text='Tree Viewer')

        notebook.pack(fill=tk.BOTH,expand=True)

        self._setup_status_bar(self.window)

    def _setup_status_bar(self, parent):
        frame = tk.Frame(parent, height=40)
        frame.propagate(False)
        self.status_bar = AnsiColorText(frame, bg='black')
        self.status_bar.pack(fill=tk.X,expand=True)
        frame.pack(fill=tk.X, expand=False)
        self.window.after_idle(self._update_status_bar)

    def _update_status_bar(self):
        self.status_bar.delete(1.0, tk.END)
        infile = ROOT.TGRUTLoop.Get().GetInfile()
        if infile:
            self.status_bar.write(infile.Status())
        self.window.after(1000, self._update_status_bar)

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

    def _MakeMenuBar(self):
        menubar  = tk.Menu(self.window)

        self._MakeFileMenu(menubar)
        self._MakeRefreshMenu(menubar)
        self._MakeZoneMenu(menubar)
        self._MakeOptStatMenu(menubar)
        self._MakePlotMenu(menubar)
        self._MakeHelpMenu(menubar)

        return menubar

    def _MakeFileMenu(self, menubar):
        self.defaultcanvassize = tk.StringVar(value='800x600')
        filemenu = tk.Menu(menubar,tearoff=0)
        filemenu.add_command(label="New Canvas",command=self.open_canvas)
        filemenu.add_separator()
        filemenu.add_checkbutton(label="Size 800 x 600",onvalue="800x600",
                                 variable=self.defaultcanvassize)
        filemenu.add_checkbutton(label="Size 1200 x 600",onvalue="1200x600",
                                 variable=self.defaultcanvassize)
        filemenu.add_checkbutton(label="Size 1900 x 450",onvalue="1900x450",
                                 variable=self.defaultcanvassize)
        filemenu.add_checkbutton(label="Size 1900 x 900",onvalue="1900x900",
                                 variable=self.defaultcanvassize)
        filemenu.add_separator()
        filemenu.add_command(label="Close All Canvases",command=self.close_all_canvases)
        filemenu.add_separator()
        filemenu.add_command(label="Open GUI",command=self.hello)
        filemenu.add_command(label="Save GUI",command=self._save_gui_file)
        filemenu.add_command(label="Dump ROOT Config",
                             command=lambda :self._dump_root_file(include_histograms=False))
        filemenu.add_command(label="Dump ROOT Histograms",command=self._dump_root_file)
        filemenu.add_separator()
        filemenu.add_command(label="Exit",command=ROOT.TGRUTint.instance().Terminate)
        menubar.add_cascade(label="File",menu=filemenu)

    def _PickIcon(self, obj):
        # If this is a TKey, look up the icon for the thing it points to.
        if obj.InheritsFrom('TKey'):
            obj = ROOT.TClass(obj.GetClassName())

        if obj.InheritsFrom('TH2'):
            return self.icons['h2_t']
        elif obj.InheritsFrom('TH1'):
            return self.icons['h1_t']
        elif obj.InheritsFrom('TFile'):
            return self.icons['tfile']
        elif obj.InheritsFrom('TDirectory'):
            return self.icons['folder_t']
        elif obj.InheritsFrom('TList'):
            return self.icons['folder_t']
        elif obj.InheritsFrom('TTree'):
            return self.icons['ttree']
        else:
            return ''

    def get_canvas_size(self,size=""):
        if not size:
            size = self.defaultcanvassize.get()
        size = size.lower()
        size.replace(" ","")
        size = size.split("x")
        array =(size[0],size[1])
        return array

    def _MakeRefreshMenu(self,menubar):
        self.refreshrate  = tk.IntVar(value='-1')

        refreshmenu = tk.Menu(menubar,tearoff=0)
        refreshmenu.add_checkbutton(label="Off",onvalue=-1,
                                    variable=self.refreshrate)
        refreshmenu.add_command(label="Now!",command=self.RefreshHistograms)
        refreshmenu.add_separator()
        refreshmenu.add_checkbutton(label="1 second",onvalue=1,
                                    variable=self.refreshrate)
        refreshmenu.add_checkbutton(label="2 seconds",onvalue=2,
                                    variable=self.refreshrate)
        refreshmenu.add_checkbutton(label="5 seconds",onvalue=5,
                                    variable=self.refreshrate)
        refreshmenu.add_checkbutton(label="10 seconds",onvalue=10,
                                    variable=self.refreshrate)
        refreshmenu.add_checkbutton(label="30 seconds",onvalue=30,
                                    variable=self.refreshrate)
        menubar.add_cascade(label="Refresh",menu=refreshmenu)
        self.window.after_idle(self._PeriodicRefresh)

    def _PeriodicRefresh(self):
        if self.refreshrate.get()<0:
            self.window.after(1000,self._PeriodicRefresh)
            return

        self.RefreshHistograms()
        self.window.after(self.refreshrate.get()*1000, self._PeriodicRefresh)

    def _MakeZoneMenu(self, menubar):
        self.predefinedzones = tk.StringVar(value='1x1')
        self.zone_rows = 1
        self.zone_cols = 1

        zonesmenu = tk.Menu(menubar,tearoff=0)
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
        zonesmenu.add_checkbutton(label="3 x 1",onvalue='3x1',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="3 x 2",onvalue='3x2',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="3 x 3",onvalue='3x3',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="4 x 4",onvalue='4x4',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="8 x 4",onvalue='8x4',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="12 x 12",onvalue='12x12',
                                  variable=self.predefinedzones,command=self.set_zones)
        zonesmenu.add_checkbutton(label="13 x 13",onvalue='13x13',
                                  variable=self.predefinedzones,command=self.set_zones)
        menubar.add_cascade(label="Zones",menu=zonesmenu)

    def set_zones(self):
        zones_str = self.predefinedzones.get()
        try:
            zones = map(int,zones_str.split("x"))
            self.zone_cols,self.zone_rows = zones
        except (IndexError,ValueError):
            print 'Cannot set zones to "{}"'.format(zones_str)

    def _MakeOptStatMenu(self, menubar):
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

        optstatmenu = tk.Menu(menubar,tearoff=0)
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
        menubar.add_cascade(label="OptStats",menu=optstatmenu)

    def _MakePlotMenu(self, menubar):
        self.plotlocation = tk.StringVar(value='NewCanvas')

        plotmenu = tk.Menu(menubar,tearoff=0)
        plotmenu.add_checkbutton(label="New Canvas",onvalue='NewCanvas',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Next Pad",onvalue='NextPad',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (replace)",onvalue='Replace',
                                 variable=self.plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (overlay)",onvalue='Overlay',
                                 variable=self.plotlocation)
        menubar.add_cascade(label="Plot",menu=plotmenu)

    def _MakeHelpMenu(self,menubar):
        helpmenu = tk.Menu(menubar,tearoff=0)
        menubar.add_cascade(label="Send Help",menu=helpmenu)

    def RefreshHistograms(self):
        if ROOT.online_events:
            ROOT.online_events.FillParsedHistograms()
        if ROOT.online_scalers:
            ROOT.online_scalers.FillParsedHistograms()
        update_tcanvases()

    def ResetHistograms(self,hist=None):
        if hist is None:
            for obj in ROOT.gPad.GetListOfPrimitives():
                if obj.InheritsFrom(ROOT.TH1.Class()):
                    obj.Reset()
        else:
            hist.Reset()
        update_tcanvases()

    def ResetAllHistograms(self):
        if ROOT.online_events:
            ROOT.online_events.ClearHistograms()
        if ROOT.online_scalers:
            ROOT.online_scalers.ClearHistograms()
        self.RefreshHistograms()

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

    def LoadDataFile(self, filename = None):
        if filename is None:
            filename = tkFileDialog.askopenfilename(filetypes=(("GEB File", "*.dat"),))

        if not filename:
            return

        ROOT.TGRUTLoop.Get().Stop()
        ROOT.TGRUTLoop.Get().ProcessFile(filename,'')
        ROOT.TGRUTLoop.Get().Start()

    def LoadRootFile(self,filename=None):
        if filename is None:
            filename = tkFileDialog.askopenfilename(filetypes=(("ROOT File", "*.root"),))

        if not filename:
            return

        filename = os.path.abspath(filename)
        tfile = ROOT.TGRUTint.instance().OpenRootFile(filename)
        self.files[filename] = tfile
        self.hist_tab.Insert(tfile)
        self.tree_tab.AddFile(tfile)
        self.tcut_tab.AddFile(tfile)
        self.variable_tab.AddFile(tfile)

    def Run(self):
        self.window.mainloop()

    def Update(self):
        self.window.update()

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
            array = self.get_canvas_size()
            array = map(int,array)
            canvas = ROOT.GCanvas(title,title,0,0,array[0],array[1])
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
