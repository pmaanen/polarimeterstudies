#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot, atan2, asin
from copy import deepcopy

ROOT.gStyle.SetOptStat(0000000000000)
gStyle=ROOT.gStyle
gStyle.SetMarkerStyle(20)
gStyle.SetMarkerSize(2.5)
gStyle.SetLabelSize(.045,"xy")
gStyle.SetTitleSize(.05,"xy")
gStyle.SetTitleOffset(.8,"xy")

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
    histo=infile.Get(dirname+"/hEkin")
    if histo.GetEntries()<10000:
        return None,None,None
    histo.Scale(1./histo.GetEntries())
    histo.GetXaxis().SetRangeUser(20,300)
    maximum=histo.GetMaximum()
    maximumLoc=histo.GetXaxis().GetBinCenter(histo.GetMaximumBin())
    width=max(1/2.3548*getFWHM(histo),histo.GetXaxis().GetBinWidth(histo.GetMaximumBin()))
    histo.GetXaxis().SetRangeUser(max(0,maximumLoc-7*width),maximumLoc+7*width)
    print maximumLoc,width,getFWHM(histo)
    histo.GetXaxis().SetTitle("T_{d} [MeV]")
    histo.GetYaxis().SetTitle("rel. freq.")
    fitfunc=ROOT.TF1("Gaussian","[0]*TMath::Gaus(x,[1],[2])",maximumLoc-4*width,maximumLoc+4*width)
    fitfunc.SetParName(0,"Constant")
    fitfunc.SetParName(1,"Mean")
    fitfunc.SetParName(2,"Sigma")
    fitfunc.SetParameters(maximum,maximumLoc,width)
    fitfunc.SetNpx(1000)
    histo.Fit(fitfunc,"RQ")
    if fitfunc.GetChisquare()/fitfunc.GetNDF()>100:
        return None,None,None
    histo.SetTitle(dirname+" MeV")
    histo.Draw()
    c1.Print(dirname+"-ekin.pdf")
    e=fitfunc.GetParameter(1)
    de=fitfunc.GetParError(1)
    w=2.3548*fitfunc.GetParameter(2)
    return e,de,w
def doDetector(detector,hEkin,hVertex,hTheta):
    for hit in detector:
        x=hit.x
        y=hit.y
        r=hypot(x,y)
        hVertex.Fill(x,y)
        hTheta.Fill(atan2(r,hit.z)*90./asin(1))
        hEkin.Fill(hit.ekin)
def doCalorimeter(detector,hEdep):
    sum=0
    for hit in detector:
        sum+=hit.edep
    hEdep.Fill(sum)

def analysis(filename,myWorker):
    try:
        hVertex=ROOT.TH2F("hVertex","vertex coordinates",200,-10,10,200,-10,10)
        hEdep=ROOT.TH1F("hEdep","E_{dep}",600,0,300)
        hEkin=ROOT.TH1F("hEkin","E_{kin}",600,0,300)
        hTheta=ROOT.TH1F("hTheta","#Theta",180,0,90)
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            doDetector(event.TriggerR,hEkin,hVertex,hTheta)
            doCalorimeter(event.Right,hEdep)
        hVertex.Write()
        hTheta.Write()
        hEdep.Write()
        hEkin.Write()
        outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])
if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    #myAnalysis=exampleAnalysis()
    #myAnalysis.Init()
    #todo=myAnalysis(function=analysis)
    #while len(todo):
    #    todo=myAnalysis(function=analysis)
    infile=ROOT.TFile("E22EnergyLossInTarget.root")
    materials=["G4_C","G4_Al"]
    energies=[100,200,250,270]
    lengths=[5,10,20]
    iCol=0
    for material in materials:
        for e in energies:
            x=[]
            y=[]
            dy=[]
            w=[]
            dw=[]
            for d in lengths:
                iy,idy,iw=fitDir(infile,"G4_C-"+str(e)+"-"+str(d))
                if iy!=None:
                    x.append(d)
                    y.append(iy)
                    dy.append(idy)
                    w.append(iw)
            myGraph=ROOT.TGraph(len(x))
            for ix in range(len(x)):
                myGraph.SetPoint(ix,x[ix],y[ix])
            myGraph.SetLineColor(colors[iCol % len(colors)])
            myGraph.SetMarkerColor(colors[(iCol +1 )% len(colors)])
            myGraph.SetFillColor(colors[iCol % len(colors)])
            myGraph.SetLineWidth(3)
            myGraph.SetTitle("")
            myGraph.Draw("ALP")
            myGraph.GetXaxis().SetTitle("Target Thickness [mm]")
            myGraph.GetYaxis().SetTitle("T_{d} [MeV]")
            myGraph.Draw()
            c1.Print(material+str(e)+"-energy.pdf")
            for ix in range(len(x)):
                myGraph.SetPoint(ix,x[ix],w[ix])
            myGraph.GetXaxis().SetTitle("Target Thickness [mm]")
            myGraph.GetYaxis().SetTitle("T_{d} FWHM [MeV]")
            myGraph.Draw("ALP")
            c1.Print(material+str(e)+"-FWHM.pdf")
