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
def readGeom(geomfile):
    rawdata=genfromtxt(geomfile,skip_header=1)
    detIds=[]
    phis=[]
    theta=[]
    for row in rawdata:
        detIds.append(int(row[0]))
        phi=atan2(float(row[1]),float(row[2]))*180./3.1415
        if phi<0:
            phi+=360
        phis.append(phi)
        theta.append(atan2(hypot(row[1],row[2]),row[3])*180/3.1415)
    angles=zip(phis,theta)
    return dict(zip(detIds,angles))


def doEvent(calo,de,phihisto,thetahisto,phivsthetahisto,etothisto,dehisto,geometry):
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
    angle_dict=readGeom("geometry.dat")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    de=infile.Get("DeltaE")
    
    calorhits=[]
    dehits=[]
    print "reading hits into vectors"
    for ihit in calorimeter:
        calorhits.append(hit(ihit))
    for ihit in de:
        dehits.append(hit(ihit))
    print "sorting hits"
    calorhits.sort(key=lambda hit: hit.event)
    dehits.sort(key=lambda hit: hit.detid)
    phihisto=ROOT.TH1F("phi","phi",8,0,360)
    thetahisto=ROOT.TH1F("theta","theta",10,0,20)
    anglehisto=ROOT.TH2F("PhivsTheta","PhiVsTheta",8,0,360,10,0,20)
    dehisto_etot=ROOT.TH2F("E1vsEtot","E1 vs Etot",250,0,250,100,0,100)
    dehisto_ecal=ROOT.TH2F("E1vsE2","E1 vs E2",250,0,250,100,0,100)

        
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
    #while True:
    #    if de
    fitfunc=ROOT.TF1("fitfunc","[1]*(1+[0]*cos(3.1415/180.*x+[2]))",0,360)
    phihisto.Scale(1./phihisto.GetEntries())
    phihisto.Fit("fitfunc")
    phihisto.Write()
    thetahisto.Write()
    anglehisto.Write()
    dehisto_etot.Write()
    dehisto_ecal.Write()
    return [fitfunc.GetParameter(0),fitfunc.GetParError(0)]
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

