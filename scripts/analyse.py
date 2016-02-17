#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys


<<<<<<< HEAD
class Gaus:
    def __call__( self, x, p ):
        p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

=======
>>>>>>> master
class hit:
    def __init__(self,hit):
        self.detid=hit.detid
        self.edep=hit.edep
        self.event=hit.event

def doEvent(calo,de,histo,Ekin):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    
    if len(calo)!=1 or len(de)!=1 or calo[0].event!=de[0].event:
    #    print "malformed event, event skipped"
        return
    if de[0].edep>.5:
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

class point:
   def __init__(self, x, xerr):
      self.value=x
      self.error=xerr
    
<<<<<<< HEAD
def doFile(filename,Ekin):
    edep=ROOT.TH1F("edep","edep after trigger",300,0,300)
    edep.GetYaxis().SetTitle("dN/dE_{dep} / 1/MeV")
=======
def doFile(filename):
    edep=ROOT.TH1F("dedx","dE/dx",300,0,300)
    edep.GetYaxis().SetTitle("dE/dx / 1/MeV")
>>>>>>> master
    edep.GetXaxis().SetTitle("E_{dep} / MeV")

    edep_vs_etot=ROOT.TH2F("edepvsetot","E_{dep} vs E_{kin}",3000,0,300,3000,0,300)
    edep_vs_etot.GetYaxis().SetTitle("E_{dep} / MeV")
    edep_vs_etot.GetXaxis().SetTitle("E_{kin} / MeV")

    etot_vs_z=ROOT.TH2F("ekin","E_{kin} vs z",5000,0,500,3000,0,300)
    etot_vs_z.GetYaxis().SetTitle("E_{kin} / MeV")
    etot_vs_z.GetXaxis().SetTitle("z / mm")
    infile=ROOT.TFile(filename,"UPDATE")
    calorimeter=infile.Get("Calorimeter")
    
    calorhits=[]
<<<<<<< HEAD
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
        doEvent(thisEventCalor,thisEventDe,edep,Ekin)
        if len(calorhits)==0 or len(triggerhits)==0:
            break
    edep.Write()
    fitfunc = ROOT.TF1('fitfunc','gaus(0)',0,300)
    if(edep.GetEntries()<100):
        return None
    fitfunc.SetParameter(0,edep.GetMaximum())
    fitfunc.SetParameter(1,edep.GetMaximumBin())
    fitfunc.SetParameter(2,10)
    edep.Fit(fitfunc,"Q")
    return point(fitfunc.GetParameter(1),fitfunc.GetParameter(2))

def asfloat(vec=[]):
   return asarray(map(lambda elm:float(elm)*1.0,vec))

def getDerivative(x=[],y=[],yerr=[],geterrors=False):
    res=[]
    err=[]
    if len(x)!=len(y):
        print "Error!"
        return
    if yerr!=[] and len(yerr)!=len(x):
        print "Error!"
        return
    for i in range(len(x[1:])):
        dx=x[i+1]-x[i]
        dy=y[i+1]-y[i]
        res.append(dy/dx)
    for i in range(len(yerr[1:])):
        err.append(1/(x[i+1]-x[i])*sqrt(yerr[i+1]**2+yerr[i]**2))
    return [asfloat(res),asfloat(err)]
    
def main():
    Leff=range(30,100,5)
    Ekin=range(100,350,50)
    for iEkin in Ekin: 
        edep_mean=[]
        edep_err=[]
        thisLeff=[]
        for iLeff in Leff:
            filename="deuterons_"+str(iLeff)+"_"+str(iEkin)+".root"
            try:         
                res=doFile(filename,iEkin)
                if(res!=None):
                    edep_mean.append(res.value)
                    edep_err.append(res.error)
                    thisLeff.append(iLeff)
            except:
                print "Problem analyzing ",filename,":"
                raise
        if len(thisLeff)>1:
            edep=ROOT.TGraphErrors(len(edep_mean),asfloat(thisLeff),asfloat(edep_mean),asfloat(len(edep_mean)*[0]),asfloat(edep_err))
            edep.SetTitle("")
            der=ROOT.TGraphErrors(len(edep_mean)-1,asfloat(Leff[1:]),getDerivative(thisLeff,edep_mean,edep_err)[0],asfloat((len(edep_mean)-1)*[0]),getDerivative(thisLeff,edep_mean,edep_err)[1])
            der.SetTitle("")
            c1=ROOT.TCanvas(str(iEkin)+"MeV",str(iEkin)+" MeV",800,600)
            c1.Divide(2,1)
            c1.cd(1)
            edep.Draw("AP")
            edep.GetXaxis().SetTitle("L_{eff} [mm]")
            edep.GetYaxis().SetTitle("E_{dep} [MeV]")
            edep.SetMarkerStyle(20)
            c1.cd(2)
            der.Draw("AP")
            der.GetXaxis().SetTitle("L_{eff} [mm]")
            der.GetYaxis().SetTitle("#Delta E/#Delta L [MeV/mm]")
            der.SetMarkerStyle(20)
            c1.Print(str(iEkin)+".root")
            c1.Print(str(iEkin)+".pdf")
=======
    lasthit=[0,0,0]
    for event in calorimeter:
        if event.trackId==1:
            edep_vs_etot.Fill(event.etot,event.edep)
            etot_vs_z.Fill(event.z,event.etot)
            if(lasthit!=[0,0,0]):
                dx=(lasthit[0]-event.x)
                dy=(lasthit[1]-event.y)
                dz=(lasthit[2]-event.z)
                ds=sqrt(dx**2+dy**2+dz**2)
                if(ds!=0):
                    deds=event.edep/ds
                    edep.Fill(deds)
    temp=etot_vs_z.Clone()
    temp.RebinX(10)
    profile=temp.ProfileX()
    profile.Write()
    edep_vs_etot.Write()
    etot_vs_z.Write()
    return
def main():
    for filename in sys.argv[1:]:
        print "analysing:",filename
        doFile(filename)
>>>>>>> master
main()
    
