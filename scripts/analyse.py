#!/usr/bin/ipython
import ROOT
import numpy.random as npr
import sys

projrange=ROOT.TH1F("range","Projected Range",1000,0,100)
edep=ROOT.TH1F("dEdx","dEdx",1000,0,100)

class hit:
    def __init__(self,hit):
        self.trackId=hit.trackId
        self.event=hit.event
        self.edep=hit.edep
        self.x=hit.x
        self.y=hit.y
        self.z=hit.z
def FindPrimaryTrack(hits):
    track=[]
    for hit in hits:
        if hit.trackId==1:
           track.append(hit)
    return track

def SumEnergy(track):
    Edep=0
    for hit in track:
        Edep+=hit.Edep

def doEvent(event):
    PrimaryTrack=FindPrimaryTrack(event)
    if len(PrimaryTrack)<3:
        return
    prevZ=PrimaryTrack[0].z
    maxZ=PrimaryTrack[0].z
    for hit in PrimaryTrack[1:]:
        if hit.z>maxZ and hit.z>0:
            maxZ=hit.z
        rz=prevZ+(hit.z-prevZ)*npr.rand()
        prevZ=hit.z
        if(rz>0):
            edep.Fill(rz,hit.edep)
    projrange.Fill(maxZ)
    
def doFile(name):
    infile=ROOT.TFile(name)
    calor=infile.Get("Tracker")
    allhits=[]
    print "Sorting hits..."
    for iHit in calor:
        allhits.append(hit(iHit))
    allhits.sort(key=lambda hit: hit.event)
    thisEvent=[]
    iEvent=allhits[0].event
    thisEvent.append(allhits[0])
    for iHit in allhits[1:]:
        if iHit.event==iEvent:
            thisEvent.append(iHit)
        else:
            doEvent(thisEvent)
            if iEvent % 1000 == 0:
                print "Event ",iEvent," analyzed"
            iEvent=iHit.event
            thisEvent=[]
            thisEvent.append(hit(iHit))
    fac=iEvent*edep.GetBinWidth(0)
    edep.Scale(1/fac)
    projrange.Scale(1/float(iEvent))
    
    
doFile("run0.root")
