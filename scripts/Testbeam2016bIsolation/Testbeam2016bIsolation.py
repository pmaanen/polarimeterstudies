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


def getSeed(detector):
   max=0
   maxid=-1
   for hit in detector:
      if hit.edep>max:
         max=hit.edep
         maxid=hit.detid
   return maxid
    
def getCluster(detector,id,mask):
    sum=0
    center=0
    for hit in detector:
        if hit.detid in mask:
            sum+=hit.edep
            if hit.detid==id:
                center+=hit.edep
    return center,sum


maskInner=[00,10,20,1,11,21,2,12,22]
maskOuter=[21,22,23,11,12,23,1,2,3]
def doDetector(detector,histos):
    #Inner
    # 02 12 22                                                                                                     
    # 01 11 21                                                                                                     
    # 00 10 20
    center,sum=getCluster(detector,11,maskInner)
    if sum>5.:
        histos["hInnerVsCluster"].Fill(center,sum)
    #Outer
    # 21 22 23                                                                   
    # 11 12 23                                                                                                     
    # 01 02 03 
    center,sum=getCluster(detector,12,maskOuter)
    if sum>5.:
        histos["hOuterVsCluster"].Fill(center,sum)


def doLeft(det,histos):
    #LU2
   seed=getSeed(det)
   if seed==12:
      center,sum=getCluster(det, 12, maskOuter)
      histos["LU2Center"].Fill(center)
      histos["LU2Cluster"].Fill(sum)
    #LU3
   if seed==11:
      center,sum=getCluster(det, 11, maskInner)
      histos["LU3Center"].Fill(center)
      histos["LU3Cluster"].Fill(sum)
   for hit in det:
      y=int(hit.detid/10)
      x=hit.detid % 10
      histos["Left"].Fill(x,y)
   return

def doRight(det,histos):
    #RU2
   seed=getSeed(det)
   if seed==12:
      center,sum=getCluster(det, 12, maskOuter)
      histos["RU2Center"].Fill(center)
      histos["RU2Cluster"].Fill(sum)
    #RU3
   if seed==11:
      center,sum=getCluster(det, 11, maskInner)
      histos["RU3Center"].Fill(center)
      histos["RU3Cluster"].Fill(sum)
       
   for hit in det:
      y=int(hit.detid/10)
      x=hit.detid % 10
      histos["Right"].Fill(x,y)
   return


def analysis(filename,myWorker):
    try:
        histos={}
        histos["hInnerVsCluster"]=ROOT.TH2F("hInnerVsCluster","E_{0} vs #Sigma E",1200,0,300,1200,0,300)
        histos["hInnerVsCluster"].GetXaxis().SetTitle("E_{0} [MeV]")
        histos["hInnerVsCluster"].GetYaxis().SetTitle("#Sigma E [MeV]")
        histos["hOuterVsCluster"]=ROOT.TH2F("hOuterVsCluster","E_{0} vs #Sigma E",1200,0,300,1200,0,300)
        histos["hOuterVsCluster"].GetXaxis().SetTitle("E_{0} [MeV]")
        histos["hOuterVsCluster"].GetYaxis().SetTitle("#Sigma E [MeV]")
        
        
        histos["LU2Center"]=ROOT.TH1F("hLU2Center","LU2Center",1200,0,300)
        histos["LU2Cluster"]=ROOT.TH1F("hLU2Cluster","LU2Cluster",1200,0,300)
        histos["LU3Center"]=ROOT.TH1F("hLU3Center","LU3Center",1200,0,300)
        histos["LU3Cluster"]=ROOT.TH1F("hLU3Cluster","LU3Cluster",1200,0,300)
        
        histos["RU2Center"]=ROOT.TH1F("hRU2Center","RU2Center",1200,0,300)
        histos["RU2Cluster"]=ROOT.TH1F("hRU2Cluster","RU2Cluster",1200,0,300)
        histos["RU3Center"]=ROOT.TH1F("hRU3Center","RU3Center",1200,0,300)
        histos["RU3Cluster"]=ROOT.TH1F("hRU3Cluster","RU3Cluster",1200,0,300)
        
        histos["Left"]=ROOT.TH2F("hLeft","Left",4,-0.5,3.5,4,-0.5,3.5)
        histos["Right"]=ROOT.TH2F("hRight","Right",4,-0.5,3.5,4,-0.5,3.5)
        
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
           #doDetector(event.Right,histos)
           #doDetector(event.Left,histos)
           doRight(event.Right,histos)
           doLeft(event.Left,histos)
        for h in histos.itervalues():
           h.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        raise e
    return (filename[:-5])
 
 
def main():
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
 
   
if __name__=="__main__":
    main()
