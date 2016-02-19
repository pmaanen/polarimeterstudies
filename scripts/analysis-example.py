#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
          
class exampleAnalysis(AnalysisBase):
    def Init(self):
        self.outfile=ROOT.TFile(self.args.output,"RECREATE")
        return
    def BeginWorker(self,filename):
        try:
            self.result.Reset()
        except:
            self.result=ROOT.TH1F("pt","pt",300,-15,15)

            
    def Process(self,filename):
        mean=randint(12)-6
        sigma=float(randint(999))/float(100)+0.001
        for i in range(randint(100000)):
            self.result.Fill(normal(mean,sigma))
        return
    
    def TerminateWorker(self,filename):
        self.done_queue.put((filename[:-5],[self.result]))
        return 
    
    def Terminate(self):
        self.outfile.cd()
        while not self.done_queue.empty():
            item=self.done_queue.get()
            dir=self.outfile.mkdir(item[0])
            dir.cd()    
            for elm in item[1]:
                elm.Write()
        #elm.Write()
        self.outfile.Write()
        self.outfile.Close()
        

def main():
    myAnalysis=exampleAnalysis()
    myAnalysis()
    
if __name__=="__main__":
    main()