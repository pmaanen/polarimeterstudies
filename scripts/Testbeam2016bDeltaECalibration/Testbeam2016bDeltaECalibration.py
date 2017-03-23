#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot
from copy import deepcopy

ROOT.gStyle.SetOptStat(0000000000000)
gStyle=ROOT.gStyle
gStyle.SetMarkerStyle(20)
gStyle.SetMarkerSize(2.5)
gStyle.SetLabelSize(.045,"xy")
gStyle.SetTitleSize(.05,"xy")
gStyle.SetTitleOffset(1,"xy")
gStyle.SetOptFit(1)


class exampleAnalysis(AnalysisBase):
    def Init(self):
        return

class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))

c1=ROOT.TCanvas("graph","graph",1600,900)
def fitDir(infile,h,dirname):
    return

def makePlots(title,x,y,w):
    return
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return
    
def getSeed(detector):
   max=0
   maxid=-1
   for hit in detector:
      if hit.edep>max:
         max=hit.edep
         maxid=hit.detid
   return maxid

def getSum(detector):
    sum=0
    for hit in detector:
        sum+=hit.edep
    return sum

def doTrigger(trigger,calorimeter):
    E=getSum(calorimeter)
    max=getSeed(calorimeter)
    dE=getSum(trigger)
    if(E>5):
        return max,dE
    else:
        return -1,0

def analysis(filename,myWorker):
    try:
        histosL={}
        histosR={}
        for x in [0,1,2,3]:
            for y in [0,1,2]:
                histosL[10*x+y]=ROOT.TH1F("hTriggerL"+str(x)+"-"+str(y),"hTriggerL"+str(x)+"-"+str(y),100,0,20)
                histosR[10*x+y]=ROOT.TH1F("hTriggerR"+str(x)+"-"+str(y),"hTriggerR"+str(x)+"-"+str(y),100,0,20)
        hTrigger=ROOT.TH1F("hTrigger","hTrigger",100,0,20)
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            detIdL,TrigL=doTrigger(event.TriggerL,event.Left)
            detIdR,TrigR=doTrigger(event.TriggerR,event.Right)
            if TrigL>0.5:
                print "Left",detIdL,TrigL
            if TrigR>0.5:
                print "Right",detIdR,TrigR
            if(TrigL>0.5):
                histosL[detIdL].Fill(TrigL)
                hTrigger.Fill(TrigL)
            if(TrigR>0.5):
                histosR[detIdR].Fill(TrigR)
                hTrigger.Fill(TrigR)
        for h in histosL.itervalues():
            h.Write()
        for h in histosR.itervalues():
            h.Write()
        hTrigger.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename
        raise e
    return (filename[:-5])
    
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
    infile=ROOT.TFile(myAnalysis.args.output,"UPDATE")
    dir=infile.mkdir("plots")
    materials=["G4_GRAPHITE","G4_Al","G4_Mg","G4_Si"]
    energies=[200,235,250,270,300]
    lengths=[5]
    for material in materials:
        t=[]
        ed=[]
        w=[]
        for e in energies:
            continue
    
