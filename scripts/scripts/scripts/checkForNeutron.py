#!/usr/bin/env python
import ROOT
from math import sqrt
ROOT.gStyle.SetOptStat(00000)
infile=ROOT.TFile("run_0.root")
outfile=ROOT.TFile("cocktail.root","RECREATE")
outfile.cd()
hBreakup=ROOT.TH1F("hBreakup","neutron (breakup) escapes",300,0,300)
hNeutron=ROOT.TH1F("hNeutron","neutron (other) escapes",300,0,300)
hGamma=ROOT.TH1F("hGamma","#gamma escapes",300,0,300)
hOther=ROOT.TH1F("hOther","other",300,0,300)
hAbsorbed=ROOT.TH1F("hAbsorbed","no particle escape",300,0,300)
hDeuteronExit=ROOT.TH1F("deuteronExit","deuteron escapes",300,0,300)
import pypdt
neutron_mass=pypdt.get(2112).mass
proton_mass=pypdt.get(2212).mass
deuteron4=ROOT.TLorentzVector(0,0,sqrt(.270*(.270+2*1.875)),1.875+.270)
mothermass=ROOT.TH1F("mother","mother mass",1000,0,5)
missingmass=ROOT.TH1F("missing","missing mass",1000,0,5)
iev=0
edepMap={}
particleMap={}
for event in infile.sim:
    if iev % (0.05*infile.sim.GetEntries())==0:
        print str(int(float(100.*iev/infile.sim.GetEntries())))+"% done"
    deuteronExit=False
    breakup=False
    other=False
    gamma=False
    neutron=False
    tracks=[]
    iev+=1
    allTrackIds=[]
    edep=0
    for hit in event.Calorimeter:
        if hit.pid in edepMap:
            edepMap[hit.pid]+=hit.edep
        else:
            edepMap[hit.pid]=hit.edep
        edep+=hit.edep
        if hit.trid not in allTrackIds:
            allTrackIds.append(hit.trid)
    for id in allTrackIds:
        tracks.append(sorted(filter(lambda hit:hit.trid==id,event.Calorimeter),key=lambda hit:hit.tof))
    protons=[]
    neutrons=[]
    particles=[]
    for track in tracks:
        if(len(track)!=0):
            if(track[0].pid==2112):
                neutrons.append(ROOT.TLorentzVector(track[0].px,track[0].py,track[0].pz,track[0].ekin))
            if(track[0].pid==2212):
                protons.append(ROOT.TLorentzVector(track[0].px,track[0].py,track[0].pz,track[0].ekin))
            #if(track[0].pid!=2212 and track[0].pid!=2112):
            #    particles.append(ROOT.TLorentzVector(track[0].px,track[0].py,track[0].pz,track[0].ekin))
    if(len(protons)!=0 and len(neutrons)!=0):
        for p in protons:
            for n in neutrons:
                m=abs((deuteron4-p-n).M())
                missingmass.Fill(m)
                m=(p+n).M()
                mothermass.Fill(m)
                if abs(m/1.875-1)<0.2:
                    breakup=True
    for hit in event.Mother:
        if hit.pid in particleMap:
            particleMap[hit.pid]+=1
        else:
            particleMap[hit.pid]=1
        if hit.pid==1000010020:
            deuteronExit=True
        if hit.pid==2112 and breakup==False:
            neutron=True
        if hit.pid==22 and neutron==False and breakup==False:
            gamma=True
        if breakup==False and deuteronExit==False and gamma==False and neutron==False:
            other=True
    if(other==True):
        hOther.Fill(edep)
    if(deuteronExit==True):
        hDeuteronExit.Fill(edep)
    if(breakup==False and other==False and deuteronExit==False and neutron==False and gamma==False):
        hAbsorbed.Fill(edep)
    if(breakup==True):
        hBreakup.Fill(edep)
    if neutron:
        hNeutron.Fill(edep)
    if gamma:
        hGamma.Fill(edep)

sum=max(1,hNeutron.GetEntries()+hGamma.GetEntries()+hDeuteronExit.GetEntries()+hBreakup.GetEntries()+hOther.GetEntries()+hAbsorbed.GetEntries())
hBreakup.Scale(100./sum)
hOther.Scale(100./sum)
hNeutron.Scale(100./sum)
hAbsorbed.Scale(100./sum)
hDeuteronExit.Scale(100./sum)
hGamma.Scale(100./sum)
def SetColor(histo,color):
    histo.SetLineColor(color)
    histo.SetFillColor(color)
    histo.SetMarkerColor(color)
SetColor(hBreakup,ROOT.kBlue)
SetColor(hOther,ROOT.kRed)
SetColor(hAbsorbed,ROOT.kGreen+2)
SetColor(hDeuteronExit,ROOT.kYellow+2)
SetColor(hGamma,ROOT.kMagenta+2)
SetColor(hNeutron,ROOT.kCyan+2)
hBreakup.Draw()
c1=ROOT.gROOT.FindObject("c1")
c1.SetLogy()
c1.Print("hBreakup.pdf")
hOther.Draw()
c1.Print("hOther.pdf")
hAbsorbed.Draw()
c1.Print("hAbsorbed.pdf")
hDeuteronExit.Draw()
c1.Print("hDeuteronExit.pdf")
hGamma.Draw()
c1.Print("hGamma.pdf")
hNeutron.Draw()
c1.Print("hNeutron.pdf")
stack=ROOT.THStack("stacked","energy deposition in detector element")
stack.Add(hOther)
stack.Add(hBreakup)
stack.Add(hNeutron)
stack.Add(hGamma)
stack.Add(hDeuteronExit)
#stack.Add(hOther)
stack.Add(hAbsorbed)
stack.Draw()
stack.GetYaxis().SetTitle("rel. freq. [%]")
stack.GetXaxis().SetTitle("E_{dep} [MeV]")
stack.GetXaxis().SetNoExponent(True)
"""
hAbsorbed.Draw()
hBreakup.Draw("SAME")
hOther.Draw("SAME")
"""
missingmass.GetXaxis().SetTitle("m_{miss} [GeV]")
mothermass.GetXaxis().SetTitle("m_{p+n} [GeV]")
missingmass.Scale(100./missingmass.GetEntries())
mothermass.Scale(100./mothermass.GetEntries())
missingmass.GetYaxis().SetTitle("rel. freq. [%]")
mothermass.GetYaxis().SetTitle("rel. freq. [%]")
c1.BuildLegend(.5,.5,.9,.8)
c1.Print("cocktail.pdf")
missingmass.GetXaxis().SetRangeUser(missingmass.GetMean()-5*missingmass.GetRMS(),missingmass.GetMean()+5*missingmass.GetRMS())
missingmass.Draw()
c1.Print("missingmass.pdf")
mothermass.GetXaxis().SetRange(mothermass.FindFirstBinAbove(0)-3,mothermass.FindLastBinAbove(0)+3)
mothermass.Draw()
c1.Print("mothermass.pdf")
hBreakup.Write()
hOther.Write()
hGamma.Write()
hAbsorbed.Write()
hDeuteronExit.Write()
mothermass.Write()
missingmass.Write()
stack.Write()
print "Exiting particles"
for partId,count in sorted(particleMap.iteritems(), key=lambda (k,v): (v,k),reverse=True):
    if pypdt.get(abs(partId))is not None:
        if partId<0:
            print "anti-",pypdt.get(abs(partId)).name,":",count
        else:
            print pypdt.get(abs(partId)).name,":",count
    else:
        print partId,":",count
