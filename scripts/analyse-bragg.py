#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys

class braggAnalysis(AnalysisBase):
    def Init(self):
        outfile=ROOT.TFile(argv[1]+"-histos.root","RECREATE")
        return
    def BeginWorker(self,filename):
        try:
            self.range.Reset()
            self.dedx.Reset()
            self.edep_vs_etot.Reset()
            self.etot_vs_z.Reset()
            self.xhist.Reset()
            self.yhist.Reset()
        except:
            self.range=ROOT.TH1F("Range","range",5000,0,500)
            self.xhist=ROOT.TH1F("x","lateral shower size x",5000,-250,250)
            self.yhist=ROOT.TH1F("y","lateral shower size y",5000,-250,250)
            self.dedx=ROOT.TH1F("dedx","dE/dx",5000,0,500)
            self.edep_vs_etot=ROOT.TH2F("edepvsetot","E_{dep} vs E_{kin}",3000,0,300,3000,0,300)
            self.edep_vs_etot.GetYaxis().SetTitle("E_{dep} / MeV")
            self.edep_vs_etot.GetXaxis().SetTitle("E_{kin} / MeV")
            self.etot_vs_z=ROOT.TH2F("ekin","E_{kin} vs z",5000,0,500,3000,0,300)
            self.etot_vs_z.GetYaxis().SetTitle("E_{kin} / MeV")
            self.etot_vs_z.GetXaxis().SetTitle("z / mm")
    def Process(self,filename):
        infile=ROOT.TFile(filename,"READ")
        calorimeter=infile.Get("Calorimeter")
        events=unpack(calorimeter,CaloHit)
        while True:
            iEvent=events[-1].event
            thisEventCalor=getOneEvent(iEvent,events)
            self.doEvent(thisEventCalor)
            if len(events)==0:
                break
        return
    def TerminateWorker(self,filename):
        temp=etot_vs_z.Clone()    
        temp.RebinX(10)           
        temp2=edep_vs_etot.Clone()
        temp2.RebinX(10)          
        profile=temp.ProfileX()           
        profile2=temp2.ProfileX()           
        self.done_queue.put((filename[:-5],[self.range,self.dedx,self.edep_vs_etot,self.etot_vs_z,self.xhist,self.yhist,profile,profile2]))
        return 
    def TerminateWorker(self,filename):       
        self.outfile.cd()
        while not self.done_queue.empty():
            item=self.done_queue.get()
            dir=self.outfile.mkdir(item[0])
            dir.cd()    
            for elm in item[1]:
                elm.Write()
        self.outfile.Write()
        self.outfile.Close()
        return
    def doEvent(self,calo):
        if len(calo)==0:
            print "no hits, event skipped"
            return
        primaryTrack=sorted(filter(lambda hit:hit.trackId==1,calo),key=lambda hit:hit.time)
        protonTrack=sorted(filter(lambda hit:hit.particleId==2212,calo),key=lambda hit:hit.trackId)
        for hit in primaryTrack:
             self.edep_vs_etot.Fill(hit.etot,hit.edep)
             self.etot_vs_z.Fill(hit.z,hit.etot)
             self.dedx.Fill(hit.z,hit.edep)
             if len(protonTrack)==0:
                 self.range.Fill(primaryTrack[-1].z)
                 self.xhist.Fill(primaryTrack[-1].x)
                 self.yhist.Fill(primaryTrack[-1].y)
        return
    
def main():
    bragg=braggAnalysis(24)
    bragg.AddFiles(sys.argv[2:])
    bragg()
main()