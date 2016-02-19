#!/usr/bin/env python
from AnalysisBase import *
from ROOT import TH1F
import sys
class AnalysisAbsorber(AnalysisBase):
    def Init(self):
        return
    def Begin(self,filename):
        if not hasattribute(self,'einit'):
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
    def Terminate(self,filename):
        outfile=TFile(filename[:-5]+"-histos.root","RECREATE")
        outfile.cd()
        self.einit.Write()
        outfile.Write()
        outfile.Close()
    def doEvent(self,calo):
        if len(calo)==0:
            return
        primaryTrack=sorted(filter(lambda hit:hit.trackId==1,calo),key=lambda hit:hit.time)
        self.einit.Fill(primaryTrack[0])
        return

def main():
    myAnalysis=AnalysisAbsorber(4)
    for filename in sys.argv[1:]:
        myAnalysis.arguments.append(filename)
    myAnalysis()
main()
