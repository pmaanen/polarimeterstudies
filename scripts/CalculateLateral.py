#!/usr/bin/env python
import ROOT,sys,numpy,math
c1=ROOT.TCanvas("*","*",1600,900)
colors=[ROOT.kBlue,ROOT.kRed,ROOT.kSpring,ROOT.kOrange,ROOT.kMagenta]
def getFWHM(histo):
     maximum=histo.GetMaximum()
     firstbin=histo.FindFirstBinAbove(.5*maximum)
     lastbin=histo.FindLastBinAbove(.5*maximum)
     first=histo.GetXaxis().GetBinCenter(firstbin)
     last=histo.GetXaxis().GetBinCenter(lastbin)
     return last-first
def getIntegral(histo,threshold):
    zero=histo.GetXaxis().FindBin(0)
    for cut in range(zero-1,histo.GetXaxis().GetLast()):
        cut_neg=histo.GetXaxis().FindBin(-histo.GetXaxis().GetBinCenter(cut))
        sum=0
        for ii in range(cut_neg,cut):
            sum+=histo.GetBinContent(ii)
        if sum>threshold*histo.GetEntries():
            print histo.GetXaxis().GetBinCenter(cut),getFWHM(histo)
            return histo.GetXaxis().GetBinCenter(cut)
    print "no cut found!"
        
def doFile(infile,dirname):
    try:
        histo=infile.Get(dirname+"/x")
        histo.Draw()
        c1.Print(dirname+"-x.pdf")
        return getIntegral(histo,0.9),0
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
    graph.GetYaxis().SetTitle("x displacement (90% inside) [mm]")
    graph.SetLineColor(colors[iCol % len(colors)])
    graph.SetMarkerStyle(24)
    graph.SetFillColor(colors[iCol % len(colors)])
    graph.SetLineWidth(3)
    iCol+=1
    allGraph.Add(graph)
    graph.Draw("ALP")
    c1.Print(material+"-x.pdf")
    c1.Print(material+"-x.root")
    c1.Print(material+"-x.C")
c1.Clear()
allGraph.Draw("ALP")
allGraph.GetXaxis().SetTitle("E_{kin} [MeV]")
allGraph.GetYaxis().SetTitle("x displacement (RMS) [mm]")
c1.BuildLegend(0.4,0.67,0.65,0.88)
ROOT.gPad.Modified()
c1.Print("thickness.pdf")
c1.Print("thickness.root")
c1.Print("thickness.C")
