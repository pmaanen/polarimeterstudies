#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from copy import deepcopy
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return
        
def analysis(filename,myWorker):
    infile=ROOT.TFile(filename,"READ")
    data=infile.Get("sim")
    outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
    dir=outfile.mkdir(filename[:-5])
    dir.cd()
    hEdep=ROOT.TH1F("hEdep","deposited energy in detector",3000,0,300)
    hEkin=ROOT.TH1F("hEkin","kinetic energy in front of detector",3000,0,300)
    try:
        for event in data:
            start=False
            if len(filter(lambda hit:hit.edep>0.25,event.Start))==len(event.Start):
                start=True
            for hit in event.Detector:
                if hit.trid==1 and start:
                    hEkin.Fill(hit.ekin)
            for hit in event.Hodoscope:
                if start:
                    hEdep.Fill(hit.edep)
        hEdep.Write()
        hEkin.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])


if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
