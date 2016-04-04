#!/usr/bin/env ipython
from ROOT import TH2F,TCanvas,TTree,TFile

Ekin=range(50,300,1)
scatterplot=TH2F("EdepVsTd","E_{dep} vs T_{d}",30,0,300,30,0,30)
for iEkin in Ekin:
    infile=TFile(str(iEkin)+".root","READ")
    for event in infile.Calorimeter:
        scatterplot.Fill(iEkin,event.edep)
c1=TCanvas("plot","plot",1200,800)
scatterplot.Draw()
c1.Print("c1.pdf")
