#!/usr/bin/env python
from ROOT import TTree, TFile
import argparse

parser = argparse.ArgumentParser(description='Convert a ascii table into a root tree.')
parser.add_argument('src')
parser.add_argument('dst')
args = parser.parse_args()

outfile=TFile(args.dst,"RECREATE")
sim=TTree("sim","sim")
print sim.ReadFile(args.src)
sim.Write()
outfile.Close()
