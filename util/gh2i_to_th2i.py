#!/usr/bin/env grutinizer-python

import glob
import os
import sys

import ROOT

def convert(input_filename, output_filename):
    tf_in = ROOT.TFile(input_filename)
    tf_out = ROOT.TFile(output_filename, 'RECREATE')
    for key in tf_in.GetListOfKeys():
        obj = key.ReadObj()
        if isinstance(obj, ROOT.GH2I):
            output = ROOT.TH2I(obj)
        else:
            output = obj

        if not isinstance(obj, ROOT.TTree):
            output.Write()

for filename in sys.argv[2:]:
    base = os.path.splitext(filename)[0]
    output = base + '_th2i.root'
    print 'Converting {} to {}'.format(filename, output)
    convert(filename, output)
