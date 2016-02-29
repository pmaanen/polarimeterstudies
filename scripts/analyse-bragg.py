#!/usr/bin/env python
from AnalysisBase import *
import ROOT
from numpy import genfromtxt,asarray
from array import array
from math import acos,atan2,sqrt,hypot
import sys

def analyse(filename,myWorker):
    try:
        outfile=ROOT.TFile(filename[:-5]+"-histos.root","RECREATE")
        dir=outfile.mkdir(filename[:-5])
        dir.cd()
        histos=[]
        histos.append(ROOT.TH1F("tof","tof",1000,0,10))
        histos.append(ROOT.TH1F("range","range",5000,0,500))
        histos.append(ROOT.TH1F("pathlength","pathlength",5000,0,500))
        histos.append(ROOT.TH1F("x","lateral range x",5000,-250,250))
        histos.append(ROOT.TH1F("y","lateral range y",5000,-250,250))
        histos.append(ROOT.TH1F("dedx","dE/dx",5000,0,500))
        histos.append(ROOT.TH2F("edep_vs_etot","E_{dep} vs E_{kin}",3000,0,300,3000,0,300))
        histos.append(ROOT.TH2F("etot_vs_z","E_{kin} vs z",5000,0,500,3000,0,300))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        histomap["edep_vs_etot"].GetYaxis().SetTitle("E_{dep} / MeV")
        histomap["edep_vs_etot"].GetXaxis().SetTitle("E_{kin} / MeV")
        histomap["etot_vs_z"].GetYaxis().SetTitle("E_{kin} / MeV")
        histomap["etot_vs_z"].GetXaxis().SetTitle("z / mm")
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        for entry in data:
            hits=[TrackerHit(h) for h in entry.Calorimeter]
            doEvent(hits,histomap)
        temp=histomap["etot_vs_z"].Clone()
        temp.RebinX(10)
        temp2=histomap["edep_vs_etot"].Clone()
        temp2.RebinX(10)
        dir.cd()
        profile=temp.ProfileX()
        profile.Write()
        profile2=temp2.ProfileX()
        profile2.Write()
        for h in histos:
            h.Write()
        #outfile.Write()
        outfile.Close()
    except Exception as e:
        print "Problem in file:",filename,str(e)
        return e
    return (filename[:-5])


def doEvent(hits,histomap):
    if len(hits)==0:
        return None
    primaryTrack=sorted(filter(lambda hit:hit.trackId==1,hits),key=lambda hit:hit.time)
    pathlength=0
    for i in range(len(primaryTrack)):
        hit=primaryTrack[i]
        lastHit=primaryTrack[i-1]
        if i>0:
            pathlength+=hypot(lastHit.x-hit.x,hypot(lastHit.y-hit.y,lastHit.z-hit.z))
        histomap["edep_vs_etot"].Fill(hit.etot,hit.edep)
        histomap["etot_vs_z"].Fill(hit.z,hit.etot)
        histomap["dedx"].Fill(hit.z,hit.edep)
        histomap["tof"].Fill(hit.time)
#Check if last hit is near minimum kinetic energy cut=>no particle conversion took place
    if primaryTrack[-1].etot<.15:
        histomap["pathlength"].Fill(pathlength)
        histomap["range"].Fill(primaryTrack[-1].z)
        histomap["x"].Fill(primaryTrack[-1].x)
        histomap["y"].Fill(primaryTrack[-1].y)
        histomap["tof"].Fill(primaryTrack[-1].time)
class braggAnalysis(AnalysisBase):
    def Init(self):
        return
    def __init__(self):
        AnalysisBase.__init__(self)

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=braggAnalysis()
    myAnalysis.Init()
    leftToDo=myAnalysis(analyse)
    while len(leftToDo):
      myAnalysis(analyse)
