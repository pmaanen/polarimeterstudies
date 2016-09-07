#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy.random import randint,normal
import sys
from math import hypot
from copy import deepcopy
class exampleAnalysis(AnalysisBase):
    def Init(self):
        return


def analysis(filename,myWorker):
    try:
        hSum=ROOT.TH1F("hSum","sum of E_{dep}",300,0,300)
        hMax=ROOT.TH1F("hMax","max of E_{dep}",300,0,300)
        hDiff=ROOT.TH1F("hDiff","difference between sum and max",600,-300,300)
	hEkin=ROOT.TH1F("hEkin","kinetic Energy of primary particle",300,0,300)
        hXE=ROOT.TH2F("hXE","x vs deposited energy",500,-250,250,300,0,300)
        hYE=ROOT.TH2F("hYE","y vs deposited energy",500,-250,250,300,0,300)
        hXY=ROOT.TH2F("hXY","coordinate of entry point",500,-250,250,500,-250,250)
        hXYE3=ROOT.TH3F("hXYE","x vs y vs E_{dep}",500,-250,250,500,-250,250,300,0,300)
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        dir.cd()
        for event in data:
            sum=0
            for hit in event.Left:
                sum+=hit.edep
            x=y=0
            for hit in event.TriggerL:
                if hit.trid==1 and len(event.Left)!=0:
		    hEkin.Fill(hit.ekin)
                    x=hit.x
                    y=hit.y
                    hXE.Fill(x,sum)
                    hYE.Fill(y,sum)
                    hXY.Fill(x,y)
                    hXYE3.Fill(x,y,sum)
                    hSum.Fill(sum)
                    hMax.Fill(sorted(event.Left,key=lambda hit:hit.edep,reverse=True)[0].edep)
                    hDiff.Fill(sum-sorted(event.Left,key=lambda hit:hit.edep,reverse=True)[0].edep)
        hEkin.Write()
        hMax.Write()
        hSum.Write()
	hDiff.Write()
	hXYE3.GetXaxis().SetTitle("x [mm]")
	hXYE3.GetYaxis().SetTitle("y [mm]")
	hXYE3.GetZaxis().SetTitle("E_{dep} [MeV]")	
	pXYE=hXYE3.Project3DProfile("yx")
        y1=hXYE3.Project3D("zy").GetYaxis().FindBin(-1000)
        y2=hXYE3.Project3D("zy").GetYaxis().FindBin(1000)
        x1=hXYE3.Project3D("zx").GetYaxis().FindBin(-1000)
        x2=hXYE3.Project3D("zx").GetYaxis().FindBin(1000)
        hXYE3.Project3D("zy").ProfileX("Edep_vs_ y",x1,x2).Write()
        hXYE3.Project3D("zx").ProfileX("Edep_vs_x",y1,y2).Write()
        #pXE=hXE.ProfileX()
        #pYE=hYE.ProfileX()
        #pXE.GetXaxis().SetTitle("x [mm]")
        #pXE.GetYaxis().SetTitle("<E_{dep}> [MeV]")
        #pYE.GetXaxis().SetTitle("y [mm]")
        #pYE.GetYaxis().SetTitle("<E_{dep}> [MeV]")
	#pXE.Write()
	#pYE.Write()
        hXYE3.Write()
        hXE.Write()
        hYE.Write()
        hXY.Write()
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
