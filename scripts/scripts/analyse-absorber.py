#!/usr/bin/env python
from AnalysisBase import *
from ROOT import TH1F
import sys
class absorberAnalysis(AnalysisBase):
    def Init(self):
        self.outfile=ROOT.TFile(self.args.output,"RECREATE")
        return
    def BeginWorker(self,filename):
        try:
            self.einit.Reset()
        except:
            self.einit=TH1F("einit","initial kinetic energy",300,0,300)
            self.einit.GetXaxis.SetTitle("T_{d,init} [MeV]")
            self.einit.GetYaxis().SetTitle("#")
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
    def doEvent(self,calo):
        if len(calo)==0:
            return
        primaryTrack=sorted(filter(lambda hit:hit.trackId==1,calo),key=lambda hit:hit.time)
        self.einit.Fill(primaryTrack[0])
        return

def main():
    myAnalysis=absorberAnalysis()
    myAnalysis()
    
if __name__=="__main__":
    main()
