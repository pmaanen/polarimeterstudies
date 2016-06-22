#!/usr/bin/env python
import ROOT,sys,numpy,math
c1=ROOT.TCanvas("*","*",1600,900)
colors=[ROOT.kBlack,ROOT.kBlue,ROOT.kRed,ROOT.kSpring,ROOT.kOrange,ROOT.kMagenta]
def getFWHM(histo):
     maximum=histo.GetMaximum()
     firstbin=histo.FindFirstBinAbove(.5*maximum)
     lastbin=histo.FindLastBinAbove(.5*maximum)
     first=histo.GetXaxis().GetBinCenter(firstbin)
     last=histo.GetXaxis().GetBinCenter(lastbin)
     return last-first
def doFile(infile,dirname):
    try:
        histo=infile.Get(dirname+"/range")
        maximum=histo.GetMaximum()
        maximumLoc=histo.GetXaxis().GetBinCenter(histo.GetMaximumBin())
        width=max(1/2.3548*getFWHM(histo),histo.GetXaxis().GetBinWidth(histo.GetMaximumBin()))
        histo=infile.Get(dirname+"/range")
        histo.GetXaxis().SetRangeUser(maximumLoc-10*width,maximumLoc+10*width)
        histo.GetXaxis().SetTitle("range [mm]")
        fitfunc=ROOT.TF1("Gaussian","[0]*TMath::Gaus(x,[1],[2])",maximumLoc-5*width,maximumLoc+5*width)
        fitfunc.SetParName(0,"Constant")
        fitfunc.SetParName(1,"Mean")
        fitfunc.SetParName(2,"Sigma")
        fitfunc.SetParameters(maximum,maximumLoc,width)
        fitfunc.SetNpx(1000)
        histo.Fit(fitfunc,"RQ")
        histo.Draw()
        c1.Print(dirname+"-range.pdf")
        val=fitfunc.GetParameter(1)
        err=fitfunc.GetParError(1)
        return val,err
    except:
        print "Problem analysing ",dirname
        raise

def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))

materials=["iron","tungsten","lead","plastic","lyso"]
Ekin=asfloatarray(range(100,300,50)+[270])
graphs=[]
allGraph=ROOT.TMultiGraph()
iCol=2
infile=ROOT.TFile("bragg.root")
for material in materials:
    values=[]
    errors=[]
    for iEkin in Ekin:
        val=0
        err=0
        dirname="deuteron-"+str(material)+"-"+str(int(iEkin))
        try:
            val,err=doFile(infile,dirname)
        except:
            print "Error analysing",infile
            raise
        values.append(val)
        errors.append(err)
    graph=ROOT.TGraphErrors(len(Ekin),Ekin,asfloatarray(values),asfloatarray(len(Ekin)*[0]),asfloatarray(errors))
    graph.SetName(material)
    graph.SetTitle(material)
    graph.GetXaxis().SetTitle("E_{kin} [MeV]")
    graph.GetYaxis().SetTitle("Range [mm]")
    graph.SetLineColor(colors[iCol % len(colors)])
    graph.SetMarkerStyle(24)
    graph.SetFillColor(colors[iCol % len(colors)])
    graph.SetLineWidth(3)
    iCol+=1
    allGraph.Add(graph)
    graph.Draw("ALP")
    c1.Print(material+"-range.pdf")
    c1.Print(material+"-range.root")
    c1.Print(material+"-range.C")
c1.Clear()
allGraph.Draw("ALP")
allGraph.GetXaxis().SetTitle("E_{kin} [MeV]")
allGraph.GetYaxis().SetTitle("Range [mm]")
c1.BuildLegend(0.4,0.67,0.65,0.88)
ROOT.gPad.Modified()
c1.Print("range.pdf")
c1.Print("range.root")
c1.Print("range.C")
