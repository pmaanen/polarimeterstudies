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
        histos.append(ROOT.TH1F("edep","edep",1200,0,300))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        for entry in data:
            hits=[CaloHit(h) for h in entry.Detector]
            doEvent(hits,histomap)
        for h in histos:
            h.Write()
        #outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])


def doEvent(hits,histomap):
    if len(hits)==0:
        return None
    for hit in hits:
        histomap["edep"].Fill(hit.edep)

class hodoscopeAnalysis(AnalysisBase):
    def Init(self):
        return
    def __init__(self):
        AnalysisBase.__init__(self)

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=hodoscopeAnalysis()
    myAnalysis.Init()
    leftToDo=myAnalysis(analyse)
    while len(leftToDo):
      myAnalysis(analyse)
