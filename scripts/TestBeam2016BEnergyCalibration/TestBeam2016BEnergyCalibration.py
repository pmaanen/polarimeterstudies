#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot
from copy import deepcopy

ROOT.gStyle.SetOptStat(0000000000000)
gStyle=ROOT.gStyle
gStyle.SetMarkerStyle(20)
gStyle.SetMarkerSize(2.5)
gStyle.SetLabelSize(.045,"xy")
gStyle.SetTitleSize(.05,"xy")
gStyle.SetTitleOffset(1,"xy")
gStyle.SetOptFit(1)

class exampleAnalysis(AnalysisBase):
    def Init(self):
        return

class Gaus:
   def __call__( self, x, p ):
       p[0]*ROOT.TMath.Gaus(x[0],p[1],p[2])

colors=[ROOT.kBlue,ROOT.kRed,ROOT.kYellow-3,ROOT.kSpring,ROOT.kOrange,ROOT.kMagenta]

def getFWHM(histo):
    maximum=histo.GetMaximum()
    firstbin=histo.FindFirstBinAbove(.5*maximum)
    lastbin=histo.FindLastBinAbove(.5*maximum)
    first=histo.GetXaxis().GetBinCenter(firstbin)
    last=histo.GetXaxis().GetBinCenter(lastbin)
    return last-first

def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))

c1=ROOT.TCanvas("graph","graph",1600,900)
def fitDir(infile,dirname):
    histo=infile.Get(dirname+"/hEdep").Clone()
    histo.SetName("hEdep-Zoomed")
    infile.cd(dirname)
    if histo.GetEntries()<1000:
        return None,None,None
    histo.Scale(1./histo.GetEntries())
    histo.GetXaxis().SetRangeUser(20,300)
    maximum=histo.GetMaximum()
    maximumLoc=histo.GetXaxis().GetBinCenter(histo.GetMaximumBin())
    width=max(1/2.3548*getFWHM(histo),histo.GetXaxis().GetBinWidth(histo.GetMaximumBin()))
    histo.GetXaxis().SetRangeUser(max(0,maximumLoc-5*width),maximumLoc+5*width)
    histo.GetYaxis().SetRangeUser(0,1.2*maximum)
    histo.GetXaxis().SetTitle("E_{dep} [MeV]")
    histo.GetYaxis().SetTitle("rel. freq.")
    fitfunc=ROOT.TF1("Gaussian","[0]*TMath::Gaus(x,[1],[2])",maximumLoc-4*width,maximumLoc+4*width)
    fitfunc.SetParName(0,"Constant")
    fitfunc.SetParName(1,"Mean")
    fitfunc.SetParName(2,"Sigma")
    fitfunc.SetParameters(maximum,maximumLoc,width)
    fitfunc.SetNpx(1000)
    histo.Fit(fitfunc,"RQ")
    if fitfunc.GetChisquare()/fitfunc.GetNDF()>1e6:
        return None,None,None
    histo.SetTitle(dirname+" MeV")
    histo.Draw()
    histo.Write()
    c1.Print(dirname+"-ekin.pdf")
    e=fitfunc.GetParameter(1)
    de=fitfunc.GetParError(1)
    w=2.3548*fitfunc.GetParameter(2)
    return e,de,w

def makePlots(title,x,y,w):
        gEdep=ROOT.TGraph(len(x))
        gEdep.SetName(title[:-1])
        gEdep.SetTitle("")
        for ix in range(len(x)):
            gEdep.SetPoint(ix,x[ix],y[ix])
        gEdep.SetLineColor(colors[iCol % len(colors)])
        gEdep.SetMarkerColor(colors[(iCol +1 )% len(colors)])
        gEdep.SetFillColor(colors[iCol % len(colors)])
        gEdep.SetLineWidth(3)
        gEdep.SetTitle("")
        gEdep.Draw("ALP")
        gEdep.GetYaxis().SetTitle("E_{dep} [MeV]")
        gEdep.GetXaxis().SetTitle("T_{d} [MeV]")
        gEdep.Draw()
        c1.Print(title+"energy.pdf")
        #c1.Print(title+"energy.root")
        gEdep.Write()
        gFWHM=ROOT.TGraph(len(x))
        gFWHM.SetName(title[:-1])
        gFWHM.SetTitle(title[:-1])
        gFWHM.SetLineColor(colors[iCol % len(colors)])
        gFWHM.SetMarkerColor(colors[(iCol +1 )% len(colors)])
        gFWHM.SetFillColor(colors[iCol % len(colors)])
        gFWHM.SetLineWidth(3)
        for ix in range(len(x)):
            gFWHM.SetPoint(ix,x[ix],w[ix])
        gFWHM.GetXaxis().SetTitle("T_{d} [MeV]")
        gFWHM.GetYaxis().SetTitle("#Delta E (FWHM) [MeV]")
        gFWHM.Draw("ALP")
        c1.Print(title+"FWHM.pdf") 
        #c1.Print(title+"FWHM.root")
        gFWHM.Write()
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return
    
def analysis(filename,myWorker):
    try:
        hSum=ROOT.TH1F("hEdep","E_{dep}",3000,0,300)
        hSum.GetXaxis().SetTitle("E_{dep} [MeV]")
        hSum.GetYaxis().SetTitle("rel. freq.")
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            sum=0
            for hit in event.Detector:
                sum+=hit.edep
            if(sum>5.):
                hSum.Fill(sum)
        hSum.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])
    
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=exampleAnalysis()
    myAnalysis.Init()
    todo=myAnalysis(function=analysis)
    while len(todo):
        todo=myAnalysis(function=analysis)
    infile=ROOT.TFile(myAnalysis.args.output,"UPDATE")
    dir=infile.mkdir("plots")
    materials=["G4_GRAPHITE","G4_Al","G4_Mg","G4_AIR"]
    energies=[100,135,150,200,235,250,270]
    lengths=[5]
    iCol=0
    for material in materials:
        t=[]
        ed=[]
        w=[]
        for e in energies:
            ie,ide,iw=fitDir(infile, material+"-"+str(e)+"-5")
            if ie!=None:
                t.append(e)
                ed.append(ie)
                w.append(iw)
        dir.cd()
        makePlots(material+"5-",t,ed,w)       
    t=[]
    ed=[]
    w=[]
    for e in energies:
        ie,ide,iw=fitDir(infile, "G4_GRAPHITE-"+str(e)+"-10")
        if ie!=None:
            t.append(e)
            ed.append(ie)
            w.append(iw)
    dir.cd()
    makePlots("G4_GRAPHITE-10-",t,ed,w)
