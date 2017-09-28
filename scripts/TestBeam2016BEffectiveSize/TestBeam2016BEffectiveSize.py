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


def sumDet(detector):
    sum=0
    for hit in detector:
        sum+=hit.edep
    return sum

def doEvent(event,histos,efficienciesX,efficienciesY):
    #TODO: go through event, decide for every cut Ecut=0..Tbeam,10 if event passed
    #cut is based on sum(?)
    cuts=range(0,280,10)
    edep=sumDet(event.Right)
    for ii in range(len(cuts)): 
        for hit in event.ObserverR:
            if hit.trid>1:
                continue
            bPassed=False
            if(edep>cuts[ii]):
                bPassed=True
            efficiencesX[ii].Fill(hit.x,bPassed)
            efficiencesY[ii].Fill(hit.y,bPassed)
    edep=sumDet(event.Left)
    for ii in range(len(cuts)):
        for hit in event.ObserverL:
            if hit.trid>1:
                continue
            bPassed=False
            if(edep>cuts[ii]):
                bPassed=True
            efficiencesX[ii].Fill(hit.x,bPassed)
            efficiencesY[ii].Fill(hit.y,bPassed)
        
  
        

def analysis(filename,myWorker):
    try:
        histos={}
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        infile=ROOT.TFile(filename,"READ")
        efficienciesX=[]
        efficienciesY=[]
        cuts=range(0,280,10)
        for cut in cuts:
            efficienciesX.append(ROOT.TEfficiency("effX"+str(cut),";x;#epsilon",27,0,270))
            efficienciesY.append(ROOT.TEfficiency("effY"+str(cut),";x;#epsilon",27,0,270))
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            doEvent(event,histos,efficienciesX,efficienciesY)
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
