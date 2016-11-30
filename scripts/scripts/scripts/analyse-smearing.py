#!/usr/bin/env python
import ROOT
import numpy.random

def main():
    c1=ROOT.TCanvas("c1","c1",800,600)
    allEvents=ROOT.TH1F("edep_all","edep_all",300,0,300)
    smearedEdep=ROOT.TH1F("edep_sm","edep_sm",500,0,500)
    infile=ROOT.TFile("test_deuterons.root","UPDATE")
    eff=ROOT.TEfficiency("eff","my efficiency;x;#epsilon",20,0,300)
    for event in infile.Get("Calorimeter"):
        allEvents.Fill(event.edep)
        smeared=event.edep*numpy.random.normal(1,0.3)
        smearedEdep.Fill(smeared)
        passed=False
        if smeared>200:
           passed=True
        eff.Fill(passed,event.edep)
    allEvents.Write()
    smearedEdep.Write()
    eff.Draw("AP")
    c1.Print("c1.pdf")
main()
