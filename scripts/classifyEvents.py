#!/usr/bin/env python
import sys
old=sys.argv
sys.argv=[]
import ROOT
from math import sqrt,log
import sys
from numpy.random import randint,normal
import pypdt
ROOT.gStyle.SetOptStat(00000)
from AnalysisBase import *
from copy import deepcopy
sys.argv=old
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return

def analysis(filename,myWorker):
    histogram=ROOT.TH1F("analysis","analysis",100,0,100)
    infile=ROOT.TFile(filename)
    outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
    dir=outfile.mkdir(filename[:-5])
    dir.cd()
    iev=0
    edepMap={}
    particleMap={}
    sigmas=[0,1]
    try:
        for sigma in sigmas:
            suffix=""
            if sigma==0:
                suffix=""
            else:
                suffix="_real"
            hEdep=ROOT.TH1F("hEdep"+suffix,"E_{dep} in detector"+suffix[1:],300,0,300)
            hEdep.GetXaxis().SetTitle("E_{dep} [MeV]")
            hEdep.GetYaxis().SetTitle("rel. freq. [%]")
            for event in infile.sim:
                if len(filter(lambda hit:hit.edep>0.5,event.Trigger))==0:
                    continue
                edep=0
                for hit in event.Calorimeter:
                    edep+=hit.edep
                if edep>0:
                    if sigma>0:
                        s=1/(200.*sqrt(2*log(2)))*(407.8/edep+-15.18/sqrt(edep)+0.5582)
                        edep*=normal(1,s)
                    hEdep.Fill(edep)
            hEdep.Scale(1./hEdep.GetEntries())
            hEdep.Write()
        outfile.Close()
        return filename[:-5]
    except Exception as e:
        return e
def Terminate(self):
    AnalysisBase(self)
    

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
