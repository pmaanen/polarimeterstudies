#!/usr/bin/env python
from ROOT import TFile, TH1F, TTree, TCanvas,gROOT, TGraphErrors, gRandom, TF1,gStyle
from ROOT.TMath import Gaus,Landau
from numpy import asarray

def asfloatarray(vec):
    return asarray(map(lambda x:float(x),vec))
def doFile(filename):
    histo=TH1F("edep","edep",1200,0,300)
    infile=TFile(filename)
    intree=infile.Get("sim")
    for event in intree:
        for hit in event.Absorber:
            histo.Fill(hit.edep)
    histo.Fit("gaus","Q")
    histo.Draw()
    c1=gROOT.FindObject("c1")
    c1.Print(filename[:-5]+".pdf")
    gaus=gROOT.FindObject("gaus")
    ndf=gaus.GetNDF()
    chi=9999
    if ndf>0:
        chi=gaus.GetChisquare()/ndf
    return gaus.GetParameter(1)+3*gaus.GetParameter(2),gROOT.FindObject("gaus").GetParError(1),chi

def main():
    gStyle.SetOptStat(000000)
    gStyle.SetOptFit(1)
    thick=range(0,55,5)
    edep=[]
    x=[]
    sigma_edep=[]
    for thickness in thick:
        val,err,chi=doFile(str(thickness)+".root")
        if(chi<10):
            x.append(thickness)
            edep.append(270-val)
            sigma_edep.append(err)
    graph=TGraphErrors(len(edep),asfloatarray(edep),asfloatarray(x),asfloatarray(len(edep)*[0]),asfloatarray(len(edep)*[0]))
    graph.Draw("ALP")
    c1=gROOT.FindObject("c1")
    c1.Print("thickness.pdf")
    c1.Print("thickness.root")

        
if __name__=="__main__":
    main()
