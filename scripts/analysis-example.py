#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from copy import deepcopy
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return
        
def analysis(filename,myWorker):
    histogram=ROOT.TH1F("analysis","analysis",100,0,100)
    outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
    dir=outfile.mkdir(filename[:-5])
    dir.cd()
    for _ in range(1000):
        histogram.Fill(randint(100))
    histogram.Write()
    outfile.Write()
    outfile.Close()
    try:
        if randint(10) %2 == 0 or True:
            return (filename[:-5])
        else:
            raise
    except:
        print myWorker.name,"did not finish",filename
        return None
if __name__=="__main__":
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
