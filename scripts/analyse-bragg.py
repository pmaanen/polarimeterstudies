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
        histos.append(ROOT.TH1F("tof","tof",1000,0,5))
        histos.append(ROOT.TH1F("range","range",5000,0,500))
        histos.append(ROOT.TH1F("pathlength","pathlength",5000,0,500))
        histos.append(ROOT.TH1F("x","lateral range x",100,-50,50))
        histos.append(ROOT.TH1F("y","lateral range y",100,-50,50))
        histos.append(ROOT.TH2F("dedx","dedx",5000,0,500,2000,0,200))
        histos.append(ROOT.TH2F("edep_vs_ekin","E_{dep} vs E_{kin}",300,0,300,2000,0,200))
        histos.append(ROOT.TH2F("ekin_vs_z","E_{kin} vs z",5000,0,500,300,0,300))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        histomap["edep_vs_ekin"].GetYaxis().SetTitle("E_{dep} / MeV")
        histomap["edep_vs_ekin"].GetXaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetYaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetXaxis().SetTitle("z / mm")
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        for entry in data:
            hits=[TrackerHit(h) for h in entry.Detector]
            doEvent(hits,histomap)
        temp=histomap["ekin_vs_z"].Clone()
        temp.RebinX(10)
        temp2=histomap["edep_vs_ekin"].Clone()
        temp2.RebinX(10)
        dir.cd()
        profile=temp.ProfileX()
        profile.SetName("ekin_vs_z_pfx")
        profile.SetTitle("E_{kin} vs z")
        profile.Write()
        profile2=temp2.ProfileX()
        profile2.SetTitle("E_{dep} vs E_{kin}")
        profile2.Write()       
        temp3=histomap["dedx"].Clone()
        temp3.SetMinimum(2)
        temp3.RebinX(10)
        profile3=temp3.ProfileX()
        profile3.SetName("dEdz")
        profile3.SetTitle("dE/dz")
        profile3.Write()
        profile4=histomap["dedx"].ProfileX()
        profile4.Write()
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
    if len(primaryTrack)==0:
        return None
    for i in range(1,len(primaryTrack)):
        hit=primaryTrack[i]
        lastHit=primaryTrack[i-1]
        ds=0              
        if i>0:
            ds=hypot(lastHit.x-hit.x,hypot(lastHit.y-hit.y,lastHit.z-hit.z))
        if ds!=0:
            pathlength+=ds
            histomap["edep_vs_ekin"].Fill(hit.etot,hit.edep/ds)
            histomap["ekin_vs_z"].Fill(hit.z,hit.etot)
            histomap["dedx"].Fill(hit.z,hit.edep/ds)
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
