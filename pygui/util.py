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
        if obj.InheritsFrom('TPad'):
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


class TKeyDict(dict):
    def __getitem__(self, key):
        output = super(TKeyDict,self).__getitem__(key)
        if output.InheritsFrom('TKey'):
            print 'Reading key ',output.GetName()
            value = output.ReadObj().Clone()
            value.SetDirectory(0)
            if hasattr(output, 'hist_pattern'):
                value.hist_pattern = output.hist_pattern
            self[key] = value
            output = value
        return output

    def is_tkey(self, key):
        output = super(TKeyDict,self).__getitem__(key)
        try:
            return output.InheritsFrom('TKey')
        except AttributeError:
            return False
