#!/opt/local/bin/python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event

def doEvent(calo,de):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    calo.sort(key=lambda hit:hit.edep,reverse=True)
    if calo[0].detid in geometry and calo[0].edep>200:
        phihisto.Fill(geometry[calo[0].detid][0]+0.1)
        thetahisto.Fill(geometry[calo[0].detid][1]+0.1)
        phivsthetahisto.Fill(geometry[calo[0].detid][0]+0.1,geometry[calo[0].detid][1]+0.1)
    for calohit in calo:
        for dehit in de:
            if calohit.detid==dehit.detid:
                etothisto.Fill(calohit.edep+dehit.edep,dehit.edep)
                dehisto.Fill(calohit.edep,dehit.edep)
                
def doFile(filename):
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    trigger=infile.Get("Trigger")
    
    calorhits=[]
    triggerhits=[]
    print "reading hits into vectors"
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    for ihit in de:
        triggerhits.append(hit(ihit))
    print "sorting hits"
    calorhits.sort(key=lambda hit: hit.event)
    triggerhits.sort(key=lambda hit: hit.event)

        
    thisEventCalor=[]
    thisEventDe=[]
    iEvent=calorhits[-1].event
    while True:
        if calorhits[-1].event==iEvent:
            thisEventCalor.append(calorhits.pop())
        else:
            while True:
                if len(dehits)==0:
                    break
                if dehits[-1].event==iEvent:
                    thisEventDe.append(dehits.pop())
                else:
                    doEvent(thisEventCalor,thisEventDe,phihisto,thetahisto,anglehisto,dehisto_etot,dehisto_ecal,angle_dict)
                    thisEventCalor=[]
                    thisEventDe=[]
                    iEvent=min(calorhits[-1].event,dehits[-1].event)
                    break
        if len(calorhits)==0 or len(dehits)==0:
            break

        
files=[]
asym=[]
asymErr=[]
pol=[-0.5,-0.25,0.,0.25,0.5]
polErr=[0.,0.,0.,0.,0.]
files.append(doFile("dc_elastic_235_d050_merged.root"))
files.append(doFile("dc_elastic_235_d025_merged.root"))
files.append(doFile("dc_elastic_235_d000_merged.root"))
files.append(doFile("dc_elastic_235_u025_merged.root"))
files.append(doFile("dc_elastic_235_u050_merged.root"))
for a in files:
    asym.append(a[0])
    asymErr.append(a[1])

print asym
print asymErr


c1=ROOT.TCanvas("pol vs asym","pvsa",800,600)

graph=ROOT.TGraph(len(asym),asarray(pol),asarray(asym))
graph.Draw("AP")
c1.Print("c1.root")

