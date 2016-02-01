#!/usr/bin/env python
import ROOT,numpy
def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))
histos=[]
iCol=3
c1=ROOT.TCanvas("lateral","lateral displacement x",800,600)
ROOT.gStyle.SetOptStat("e")
ROOT.gStyle.SetOptFit(1)
class Gaus:
   def __call__( self, x, par ):
      return par[0]*ROOT.TMath.Gaus(x[0],par[1],par[2])
size=[]
size_err=[]
Ekin=range(100,350,50)
for iEkin in Ekin:
    infile=ROOT.TFile("bragg-lyso-"+str(iEkin)+".root")
    histo=infile.Get("x")
    histo.GetXaxis().SetRangeUser(-4*histo.GetRMS(),4*histo.GetRMS())
    histo.GetXaxis().SetTitle("lateral displacement / mm")
    fitfunc=ROOT.TF1("Gaussian",Gaus(),-20.,20.,3)
    fitfunc.SetParameters(histo.GetMaximum(),histo.GetMean(),histo.GetRMS())
    fitfunc.SetParName(0,"Constant")
    fitfunc.SetParName(1,"Mean")
    fitfunc.SetParName(2,"Sigma")
    fitfunc.SetNpx(1000)
    histo.Fit(fitfunc,"RQ")
    histo.Draw()
    c1.Print(infile.GetName()[:-5]+".pdf")
    size.append(fitfunc.GetParameter(2))
    size_err.append(fitfunc.GetParError(2))

graph=ROOT.TGraphErrors(len(Ekin),asfloatarray(Ekin),asfloatarray(size),asfloatarray(len(Ekin)*[0]),asfloatarray(size_err))
graph.SetTitle("Lateral displacement vs E_{d}")
graph.GetXaxis().SetTitle("E_{d} / MeV")
graph.GetYaxis().SetTitle("d / mm")
graph.Draw("ALP")

c1.Print("lyso-displacement.pdf")
#for histo in histos:
    #histo.Fit("Gaus")
    #histo.Draw()
    #histo.GetXaxis.SetRangeUser(-50,50)
    #histo.SetLineColor(iCol)
    #iCol+=2
#c1.Print("lat.pdf")
    
