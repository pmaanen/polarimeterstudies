#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys


class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event

def doEvent(calo,histo):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    histo.Fill(calo[0].edep)
def getOneEvent(EventIndex,EventList):
    result=[]
    while True:
        if len(EventList)==0:
            break
        if EventList[-1].event==EventIndex:
           result.append(EventList.pop())
        else:
            break
    return result
    
def doFile(filename):
    edep=ROOT.TH1F("edep","edep after trigger",300,0,300)
    edep.GetYaxis().SetTitle("dN/dE_{dep} / 1/MeV")
    edep.GetXaxis().SetTitle("E_{dep} / MeV")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    calorhits=[]
    
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    calorhits.sort(key=lambda hit: hit.event)
    thisEventCalor=[]
    while True:
        iEvent=calorhits[-1].event
        thisEventCalor=getOneEvent(iEvent,calorhits)
        doEvent(thisEventCalor,edep)
        if len(calorhits)==0:
            break
    fitfunc = ROOT.TF1('fitfunc','gaus(0)',min(25,edep.GetMean()-5*edep.GetRMS()),edep.GetMean()+5*edep.GetRMS())
    fitfunc.SetParameter(0,edep.GetMaximum())
    fitfunc.SetParameter(1,edep.GetMean())
    fitfunc.SetParameter(2,edep.GetRMS())
    edep.Fit(fitfunc,"IR")
    edep.Write()
    if(fitfunc.GetChisquare()>10):
       return [edep.GetMean(),edep.GetRMS()/sqrt(edep.GetEntries())]
    else: 
       return [fitfunc.GetParameter(1),fitfunc.GetParError(1)]
def main():
    edep_mean=[]
    edep_error=[]
    Ekin_error=[]
    Ekin=range(240,305,5)
    for iEkin in Ekin:
        filename="40_Fe_Plastic_"+str(iEkin)+".root"
        print "analysing "+str(filename)
        result=doFile(filename)
        edep_mean.append(result[0])
        edep_error.append(result[1])
        Ekin_error.append(0)
    for i in range(len(Ekin)):
        Ekin[i]=float(Ekin[i])*1.0
    myGraph=ROOT.TGraphErrors(len(edep_mean),asarray(Ekin),asarray(edep_mean),asarray(Ekin_error),asarray(edep_error))
    myGraph.Print()
    c1=ROOT.TCanvas("c1","c1",800,600)
    myGraph.Draw("AP")
    c1.Print("c1.root")
main()
