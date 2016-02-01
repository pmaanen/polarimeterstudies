#!/usr/bin/env python
import ROOT
histos=[]
iCol=3
for iEkin in range(100,350,50):
    infile=ROOT.TFile("bragg_lyso_"+str(iEkin)+".root")
    histos.append(infile.Get("x").Clone())
c1=ROOT.TCanvas("lateral","lateral shower size",800,600)
for histo in histos:
#    histo.Fit("Gaus")
    histo.Draw()
    histo.GetXaxis.SetRangeUser(-50,50)
    histo.SetLineColor(iCol)
    iCol+=2
c1.Print("lat.pdf")
    
