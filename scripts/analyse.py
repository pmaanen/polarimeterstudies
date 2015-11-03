#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys


class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event

def doEvent(calo,de,histo):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    
    if len(calo)!=1 or len(de)!=1 or calo[0].event!=de[0].event:
    #    print "malformed event, event skipped"
        return
    if de[0].edep>5:
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
    #edep=ROOT.TH1F("dedx","dE/dx",300,0,300)
    #edep.GetYaxis().SetTitle("dE/dx / 1/MeV")
    #edep.GetXaxis().SetTitle("E_{dep} / MeV")

    edep_vs_etot=ROOT.TH1F("edepvsetot","E_{dep} vs E_{kin}",300,0,300,300,0,300)
    edep_vs_etot.GetYaxis().SetTitle("E_{dep} / MeV")
    edep_vs_etot.GetXaxis().SetTitle("E_{kin} / MeV")

    etot_vs_z=ROOT.TH1F("ekin","E_{kin} vs z",500,0,500,300,0,300)
    etot_vs_z.GetYaxis().SetTitle("E_{kin} / MeV")
    etot_vs_z.GetXaxis().SetTitle("z / mm")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    
    calorhits=[]
    for event in calorimeter:
       edep_vs_etot.Fill(event.etot,event.edep)
       etot_vs_z.Fill(event.z,event.etot)
    edep_vs_etot.Write()
    etot_vs_z.Write()
    return
def main():
   for filename in sys.argv[1:]:
      doFile(filename)
main()
