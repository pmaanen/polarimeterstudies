#!/usr/bin/env python
import ROOT,sys,numpy

def doFile(infile):
    infile=ROOT.TFile(infile,"UPDATE")
    EvsZ=infile.Get("ekin_pfx")
    last=EvsZ.FindLastBinAbove(50)
    thick=EvsZ.GetBinCenter(last)
    return thick

def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))
#def main():
materials=["iron","lead","alu"]
Ekin=asfloatarray(range(50,350,50)+[270])
graphs=[]
allGraph=ROOT.TMultiGraph()
iCol=2
for material in materials:
    res=[]
    for iEkin in Ekin:
        infile="deuteron-"+str(material)+"-"+str(int(iEkin))+"-histos.root"
        print "analysing",infile
        try:
            thick=doFile(infile)
        except:
            print "Error analysing",infile
            raise
        res.append(thick)
        print "Optimum thickness for",infile,":",thick,"mm"
    graph=ROOT.TGraph(len(Ekin),Ekin,asfloatarray(res))
    graph.SetName(material)
    graph.SetTitle(material)
    graph.GetXaxis().SetTitle("E_{kin} / MeV")
    graph.GetYaxis().SetTitle("Thickness / mm")
    graph.SetLineColor(iCol)
    graph.SetMarkerStyle(10+iCol)
    graph.SetFillColor(0)
    graph.SetLineWidth(3)
    iCol+=2
    #graph.SetLineColour()
    allGraph.Add(graph)
c1=ROOT.TCanvas("*","*",1600,900)
allGraph.Draw("ALP")
allGraph.GetXaxis().SetTitle("E_{kin} / MeV")
allGraph.GetYaxis().SetTitle("Thickness / mm")
c1.BuildLegend(0.4,0.67,0.65,0.88)
ROOT.gPad.Modified()
c1.Print("c1.pdf")
c1.Print("c1.root")
#main()
