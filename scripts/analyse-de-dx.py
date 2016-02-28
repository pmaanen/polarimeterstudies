#!/usr/bin/env python
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys
from numpy.random import randint
from time import sleep
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
   outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
   dir=outfile.mkdir(filename[:-5])
   dir.cd()
   edep=ROOT.TH1F("edep","edep after trigger",600,0,300)
   edep.GetYaxis().SetTitle("dN/dE_{dep} / .5/MeV")
   edep.GetXaxis().SetTitle("E_{dep} [MeV]")
   edep_raw=ROOT.TH1F("edep_raw","edep before trigger",600,0,300)
   edep_raw.GetYaxis().SetTitle("dN/dE_{dep} / 0.5/MeV")
   edep_raw.GetXaxis().SetTitle("E_{dep} [MeV]")
   infile=ROOT.TFile(filename,"UPDATE")
   calorimeter=infile.Get("Calorimeter")
   trigger=infile.Get("Trigger")
   calorhits=[]
   triggerhits=[]
   try:
      calorhits=unpack(calorimeter,CaloHit)
      triggerhits=unpack(trigger,CaloHit)
      thisEventCalor=[]
      thisEventDe=[]
      while True:
         iEvent=calorhits[-1].event
         thisEventCalor=getOneEvent(iEvent,calorhits)
         thisEventDe=getOneEvent(iEvent,triggerhits)
         doEvent(thisEventCalor,thisEventDe,edep,edep_raw)
         if len(calorhits)==0 or len(triggerhits)==0:
            break
   except:
      print "Problem in file:",filename
      return None
   edep.Write()
   edep_raw.Write()
   outfile.Write()
   outfile.Close()
   return (filename[:-5])

def doEvent(calo,de,edep,edep_raw):
   if len(calo)==0:
      print "malformed event, event skipped" 
      return
   edep_raw.Fill(calo[0].edep)
   if len(filter(lambda x: x.edep>0.5,de))==4:
      edep.Fill(calo[0].edep)
   return

class dedxAnalysis(AnalysisBase):
   def Init(self):
      return
    
if __name__=="__main__":
   myAnalysis=dedxAnalysis()
   myAnalysis.Init()
   myAnalysis(analyse)
