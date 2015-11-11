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
    #edep=ROOT.TH2F("dedx","dE/dx",300,0,300,5000,0,500)
    #edep.GetYaxis().SetTitle("dE/dx / 1/MeV")
    #edep.GetXaxis().SetTitle("z / mm")
    dedx=ROOT.TH1F("dedx","dE/dx",5000,0,500)
    edep_vs_etot=ROOT.TH2F("edepvsetot","E_{dep} vs E_{kin}",3000,0,300,3000,0,300)
    edep_vs_etot.GetYaxis().SetTitle("E_{dep} / MeV")
    edep_vs_etot.GetXaxis().SetTitle("E_{kin} / MeV")

    etot_vs_z=ROOT.TH2F("ekin","E_{kin} vs z",5000,0,500,3000,0,300)
    etot_vs_z.GetYaxis().SetTitle("E_{kin} / MeV")
    etot_vs_z.GetXaxis().SetTitle("z / mm")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    
    calorhits=[]
    lasthit=[0,0,0]
    for event in calorimeter:
        if event.trackId==1:
            edep_vs_etot.Fill(event.etot,event.edep)
            etot_vs_z.Fill(event.z,event.etot)
            #if(lasthit!=[0,0,0]):
            #    dx=(lasthit[0]-event.x)
            #    dy=(lasthit[1]-event.y)
            #    dz=(lasthit[2]-event.z)
            #    ds=sqrt(dx**2+dy**2+dz**2)
            #    if(ds!=0):
            #        deds=event.edep/ds
            #        edep.Fill(deds,event.z)
            #lasthit=[event.x,event.y,event.z]
            dedx.Fill(event.z,event.edep)
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
    return
def main():
    for filename in sys.argv[1:]:
        print "analysing:",filename
        doFile(filename)
main()
