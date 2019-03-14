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

def update_tcanvases(objects=None):
    if objects is None:
        objects = ROOT.gROOT.GetListOfCanvases()

    for obj in objects:
        if isinstance(obj, ROOT.TPad):
            obj.Modified()
            obj.Update()
            update_tcanvases(obj.GetListOfPrimitives())


def increment_name(name):
    res = re.search('[0-9]+$', name)
    if res:
        prefix = name[:-len(res.group())]
        number = int(res.group()) + 1
        return prefix + str(number)
    else:
        return name + '_1'


class PreserveGDir(object):

    def __init__(self, directory = None):
        self.directory = directory

    def __enter__(self):
        self.gdir = ROOT.gDirectory
        if self.directory is not None:
            self.directory.cd()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.gdir.cd()


class TKeyDict(dict):
    def __getitem__(self, key):
        output = super(TKeyDict,self).__getitem__(key)
        if isinstance(output, ROOT.TKey):
            replacement_classes = [(ROOT.TH2D, ROOT.GH2D),
                                   (ROOT.TH1D, ROOT.GH1D),
                                   (ROOT.TH1F, ROOT.GH1D),
                                   #(ROOT.TH2D, ROOT.xH2D),
                                   #(ROOT.TH3D, ROOT.xH3D),
                                   ]

            output = output.ReadObj()
            for in_cls,out_cls in replacement_classes:
                if (isinstance(output, in_cls) and
                    not isinstance(output, out_cls)):
                    replacement = out_cls(output)
                    output.Delete()
                    output = replacement
                    break
            self[key] = output

        return output

    def is_tkey(self, key):
        output = super(TKeyDict,self).__getitem__(key)
        try:
            return isinstance(output, ROOT.TKey)
        except AttributeError:
            return False
