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
class chit:
   def __init__(self,hit):
      self.detid=hit.detid
      self.nphot=hit.nphot
      self.event=hit.event

def doEvent(calo,cathode,histo):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    
    if len(calo)!=1 or len(cathode)!=1 or calo[0].event!=cathode[0].event:
    #    print "malformed event, event skipped"
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
    nphot=ROOT.TH1F("nphot","N_{#gamma}",300,0,300)
    nphot.GetYaxis().SetTitle("#")
    nphot.GetXaxis().SetTitle("N_{#gamma}")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    trigger=infile.Get("Cathode")
    
    calorhits=[]
    cathodehits=[]
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    for ihit in trigger:
        cathodehits.append(chit(ihit))
    calorhits.sort(key=lambda hit: hit.event)
    cathodehits.sort(key=lambda hit: hit.event)

        
    thisEventCalor=[]
    thisEventCathode=[]
    while True:
        iEvent=calorhits[-1].event
        thisEventCalor=getOneEvent(iEvent,calorhits)
        thisEventCathode=getOneEvent(iEvent,cathodehits)
        doEvent(thisEventCalor,thisEventCathode,nphot)
        if len(calorhits)==0 or len(cathodehits)==0:
            break
    nphot.Write()
    return nphot.GetMean()
def main():
    nphot_mean=[]
    zPos=range(5,100,10)
    for z in zPos:
        filename="muons_"+str(z)+".root"
        print "analysing "+str(filename)
        try:
            nphot_mean.append(doFile(filename))
        except:
            print "Error while analysing "+filename+":"
            raise
    for i in len(zPos):
       zPos[i]=float(zPos[i])*1.0
    myGraph=ROOT.TGraph(len(nphot_mean),asarray(zPos),asarray(nphot_mean))
    myGraph.GetYaxis().SetTitle("<N_{#gamma}>")
    myGraph.GetXaxis().SetTitle("z [mm]")
    c1=ROOT.TCanvas("","",800,600)
    myGraph.Draw("ALP")
    c1.Print("c1.root")
main()
