#!/opt/local/bin/python
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
    edep=ROOT.TH1F("edep","edep after trigger",300,0,300)
    edep.GetYaxis().SetTitle("dN/dE_{dep} / 1/MeV")
    edep.GetXaxis().SetTitle("E_{dep} / MeV")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    trigger=infile.Get("Trigger")
    
    calorhits=[]
    triggerhits=[]
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    for ihit in trigger:
        triggerhits.append(hit(ihit))
    calorhits.sort(key=lambda hit: hit.event)
    triggerhits.sort(key=lambda hit: hit.event)

        
    thisEventCalor=[]
    thisEventDe=[]
    while True:
        iEvent=calorhits[-1].event
        thisEventCalor=getOneEvent(iEvent,calorhits)
        thisEventDe=getOneEvent(iEvent,triggerhits)
        doEvent(thisEventCalor,thisEventDe,edep)
        if len(calorhits)==0 or len(triggerhits)==0:
            break
    edep.Write()
def main():
    for filename in sys.argv[1:]:
        print "analysing "+str(filename)
        doFile(filename)
main()
