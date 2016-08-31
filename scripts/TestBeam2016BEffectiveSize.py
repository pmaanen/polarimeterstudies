#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot
from copy import deepcopy
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return


def analysis(filename,myWorker):
    try:

        hXE=ROOT.TH2F("hXE","x vs deposited energy",500,-250,250,300,0,300)
        hYE=ROOT.TH2F("hYE","y vs deposited energy",500,-250,250,300,0,300)
        hXY=ROOT.TH2F("hXY","coordinate of entry point",500,-250,250,500,-250,250)
        hXYE=ROOT.TH3F("hXYE","x vs y vs E_{dep}",500,-250,250,500,-250,250,300,0,300)
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            sum=0
            for hit in event.Left:
                sum+=hit.edep
            x=y=0
            for hit in event.TriggerL:
                if hit.trid==1:
                    x=hit.x
                    y=hit.y
            hXE.Fill(x,sum)
            hYE.Fill(y,sum)
            hXY.Fill(x,y)
            hXYE.Fill(x,y,sum)

        hXYE.Project3DProfile().Write()
        hXYE.Write()
        hXE.Write()
        hYE.Write()
        hXY.Write()
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
