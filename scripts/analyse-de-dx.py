#!/usr/bin/env python
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys
from numpy.random import randint
from time import sleep
class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

"""
def main():
    edep_mean=[]
    Leff=range(30,70,5)
    for iLeff in Leff:
        filename="deuterons_"+str(iLeff)+".root"
        print "analysing "+str(filename)
        edep_mean.append(doFile(filename))
    for i in range(len(Leff)):
        Leff[i]=float(Leff[i])*1.0
    myGraph=ROOT.TGraph(len(edep_mean),asarray(Leff),asarray(edep_mean))
    myGraph.Print()
    c1=ROOT.TCanvas("c1","c1",800,600)
    myGraph.Draw("AP")
    c1.Print("c1.root")
main()
"""
from AnalysisBase import *

import ROOT
import sys

def analyse(filename,myWorker):
    try:
        edep=ROOT.TH1F("edep","edep after trigger",600,0,300)
        edep.GetYaxis().SetTitle("dN/dE_{dep} / .5/MeV")
        edep.GetXaxis().SetTitle("E_{dep} [MeV]")
        edep_raw=ROOT.TH1F("edep_raw","edep before trigger",600,0,300)
        edep_raw.GetYaxis().SetTitle("dN/dE_{dep} / 0.5/MeV")
        edep_raw.GetXaxis().SetTitle("E_{dep} [MeV]")
        infile=ROOT.TFile(filename)
        data=infile.Get("sim")
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        for event in data:
            triggerhits=filter(lambda hit:hit.edep>.5,event.Trigger)
            for hit in event.Calorimeter:
                edep_raw.Fill(hit.edep)
            if len(triggerhits)==4:
                for hit in event.Calorimeter:
                    edep.Fill(hit.edep)
    except:
        print "Problem in file:",filename
        return None
   edep.Write()
   edep_raw.Write()
   outfile.Write()
   outfile.Close()
   return (filename[:-5])

class dedxAnalysis(AnalysisBase):
   def Init(self):
      return
    
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=dedxAnalysis()
    myAnalysis.Init()
    leftToDo=myAnalysis(analyse)
    while len(leftToDo):
      myAnalysis(analyse)
