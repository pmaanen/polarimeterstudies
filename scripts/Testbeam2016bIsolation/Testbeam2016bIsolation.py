#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot
from copy import deepcopy

#ROOT.gStyle.SetOptStat(0000000000000)
#gStyle=ROOT.gStyle
#gStyle.SetMarkerStyle(20)
#gStyle.SetMarkerSize(2.5)
#gStyle.SetLabelSize(.045,"xy")
#gStyle.SetTitleSize(.05,"xy")
#gStyle.SetTitleOffset(1,"xy")
#gStyle.SetOptFit(1)


class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

class exampleAnalysis(AnalysisBase):
    def Init(self):
        return

c1=ROOT.TCanvas("graph","graph",1600,900)

class exampleAnalysis(AnalysisBase):
    def Init(self):
        return
    
def getCluster(detector,id,mask):
    sum=0
    center=0
    for hit in detector:
        if hit.detid in mask:
            sum+=hit.edep
            if hit.detid==id:
                center+=hit.edep
    return center,sum


def doDetector(detector,histos):
    #Inner
    # 02 12 22                                                                                                     
    # 01 11 21                                                                                                     
    # 00 10 20
    mask=[00,10,20,1,11,21,2,12,22]
    center,sum=getCluster(detector,11,mask)
    if sum>5.:
        histos["hInnerVsCluster"].Fill(center,sum)
    #Outer
    # 21 22 23                                                                   
    # 11 12 23                                                                                                     
    # 01 02 03 
    mask=[21,22,23,11,12,23,1,2,3]
    center,sum=getCluster(detector,12,mask)
    if sum>5.:
        histos["hOuterVsCluster"].Fill(center,sum)



def analysis(filename,myWorker):
    try:
        histos={}
        histos["hInnerVsCluster"]=ROOT.TH2F("hInnerVsCluster","E_{center} vs #Sigma E",1200,0,300,1200,0,300)
        histos["hInnerVsCluster"].GetXaxis().SetTitle("E_{center} [MeV]")
        histos["hInnerVsCluster"].GetYaxis().SetTitle("#Sigma E [MeV]")
        histos["hOuterVsCluster"]=ROOT.TH2F("hOuterVsCluster","E_{center} vs #Sigma E",1200,0,300,1200,0,300)
        histos["hOuterVsCluster"].GetXaxis().SetTitle("E_{center} [MeV]")
        histos["hOuterVsCluster"].GetYaxis().SetTitle("#Sigma E [MeV]")
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            doDetector(event.Right,histos)
            doDetector(event.Left,histos)
        for h in histos.itervalues():
            h.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        raise e
    return (filename[:-5])
    
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
    #infile=ROOT.TFile(myAnalysis.args.output,"UPDATE")
    
