#!/usr/bin/ipython
import ROOT

class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event
def doEvent(calo,de,histo):
    for calorhit in calo:
        for dehit in de:
            if calorhit.detid==dehit.detid and calorhit.edep>1 and dehit.edep>1:
                histo.Fill(calorhit.edep+dehit.edep,dehit.edep)

def doFile(filename):
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    de=infile.Get("dE")
    
    calorhits=[]
    dehits=[]
    print "reading hits into vectors"
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    for ihit in de:
        dehits.append(hit(ihit))
    print "sorting hits"
    calorhits.sort(key=lambda hit: hit.detid)
    dehits.sort(key=lambda hit: hit.detid)
    
    histo=ROOT.TH2F("deltaEvsE","delta E vs E",200,0,200,100,0,100)
    

    
    thisEventCalor=[]
    thisEventDe=[]
    iEvent=calorhits[0]
    while True:
        while True:
            if len(calorhits)==0:
                break
            if calorhits[0].event==iEvent:
                thisEventCalor.append(calorhits.pop(0))
            else:
                break
        while True:
            if len(dehits)==0:
                break
            if dehits[0].event==iEvent:
                thisEventDe.append(dehits.pop(0))
            else:
                break
        doEvent(thisEventCalor,thisEventDe,histo)
        thisEventCalor=[]
        thisEventDe=[]
        if len(calorhits)==0 or len(dehits)==0:
            break
        iEvent=min(calorhits[0].event,dehits[0].event)
    histo.Write()
doFile("dc_merged.root")
