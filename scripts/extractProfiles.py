#!/usr/bin/env ipython
import ROOT

Ekin=range(100,300,50)+[270]
infile=ROOT.TFile("bragg.root")
outfile=ROOT.TFile("profiles.root","RECREATE")
outfile.cd()
for iEkin in Ekin:
    name="deuteron-lyso-"+str(iEkin)+"/dEdz"
    iProf=infile.Get(name)
    iProf.GetXaxis().SetRangeUser(0,100)
    iProf.SetName(str(iEkin))
    iProf.SetTitle(str(iEkin))
    iProf.Write()

