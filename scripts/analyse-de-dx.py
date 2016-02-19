#!/usr/bin/env python
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys


class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

"""
def main():
    edep_mean=[]
    Leff=range(30,70,5)
    for iLeff in Leff:
        filename="deuterons_"+str(iLeff)+".root"
        print "analysing "+str(filename)
        edep_mean.append(doFile(filename))
    for i in range(len(Leff)):
        Leff[i]=float(Leff[i])*1.0
    myGraph=ROOT.TGraph(len(edep_mean),asarray(Leff),asarray(edep_mean))
    myGraph.Print()
    c1=ROOT.TCanvas("c1","c1",800,600)
    myGraph.Draw("AP")
    c1.Print("c1.root")
main()
"""
from AnalysisBase import *
import ROOT
import sys


          
class dedxAnalysis(AnalysisBase):
    def Init(self):
        self.outfile=ROOT.TFile(self.args.output,"RECREATE")
        return
    def BeginWorker(self,filename):
        try:
            self.edep.Reset()
        except:
            self.edep=ROOT.TH1F("edep","edep after trigger",300,0,300)
            self.edep.GetYaxis().SetTitle("dN/dE_{dep} / 1/MeV")
            self.edep.GetXaxis().SetTitle("E_{dep} / MeV")

            
    def Process(self,filename):
        infile=ROOT.TFile(filename,"UPDATE")
        calorimeter=infile.Get("Calorimeter")
        trigger=infile.Get("Trigger")
    
        calorhits=[]
        triggerhits=[]
        calorhits=unpack(calorimeter,CaloHit)
        triggerhits=unpack(trigger,CaloHit)
        thisEventCalor=[]
        thisEventDe=[]
        while True:
            iEvent=calorhits[-1].event
            thisEventCalor=getOneEvent(iEvent,calorhits)
            thisEventDe=getOneEvent(iEvent,triggerhits)
            self.doEvent(thisEventCalor,thisEventDe,edep)
            if len(calorhits)==0 or len(triggerhits)==0:
                break
        return
    
    def TerminateWorker(self,filename):
        self.done_queue.put((filename[:-5],[self.edep]))
        return 
      
    def doEvent(self,calo,de):
        if len(calo)==0:
            return
        if len(calo)!=1 or len(de)!=1 or calo[0].event!=de[0].event:
    #    print "malformed event, event skipped"
            return
        if de[0].edep>5:
            self.edep.Fill(calo[0].edep)

def main():
    myAnalysis=dedxAnalysis(3)
    myAnalysis.AddFiles(sys.argv[2:])
    myAnalysis()
    
if __name__=="__main__":
    main()