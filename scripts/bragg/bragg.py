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
        histos.append(ROOT.TH1F("x","lateral range x",1000,-50,50))
        histos.append(ROOT.TH1F("y","lateral range y",1000,-50,50))
        histos.append(ROOT.TH2F("dedx","dedx",5000,0,500,2000,0,200))
        histos.append(ROOT.TH2F("edep_vs_ekin","E_{dep} vs E_{kin}",300,0,300,2000,0,200))
        histos.append(ROOT.TH2F("ekin_vs_z","E_{kin} vs z",5000,0,500,300,0,300))
        histos.append(ROOT.TH2F("x_vs_z","x vs z,",5000,0,500,1000,-50,50))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        histomap["edep_vs_ekin"].GetYaxis().SetTitle("E_{dep} / MeV")
        histomap["edep_vs_ekin"].GetXaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetYaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetXaxis().SetTitle("z / mm")
        histomap["x_vs_z"].GetXaxis().SetTitle("z [mm]")
        histomap["x_vs_z"].GetYaxis().SetTitle("x [mm]")
        infile=ROOT.TFile(filename,"READ")
        data=infile.Get("sim")
        for entry in data:
            hits=[TrackerHit(h) for h in entry.Detector]
            doEvent(hits,histomap)
        temp=histomap["ekin_vs_z"].Clone()
        temp.RebinX(2)
        temp2=histomap["edep_vs_ekin"].Clone()
        temp2.RebinX(2)
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
        temp3.RebinX(2)
        profile3=temp3.ProfileX()
        profile3.SetName("dEdz")
        profile3.SetTitle("dE/dz")
        profile3.Write()
        profile4=histomap["dedx"].ProfileX()
        profile4.Write()
        x_vs_z=histomap["x_vs_z"].ProfileX()
        x_vs_z.Write()
        x_vs_z.SetName("x_vs_z")
        x_vs_z.SetTitle("#x_vs_z")
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
            histomap["x_vs_z"].Fill(hit.z,hit.x)
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


def getXvsZ(infile,dirname):
    hXvsZ=infile.Get(dirname+"/x_vs_z")
    z=range(5,60)
    zout=[]
    sx=[]
    for iz in z:
        bin=hXvsZ.GetXaxis().FindBin(iz)
        hproj=hXvsZ.ProjectionY("_pfy",bin,bin+9)
        fitfunc=ROOT.TF1("gaus","gaus",-30,30)
        if hproj.GetEntries()>500:
            hproj.Fit(fitfunc,"QR")
            sx.append(fitfunc.GetParameter(2))
            zout.append(iz)
    return zout,sx

def getEvsZ(infile,dirname):
    hEvsZ=infile.Get(dirname+"/ekin_vs_z")
    z=range(5,60)
    zout=[]
    e=[]
    for iz in z:
        bin=hEvsZ.GetXaxis().FindBin(iz)
        hproj=hEvsZ.ProjectionY("_pfy",bin,bin+9)
#        fitfunc=ROOT.TF1("gaus","gaus",-30,30)
        if hproj.GetEntries()>500:
 #           hproj.Fit(fitfunc,"QR")
            e.append(hproj.GetMean())
            zout.append(iz)
    return zout,e

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    #myAnalysis=braggAnalysis()
    #myAnalysis.Init()
    #leftToDo=myAnalysis(analyse)
    #while len(leftToDo):
    #  myAnalysis(analyse)
    infile=ROOT.TFile("bragg.root")
    energies=[100,150,200,250,270]
    c1=ROOT.TCanvas()
    for e in energies:
        z,sx=getXvsZ(infile,"deuteron-carbon-"+str(e))
        graph=ROOT.TGraph(len(z),asfloatarray(z),asfloatarray(sx))
        fXvsZ=ROOT.TF1("pol2","pol2",3,z[-1])
        graph.Fit(fXvsZ,"QR")
        graph.Draw("AP")
        graph.GetYaxis().SetTitle("#sigma_{x} [mm]")
        graph.GetXaxis().SetTitle("z [mm]")
        graph.SetMarkerStyle(22)
        graph.Draw("AP")
        c1.Print(str(e)+"-XvsZ.pdf")
        z,ekin=getEvsZ(infile,"deuteron-carbon-"+str(e))
        graph=ROOT.TGraph(len(z),asfloatarray(z),asfloatarray(ekin))
        fEvsZ=ROOT.TF1("pol3","pol3",3,z[-1])
        graph.Fit(fEvsZ,"QR")
        graph.Draw("AP")
        graph.GetYaxis().SetTitle("E_{kin} [MeV]")
        graph.GetXaxis().SetTitle("z [mm]")
        graph.SetMarkerStyle(22)
        graph.Draw("AP")
        c1.Print(str(e)+"-EvsZ.pdf")
        for d in [1,5,10,20]:
            out=open(str(e)+"-"+str(d)+".mac","w")
            out.write("/PolarimeterStudies/target/z "+str(d)+" mm \n"
                      "/PolarimeterStudies/detector/update\n\n")
            for z in range(d):
                if fEvsZ.Eval(z+.5)<50:
                    continue
                out.write("/analysis/setFileName "+str(e)+"-"+str(d)+"-"+str(z)+".root\n"
                          "/PolarimeterStudies/dcelastic/beamsize"+' {:1.3f} {:1.3f} {:1.3f} '.format(fXvsZ.Eval(z+.5),fXvsZ.Eval(z+.5),1)+"mm\n"
                          "/PolarimeterStudies/dcelastic/beamposition"+' {0:1.3f} {0:1.3f} {1:1.3f} '.format(0,z+0.5)+"mm\n"
                          "/PolarimeterStudies/dcelastic/energy"+' {0:1.3f} '.format(fEvsZ.Eval(z+.5))+"MeV\n"
                          "/run/beamOn {nevents}\n\n"
                          )
#/analysis/setFileName {Material}-{Ekin}-{Thickness}.root
#/PolarimeterStudies/target/material {Material}
#/PolarimeterStudies/dcelastic/energy {Ekin} MeV
#/PolarimeterStudies/target/z {Thickness} mm
#/PolarimeterStudies/dcelastic/beamsize 5 5 {Thickness} mm
#/PolarimeterStudies/detector/update
#/run/beamOn 1000000
