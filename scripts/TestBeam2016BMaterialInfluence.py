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


def doDetector(detector,hVertex,hEvsR,hIsPrimaryTrack):
    for hit in detector:
        if hit.trid==1:
            hIsPrimaryTrack.Fill(1)
        else:
                hIsPrimaryTrack.Fill(0)
        x=hit.x-hit.z*hit.px/hit.pz
        y=hit.y-hit.z*hit.py/hit.pz
        r=hypot(x,y)
        hVertex.Fill(x,y)
        hEvsR.Fill(hit.ekin,r)

def analysis(filename,myWorker):
    try:
        hVertex=ROOT.TH2F("hVertex","vertex coordinates",200,-10,10,200,-10,10)
        hEvsR=ROOT.TH2F("hEvsR","Energy vs vertex distance",300,0,300,200,0,20)
        hIsPrimaryTrack=ROOT.TH1F("is_primary_track","is_primary_track",2,-0.5,1.5)
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            doDetector(event.Left,hVertex,hEvsR,hIsPrimaryTrack)
            doDetector(event.Right,hVertex,hEvsR,hIsPrimaryTrack)
        hIsPrimaryTrack.Write()
        hVertex.Write()
        hEvsR.Write()
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
