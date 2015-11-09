#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys


class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

#class Landau:
#   def __call__( self, x, p ):
#       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])


class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event
class chit:
   def __init__(self,hit):
      self.detid=hit.detid
      self.nphot=hit.nphot
      self.event=hit.event

def doEvent(cathode,histo):
   histo.Fill(cathode[0].nphot)
   return
    

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
    nphot=ROOT.TH1F("nphot","N_{#gamma}",500,0,5e5)
    nphot.GetYaxis().SetTitle("#")
    nphot.GetXaxis().SetTitle("N_{#gamma}")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    cathode=infile.Get("Cathode")
    
    calorhits=[]
    cathodehits=[]
    for ihit in cathode:
        cathodehits.append(chit(ihit))
    cathodehits.sort(key=lambda hit: hit.event)
    thisEventCathode=[]
    while True:
        iEvent=cathodehits[-1].event
        thisEventCathode=getOneEvent(iEvent,cathodehits)
        doEvent(thisEventCathode,nphot)
        if len(cathodehits)==0:
            break
    fitfunc=ROOT.TF1("fitfunc","[0]*TMath::Gaus(x,[1],[2])",nphot.GetMean()-nphot.GetRMS(),nphot.GetMean()+nphot.GetRMS())
    fitfunc.SetParameter(0,nphot.GetBinContent(nphot.FindBin(nphot.GetMean())))
    fitfunc.SetParameter(1,nphot.GetMean())
    fitfunc.SetParameter(2,nphot.GetRMS())
    nphot.Fit(fitfunc,"QRM")
    nphot.Write()
    return [fitfunc.GetParameter(1),fitfunc.GetParError(1)]
def main():
    nphot_mean=[]
    nphot_err=[]
    zPos=range(5,100,10)
    for z in zPos:
        filename="muons_"+str(z)+".root"
        print "analysing "+str(filename)
        try:
           result=doFile(filename)
           nphot_err.append(result[1])
           nphot_mean.append(result[0])
        except:
            print "Error while analysing "+filename+":"
            raise
    for i in range(len(zPos)):
       zPos[i]=float(zPos[i])*1.0
    myGraph=ROOT.TGraphErrors(len(nphot_mean),asarray(zPos),asarray(nphot_mean),asarray(len(nphot_mean)*[0]),asarray(nphot_err))
    myGraph.GetYaxis().SetTitle("<N_{#gamma}>")
    myGraph.GetXaxis().SetTitle("z [mm]")
    c1=ROOT.TCanvas("","",800,600)
    myGraph.Draw("AP")
    c1.Print("c1.root")
main()
