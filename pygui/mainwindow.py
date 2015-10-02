#!/usr/bin/env python2

import os
import Tkinter as tk
from tkFileDialog import askopenfilename
import ttk

import ROOT

from pygui.run_command import run_command
from tree_structure import Node

class MainWindow(object):

    def __init__(self,host,port):
        self.host = host
        self.port = port

        self.load_library('libGui.so',False)
        self.load_library('libGROOT.so')

        self.window = tk.Tk()
        self._load_icons()


        self.refreshrate  = tk.IntVar()
        self.refreshrate.set(-1)
        self.plotlocation = tk.IntVar()
        self.plotlocation.set(1)

        self.canvases = []
        self.files = {}

        self._setup_GUI()

    def _load_icons(self):
        self.icons = {}
        self.icons['1d hist'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','h1_t.gif'))
        self.icons['2d hist'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','h2_t.gif'))
        self.icons['tfile'] = tk.PhotoImage(
            file = os.path.join(os.path.dirname(__file__),'resources','rootdb_t.gif'))

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
        self._MakeTreeView(tree_page)
        notebook.add(tree_page, text='Tree Viewer')

        hist_page = ttk.Frame(notebook)
        self._MakeHistView(hist_page)
        notebook.add(hist_page, text='Histograms')

        notebook.pack(fill=tk.BOTH,expand=True)

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
        filemenu.add_command(label="Open",command=self.hello)
        filemenu.add_command(label="Save",command=self.hello)
        filemenu.add_separator()
        filemenu.add_command(label="Exit",command=self.window.quit)
        self.menubar.add_cascade(label="File",menu=filemenu)

    def _MakeRefreshMenu(self):
        refreshmenu = tk.Menu(self.menubar,tearoff=0)
        refreshmenu.add_checkbutton(label="Off",onvalue=-1,
                                    variable=self.refreshrate,command=self.set_refresh)
        refreshmenu.add_command(label="Now!",command=self.hello)
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
        self.menubar.add_cascade(label="Zones",menu=zonesmenu)

    def _MakeOptStatMenu(self):
        optstatmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="OptStats",menu=optstatmenu)

    def _MakePlotMenu(self):
        plotmenu = tk.Menu(self.menubar,tearoff=0)
        plotmenu.add_checkbutton(label="New Canvas",onvalue=1,
                                 variable=self.plotlocation,command=self.set_plotlocation)
        plotmenu.add_checkbutton(label="Next Pad",onvalue=2,
                                 variable=self.plotlocation,command=self.set_plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (replace)",onvalue=3,
                                 variable=self.plotlocation,command=self.set_plotlocation)
        plotmenu.add_checkbutton(label="Current Pad (overlay)",onvalue=4,
                                 variable=self.plotlocation,command=self.set_plotlocation)
        self.menubar.add_cascade(label="Plot",menu=plotmenu)

    def _MakeHelpMenu(self):
        helpmenu = tk.Menu(self.menubar,tearoff=0)
        self.menubar.add_cascade(label="Send Help",menu=helpmenu)

    def _MakeTreeView(self,parent):
        self.tree = ttk.Treeview(parent)
        self.tree.pack(fill=tk.BOTH,expand=True)
        self._LoadTree()
        self.tree.bind("<Double-1>", self.ParameterSelection)

    def _MakeHistView(self,parent):
        self.hists = ttk.Treeview(parent)
        self.hists.pack(fill=tk.BOTH,expand=True)
        self.hists.bind("<Double-1>", self.OnHistClick)

    def OnHistClick(self,event):
        hist_name = event.widget.selection()[0]
        try:
            file_name = event.widget.parent(hist_name)
        except TclError:
            return

        try:
            hist = self.files[file_name].Get(hist_name)
        except KeyError:
            return

        if not filter(None,self.canvases):
            self.canvases.append(ROOT.GCanvas())
        hist.Draw()

    def run_command(self, command):
        return run_command(command, self.host, self.port)

    def LoadRemoteFile(self):
        self.run_command('TGRUTint::instance()->OpenFileDialog()')

    def LoadLocalFile(self):
        filename = askopenfilename(filetypes=(("ROOT File", "*.root"),))
        if not filename:
            return

        tfile = ROOT.TFile(filename)
        self.files[filename] = tfile

        icon = self.icons['tfile']
        tree_id = self.hists.insert('','end',filename, text=filename, image=icon)
        for key in tfile.GetListOfKeys():
            obj = key.ReadObj()
            hist_name = obj.GetName()
            if obj.Class().InheritsFrom('TH2'):
                icon = self.icons['2d hist']
            elif obj.Class().InheritsFrom('TH1'):
                icon = self.icons['1d hist']
            else:
                icon = None

            self.hists.insert(tree_id,'end',hist_name, text=hist_name,
                              image=icon)


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
        print 'You clicked "{}"'.format(event.widget.selection()[0])

    def Run(self):
        self.window.mainloop()

    def load_library(self, library_name, grut_lib = True):
        if grut_lib:
            library_name = os.path.join(os.path.dirname(__file__),
                                        '..','libraries',library_name)
        ROOT.gSystem.Load(library_name)

    def hello(self):
        print "hello!"

    def open_canvas(self):
        self.canvases.append(ROOT.GCanvas())

    def set_refresh(self):
        print("refresh = " + str(self.refreshrate.get()))

    def set_plotlocation(self):
        print("plot location = " + str(self.plotlocation.get()))
