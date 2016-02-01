#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys
range=ROOT.TH1F("Range","range",5000,0,500)
xhist=ROOT.TH1F("x","lateral shower size x",5000,-250,250)
yhist=ROOT.TH1F("y","lateral shower size y",5000,-250,250)
dedx=ROOT.TH1F("dedx","dE/dx",5000,0,500)
edep_vs_etot=ROOT.TH2F("edepvsetot","E_{dep} vs E_{kin}",3000,0,300,3000,0,300)
edep_vs_etot.GetYaxis().SetTitle("E_{dep} / MeV")
edep_vs_etot.GetXaxis().SetTitle("E_{kin} / MeV")
etot_vs_z=ROOT.TH2F("ekin","E_{kin} vs z",5000,0,500,3000,0,300)
etot_vs_z.GetYaxis().SetTitle("E_{kin} / MeV")
etot_vs_z.GetXaxis().SetTitle("z / mm")

class hit:
    def __init__(self,hit):
        #self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event
        self.trackId=hit.trackId
        self.x=hit.x
        self.y=hit.y
        self.z=hit.z
        self.time=hit.time
        self.etot=hit.etot
        self.particleId=hit.particleId
def unpack(tree):
    res=[]
    for evt in tree:
        res.append(hit(evt))
    res.sort(key=lambda evt:evt.event)
    return res
def doEvent(calo):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    primaryTrack=sorted(filter(lambda hit:hit.trackId==1,calo),key=lambda hit:hit.time)
    protonTrack=sorted(filter(lambda hit:hit.particleId==2212,calo),key=lambda hit:hit.trackId)
    
    for hit in primaryTrack:
        edep_vs_etot.Fill(hit.etot,hit.edep)
        etot_vs_z.Fill(hit.z,hit.etot)
        dedx.Fill(hit.z,hit.edep)
    if len(protonTrack)==0:
        range.Fill(primaryTrack[-1].z)
        xhist.Fill(primaryTrack[-1].x)
        yhist.Fill(primaryTrack[-1].y)
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
    range.Reset()
    dedx.Reset()
    edep_vs_etot.Reset()
    etot_vs_z.Reset()
    xhist.Reset()
    yhist.Reset()
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    events=unpack(calorimeter)
    while True:
        iEvent=events[-1].event
        thisEventCalor=getOneEvent(iEvent,events)
        doEvent(thisEventCalor)
        if len(events)==0:
            break
        
    temp=etot_vs_z.Clone()
    temp.RebinX(10)
    temp2=edep_vs_etot.Clone()
    temp2.RebinX(10)
    profile=temp.ProfileX()
    profile.Write()
    profile2=temp2.ProfileX()
    profile2.Write()
    dedx.Write()
    edep_vs_etot.Write()
    etot_vs_z.Write()
    range.Write()
    xhist.Write()
    yhist.Write()
    return
def main():
    for filename in sys.argv[1:]:
        print "analysing:",filename
        doFile(filename)
main()
