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

Ekin=range(50,350,50)+[270]
for particle in ["deuteron","proton"]:
    for material in ["plastic","lyso","iron","lead","alu"]:
        size=[]
        size_err=[]
        for iEkin in Ekin:
            infile=ROOT.TFile(particle+"-"+material+"-"+str(iEkin)+"-histos.root")
            histo=infile.Get("Range")
            histo.GetXaxis().SetRangeUser(-4*histo.GetRMS()+histo.GetMean(),4*histo.GetRMS()+histo.GetMean())
            histo.GetXaxis().SetTitle("range / mm")
            fitfunc=ROOT.TF1("Gaussian",Gaus(),-2*histo.GetRMS()+histo.GetMean(),histo.GetMean()+2*histo.GetRMS(),3)
            fitfunc.SetParameters(histo.GetMaximum(),histo.GetMean(),histo.GetRMS())
            fitfunc.SetParName(0,"Constant")
            fitfunc.SetParName(1,"Mean")
            fitfunc.SetParName(2,"Sigma")
            fitfunc.SetNpx(1000)
            fitfunc.SetParameter(0,histo.GetMaximum())
            fitfunc.SetParameter(1,histo.GetMean())
            fitfunc.SetParameter(2,histo.GetRMS())
            histo.Fit(fitfunc,"RQ")
            histo.Draw()
            c1.Print(infile.GetName()[:-5]+"-range.pdf")
            size.append(fitfunc.GetParameter(1))
            size_err.append(fitfunc.GetParError(1))
        graph=ROOT.TGraphErrors(len(Ekin),asfloatarray(Ekin),asfloatarray(size),asfloatarray(len(Ekin)*[0]),asfloatarray(size_err))
        graph.SetTitle("")
        graph.GetXaxis().SetTitle("T_{d} [MeV]")
        graph.GetYaxis().SetTitle("Range [mm]")
    #graph.Fit("pol2","QR")
        graph.Draw("ALP")
        graph.SetLineWidth(2)
        from ROOT import kRed
        graph.SetLineColor(kRed)
        c1.Print(material+"-range.pdf")
#for histo in histos:
    #histo.Fit("Gaus")
    #histo.Draw()
    #histo.GetXaxis.SetRangeUser(-50,50)
    #histo.SetLineColor(iCol)
    #iCol+=2
#c1.Print("lat.pdf")
