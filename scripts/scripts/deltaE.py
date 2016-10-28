#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys

def analyse(filename,myWorker):
    try:
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        histos=[]
        histos.append(ROOT.TH2F("e1_vs_e2","E_{1} vs E_{2}",300,0,300,100,0,100))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        histomap["e1_vs_e2"].GetYaxis().SetTitle("E_{1} [MeV]")
        histomap["e1_vs_e2"].GetXaxis().SetTitle("E_{2} [MeV]")
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for entry in data:
            e1=0
            e2=0
            for hit in entry.Hodoscope:
                if hit.detid==0:
                    e1=hit.edep
                    e2=hit.edep
                if hit.detid==1:
                    e2+=hit.edep
            for hit in entry.Detector:
                e2+=hit.edep
            histomap["e1_vs_e2"].Fill(e2,e1)
        for h in histos:
            h.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])


def doEvent(hits,histomap):
    if len(hits)==0:
        return None
    
class e1e2Analysis(AnalysisBase):
    def Init(self):
        return
    def __init__(self):
        AnalysisBase.__init__(self)

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=e1e2Analysis()
    myAnalysis.Init()
    leftToDo=myAnalysis(analyse)
    while len(leftToDo):
      myAnalysis(analyse)
