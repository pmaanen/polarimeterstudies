#!/usr/bin/env python
import ROOT

graphs=ROOT.TFile("graphs.root")
profiles=ROOT.TFile("profiles.root")
Ekin=range(100,300,50)+[270]
c1=ROOT.TCanvas("_","_",1200,800)
for iEkin in Ekin:
    try:
        graph=graphs.Get("delta_"+str(iEkin))
        profile=profiles.Get(str(iEkin))
        graph.Draw("ALP")
        graph.SetMarkerStyle(24)
        graph.SetMarkerSize(1.2)
        profile.Draw("SAME")
        c1.Print("same_"+str(iEkin)+".pdf")
    except:
        continue
