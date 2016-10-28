#!/usr/bin/env python
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys
from numpy.random import randint
from time import sleep
import os
class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

"""
def main():
    edep_mean=[]
    Leff=range(30,70,5)
    for iLeff in Leff:
        filename="deuterons_"+str(iLeff)+".root"
        print "analysing "+str(filename)
        edep_mean.append(doFile(filename))
    for i in range(len(Leff)):
        Leff[i]=float(Leff[i])*1.0
    myGraph=ROOT.TGraph(len(edep_mean),asarray(Leff),asarray(edep_mean))
    myGraph.Print()
    c1=ROOT.TCanvas("c1","c1",800,600)
    myGraph.Draw("AP")
    c1.Print("c1.root")
main()
"""
from AnalysisBase import *

import ROOT
import sys

def analyse(filename,myWorker):
    try:
        edep=ROOT.TH1F("edep","edep after trigger",600,0,300)
        edep.GetYaxis().SetTitle("dN/dE_{dep} / .5/MeV")
        edep.GetXaxis().SetTitle("E_{dep} [MeV]")
        edep_raw=ROOT.TH1F("edep_raw","edep before trigger",600,0,300)
        edep_raw.GetYaxis().SetTitle("dN/dE_{dep} / 0.5/MeV")
        edep_raw.GetXaxis().SetTitle("E_{dep} [MeV]")
        infile=ROOT.TFile(filename)
        data=infile.Get("sim")
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        for event in data:
            triggerhits=filter(lambda hit:hit.edep>.5,event.Trigger)
            for hit in event.Detector:
                edep_raw.Fill(hit.edep)
            if len(triggerhits)==3:
                for hit in event.Detector:
                    edep.Fill(hit.edep)
                    
        edep.Write()
        edep_raw.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,": "+str(e)
        return None
    print "Finished",filename
    return (filename[:-5])

class dedxAnalysis(AnalysisBase):
   def Init(self):
      return
   def __init__self(self):
      AnalysisBase.__init__(self)
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=dedxAnalysis()
    myAnalysis.Init()
    leftToDo=myAnalysis(analyse)
    while len(leftToDo):
      myAnalysis(analyse)
    infile=ROOT.TFile(myAnalysis.args.output,"UPDATE")
    L=range(30,90,5)
    T=range(100,300,50)+[270]
    
    for iT in T:
       gEdep=ROOT.TGraph(len(T))
       gEdep.SetName("e_vs_l_deuterons_"+str(iT))
       gEdep.SetTitle("e_vs_l_deuterons_"+str(iT))
       i=0
       E=[]
       d=[]
       for iL in L:
          dir="deuterons_"+str(iT)+"_"+str(iL)
          print dir+"/edep"
          hEdep=infile.Get(dir+"/edep")
          if hEdep.GetEntries()>100:
             E.append(hEdep.GetMean())
             d.append(iL)
             gEdep.SetPoint(i,iL,hEdep.GetMean())
             i+=1
       gDeDx=ROOT.TGraph(i-1)
       gDeDx.SetName("dedx_vs_l_deuterons_"+str(iT))
       gDeDx.SetTitle("dedx_vs_l_deuterons_"+str(iT))
       for ii in range(i-1):
          de=E[ii+1]-E[ii]
          dx=d[ii+1]-d[ii]
          gDeDx.SetPoint(ii,d[ii+1],de/dx)
       gDeDx.Draw("AP")
       gDeDx.SetLineColor(ROOT.kWhite)
       gDeDx.SetMarkerSize(1.2)
       gDeDx.SetMarkerStyle(24)
       gDeDx.GetXaxis().SetTitle("L_{eff} [mm]")
       gDeDx.GetYaxis().SetTitle("#Delta E_{dep} / #Delta L_{eff} [MeV/mm]")
       gDeDx.Write()
       gEdep.Write()
    infile.Write()
    infile.Close()
    if os.path.isfile("bragg.root"): 
       exp=ROOT.TFile("dedx.root")
       truth=ROOT.TFile("bragg.root")
       
       hTruth=truth.Get("deuteron-lyso-270/dEdz")
       gExp=exp.Get("dedx_vs_l_deuterons_270")
       
       c1=ROOT.TCanvas("dedx","dE/dx",1600,900)
       
       gExp.SetTitle("")
       gExp.SetName("reconstructed")
       hTruth.SetTitle("")
       hTruth.SetName("mc truth")
       gExp.Draw()
       gExp.SetFillColor(ROOT.kWhite)
       gExp.GetXaxis().SetRangeUser(0,57)
       hTruth.Draw("SAME")
       c1.BuildLegend(0.2,0.7,0.6,0.85)
       c1.Print("dedx-270.pdf")
       c1.Print("dedx-270.root")
