#!/usr/bin/env python2

import re

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

def unpack_tdirectory(tdir):
    if tdir.GetListOfKeys():
        for key in tdir.GetListOfKeys():
            yield key.ReadObj()
    else:
        for obj in tdir.GetList():
            yield obj

def update_tcanvases():
    for canvas in ROOT.gROOT.GetListOfCanvases():
        canvas.Modified()
        canvas.Update()

def increment_name(name):
    res = re.search('[0-9]+$', name)
    if res:
        prefix = name[:-len(res.group())]
        number = int(res.group()) + 1
        return prefix + str(number)
    else:
        return name + '_1'
