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

def doEvent(calo,de,histo,Ekin):
    if len(calo)==0:
        print "no hits, event skipped"
        return
    
    if len(calo)!=1 or len(de)!=1 or calo[0].event!=de[0].event:
    #    print "malformed event, event skipped"
        return
    if de[0].edep>5 or calo[0].edep>Ekin-15:
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
    
def doFile(filename,Ekin):
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
        doEvent(thisEventCalor,thisEventDe,edep,Ekin)
        if len(calorhits)==0 or len(triggerhits)==0:
            break
    edep.Write()
    #fitfunc = ROOT.TF1('fitfunc','gaus(0)',0,300)
    #edep.Fit(fitfunc)
    #result=0
    #return fitfunc.GetParameter(1)
    return point(edep.GetMean(),edep.GetMeanError())

def asfloat(vec=[]):
   return asarray(map(lambda elm:float(elm)*1.0,vec))

def getDerivative(x=[],y=[]):
    res=[]
    if len(x)!=len(y):
       print "Error!"
       return
    for i in range(len(x[1:])):
        dx=x[i+1]-x[i]
        dy=y[i+1]-y[i]
        res.append(dy/dx)
    return asfloat(map(lambda x: max(0,x),res))
    
def main():
    Leff=range(30,100,5)
    Ekin=range(100,350,50)
    for iEkin in Ekin: 
      edep_mean=[]
      edep_err=[]
      for iLeff in sys.argv[1:]:
        filename="deuterons_"+str(iLeff)+"_"+str(iEkin)+".root"
        try:
           
            res=doFile(filename,iEkin)
            edep_mean.append(res.value)
            edep_err.append(res.error)
        except:
            print "Problem analyzing ",filename,":"
            raise
      edep=ROOT.TGraphErrors(len(edep_mean),asfloat(Leff),asfloat(edep_mean),asfloat(len(edep_mean)*[0]),asfloat(edep_err))
      edep.SetTitle("")
      der=ROOT.TGraph(len(edep_mean)-1,asfloat(Leff[1:]),getDerivative(Leff,edep_mean))
      der.SetTitle("")
      c1=ROOT.TCanvas(str(iEkin)+"MeV",str(iEkin)+" MeV",800,600)
      c1.Divide(2,1)
      c1.cd(1)
      edep.Draw("AP")
      edep.GetXaxis().SetTitle("L_eff [mm]")
      edep.GetYaxis().SetTitle("E_{dep} [MeV]")
      edep.SetMarkerStyle(20)
      c1.cd(2)
      der.Draw("AP")
      der.GetXaxis().SetTitle("L_eff [mm]")
      der.GetYaxis().SetTitle("#Delta E/#Delta L [MeV/mm]")
      der.SetMarkerStyle(20)
        #c1.Print(str(Ekin)+".root")
      c1.Print(str(iEkin)+".pdf")
main()
