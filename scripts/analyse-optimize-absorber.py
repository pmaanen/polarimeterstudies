#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys

class optimizationAnalysis(AnalysisBase):
    def Init(self):
        self.outfile=ROOT.TFile("absorber.root","RECREATE")
        return
    def BeginWorker(self,filename):
        try:
            self.einit.Reset()
        except:
            self.einit=ROOT.TH1F("einit","initial energy",300,0,300)
    def Process(self,filename):
        infile=ROOT.TFile(filename,"READ")
        calorimeter=infile.Get("Calorimeter")
        events=unpack(calorimeter)
        while True:
            iEvent=events[-1].event
            thisEventCalor=getOneEvent(iEvent,events)
            self.doEvent(thisEventCalor)
            if len(events)==0:
                break
        return
    def TerminateWorker(self,filename):
        self.done_queue.put((filename[:-5],[self.einit]))
        return 
    def Terminate(self):       
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
        if len(primaryTrack)!=0:
            self.einit.Fill(primaryTrack[0].etot)
        return
    
def main():
    myAnalysis=optimizationAnalysis(3)
    myAnalysis.AddFiles(sys.argv[2:])
    myAnalysis()
main()
