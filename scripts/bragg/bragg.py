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
        histos.append(ROOT.TH2F("xp_vs_z","x' vs z,",5000,0,500,1000,-50,50))
        histomap={}
        for h in histos:
            histomap.update({h.GetName():h})
        histomap["edep_vs_ekin"].GetYaxis().SetTitle("E_{dep} / MeV")
        histomap["edep_vs_ekin"].GetXaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetYaxis().SetTitle("E_{kin} / MeV")
        histomap["ekin_vs_z"].GetXaxis().SetTitle("z / mm")
        histomap["x_vs_z"].GetXaxis().SetTitle("z [mm]")
        histomap["x_vs_z"].GetYaxis().SetTitle("x [mm]")
        histomap["xp_vs_z"].GetXaxis().SetTitle("z [mm]")
        histomap["xp_vs_z"].GetYaxis().SetTitle("x' [mrad]")
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
        x_vs_z.SetTitle("x_vs_z")
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
            #histomap["xp_vs_z"]
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
    z=map(lambda x:x/10., range(0,10*21,5))
    zout=[]
    sx=[]
    for iz in z:
        bin=hXvsZ.GetXaxis().FindBin(iz)
        hproj=hXvsZ.ProjectionY("_pfy",bin,bin+4)
        fitfunc=ROOT.TF1("gaus","gaus",-30,30)
        if hproj.GetEntries()>500:
            hproj.Fit(fitfunc,"QR")
            sx.append(sqrt(fitfunc.GetParameter(2)**2+5**2))
            zout.append(iz)
    return zout,sx

def getEvsZ(infile,dirname):
    hEvsZ=infile.Get(dirname+"/ekin_vs_z")
    z=map(lambda x:x/10., range(0,10*21,5))
    zout=[]
    e=[]
    for iz in z:
        bin=hEvsZ.GetXaxis().FindBin(iz)
        hproj=hEvsZ.ProjectionY("_pfy",bin,bin+4)
#        fitfunc=ROOT.TF1("gaus","gaus",-30,30)
        if hproj.GetEntries()>500:
 #           hproj.Fit(fitfunc,"QR")
            e.append(hproj.GetMean())
            zout.append(iz)
    return zout,e

if __name__=="__main__":
    ROOT.gSystem.Load("libAnalysis")
    myAnalysis=braggAnalysis()
    myAnalysis.Init()
    mycolors=colors()
    #leftToDo=myAnalysis(analyse)
    #while len(leftToDo):
    #    myAnalysis(analyse)
    infile=ROOT.TFile("bragg.root")
    energies=[270,250,200,150,100]
    c1=ROOT.TCanvas()
    gWidth=ROOT.TMultiGraph()
    gEnergy=ROOT.TMultiGraph()
    fEnergyFitfunc=ROOT.TF1("pol3","pol3",0,20)
    for e in energies:
        fEvsZ=fEnergyFitfunc.Clone()
        z,sx=getXvsZ(infile,"deuteron-carbon-"+str(e))
        gX=ROOT.TGraph(len(z),asfloatarray(z),asfloatarray(sx))
        gX.SetName(str(e)+" MeV")
        fXvsZ=ROOT.TF1("pol2","pol2",0,z[-1])
        #gX.Fit(fXvsZ,"QR")
        gX.Draw("AP")
        gX.GetYaxis().SetTitle("#sigma_{x} [mm]")
        gX.GetXaxis().SetTitle("z [mm]")
        gX.SetMarkerStyle(22)
        gX.Draw("AP")
        c1.Print(str(e)+"-XvsZ.pdf")
        z,ekin=getEvsZ(infile,"deuteron-carbon-"+str(e))
        gE=ROOT.TGraph(len(z),asfloatarray(z),asfloatarray(ekin))
        gE.SetName(str(e)+" MeV")
        gE.SetTitle(str(e)+" MeV")
        gE.Fit(fEvsZ,"QR")
        gE.Draw("AP")
        gE.GetYaxis().SetRangeUser(0,280)
        gE.GetXaxis().SetRangeUser(0,30)
        gE.GetYaxis().SetTitle("E_{kin} [MeV]")
        gE.GetXaxis().SetTitle("z [mm]")
        fEvsZ.SetLineColor(mycolors.next())
        gE.SetMarkerColor(mycolors.cur())
        gE.SetFillColor(ROOT.kWhite)
        gE.SetMarkerStyle(22)
        gE.Draw("AP")
        gEnergy.Add(gE)
        gWidth.Add(gX)
        c1.Print(str(e)+"-EvsZ.pdf")
        for d in [1,5,10,20]:
            out=open(str(e)+"-"+str(d)+".mac","w")
            out.write("/PolarimeterStudies/target/z "+str(d)+" mm \n"
                      "/PolarimeterStudies/detector/update\n\n")
            for z in map(lambda x: x/10., range(0,d*10,2)):
                if fEvsZ.Eval(z+.5)<50:
                    continue
                out.write("/analysis/setFileName "+str(e)+"-"+str(d)+"-"+str(z)+".root\n"
                          "/PolarimeterStudies/dcelastic/beamsize"+' {:1.3f} {:1.3f} {:1.3f} '.format(5.,5.,.2)+"mm\n"
                          "/PolarimeterStudies/dcelastic/beamposition"+' {0:1.3f} {0:1.3f} {1:1.3f} '.format(0,z+0.1)+"mm\n"
                          "/PolarimeterStudies/dcelastic/energy"+' {0:1.3f} '.format(fEvsZ.Eval(z+.1))+"MeV\n"
                          "/run/beamOn {nevents}\n\n"
                          )
        gEnergy.Draw("AP")
        c1.BuildLegend()
        gEnergy.GetXaxis().SetLimits(0,22);
        gEnergy.GetXaxis().SetTitle("z [mm]")
        gEnergy.GetYaxis().SetTitle("E_{kin} [mm]")
        gEnergy.SetMinimum(0)
        gEnergy.SetMaximum(300)
        c1.Print("EvsZ.pdf")
        gWidth.Draw("AP")
        c1.Print("XvsZ.pdf")


            
#/analysis/setFileName {Material}-{Ekin}-{Thickness}.root
#/PolarimeterStudies/target/material {Material}
#/PolarimeterStudies/dcelastic/energy {Ekin} MeV
#/PolarimeterStudies/target/z {Thickness} mm
#/PolarimeterStudies/dcelastic/beamsize 5 5 {Thickness} mm
#/PolarimeterStudies/detector/update
#/run/beamOn 1000000
