#!/usr/bin/env python
import ROOT
from numpy.random import normal
from numpy import asarray
def asfloatarray(vec):
    return asarray(map(lambda x:float(x),vec))
colors=range(1,10,1)
Ekin=[270]
c1=ROOT.TCanvas("*","*",1200,800)
c1.SetGridx()
c1.SetGridy()
hpassed=ROOT.TH1F("passed","passed",501,-0.5,500.5)
htotal=ROOT.TH1F("total","total",501,-0.5,500.5)
allgraphs=[]
for process in ["elastic","breakup"]:
    for material in ["lyso","plastic"]:
        sigmas=[0.0,0.05,0.1,0.2]
        efficiencies=[]
        graphs=[]
        iColor=0
        mg=ROOT.TMultiGraph()
        outfile=ROOT.TFile(process+"-"+material+"-cuts"+".root","RECREATE")
        for ii in range(len(sigmas)):
            for iEkin in Ekin:
                hpassed.Reset()
                htotal.Reset()
                h_sum=ROOT.TH1F("sum"+str(iEkin)+"-"+str(sigmas[ii]),"smeared"+str(iEkin)+"-"+str(sigmas[ii]),501,-0.5,500.5)
                h_max=ROOT.TH1F("max"+str(iEkin)+"-"+str(sigmas[ii]),"smeared"+str(iEkin)+"-"+str(sigmas[ii]),501,-0.5,500.5)
                infile=ROOT.TFile(material+"-"+process+".root")
                data=infile.Get("sim")
                outfile.cd()
                passed=0
                notpassed=0
                iev=0
                for event in data:
                    if len(event.Calorimeter)==0:
                        continue
                    energies=map(lambda x:x.edep,event.Calorimeter)
                    energies.sort()
                    edep=energies[-1]
                    if(sigmas[ii]!=0):
                        edep*=normal(1,sigmas[ii])
                    else:
                        edep*=1
                    h_max.Fill(edep)
                    sum=0
                    for en in energies:
                        if(sigmas[ii]!=0):
                            en*=normal(1,sigmas[ii])
                        else:
                            en*=1
                        sum+=en
                        h_sum.Fill(sum)
                h_sum.Write()
                h_max.Write()
                for cut in range(0,350,10):
                    passed=0
                    total=10000
                    for i in range(total):
                        htotal.Fill(cut+0.01)
                    for event in data:
                        edep=0
                        if len(event.Calorimeter)==0:
                            continue
                        energies=map(lambda x:x.edep,event.Calorimeter)
                        energies.sort()
                        edep=energies[-1]
                        if(sigmas[ii]!=0):
                            edep*=normal(1,sigmas[ii])
                        else:
                            edep*=1
                        if edep>cut:
                            hpassed.Fill(cut+0.01)
                            passed+=1
                        else:
                            notpassed+=1
                    print iEkin,sigmas[ii],cut,passed,data.GetEntriesFast()
                eff=ROOT.TEfficiency(hpassed,htotal)
                efficiencies.append(ROOT.TEfficiency(eff))
                eff.Paint("")
                graph=ROOT.TGraphAsymmErrors(eff.GetPaintedGraph())
                graph.GetXaxis().SetTitle("E_{cut} [MeV]")
                graph.GetYaxis().SetTitle("#epsilon")
                graph.SetName(material+process+str(sigmas[ii]))
                graph.SetTitle(str(int(sigmas[ii]*100))+"%")
                graphs.append(graph)
                allgraphs.append(ROOT.TGraphAsymmErrors(graph))
                graph.SetLineColor(iColor % len (colors)+1)
                iColor+=1
                graph.SetLineWidth(2)
                graph.Write()
        for graph in graphs:
            mg.Add(graph)
        mg.SetTitle(process+" detection efficiency in "+material)
        mg.Draw("ALP")
        mg.GetXaxis().SetTitle("E_{cut} [MeV]")
        mg.GetYaxis().SetTitle("#epsilon")
        mg.Draw("ALP")
        c1.BuildLegend(0.75,0.4,.9,0.85)
        c1.Print(material+"-"+process+".pdf")
for material in ["plastic","lyso"]:
    mg2=ROOT.TMultiGraph()
    mg2.SetTitle("relative fom in"+material)
    signal=filter(lambda x:x.GetName()==material+"elastic"+str(sigmas[0]),allgraphs)[0]
    background=filter(lambda x:x.GetName()==material+"breakup"+str(sigmas[0]),allgraphs)[0]
    sb1=[]
    for cut in range(0,350,1):
        if(signal.Eval(cut)!=0 or background.Eval(cut)!=0):
            fom=(signal.Eval(cut)/(background.Eval(cut)+signal.Eval(cut)))**2*signal.Eval(cut)
            sb1.append(fom)
    pur1=ROOT.TGraph(len(sb1),asfloatarray(range(0,350,1)),asfloatarray(sb1))
    pur1.SetTitle(str(int(sigmas[0]*100))+"%")
    pur1.SetFillColor(ROOT.kWhite)
    pur1.SetLineColor(ROOT.kBlue)
    signal=filter(lambda x:x.GetName()==material+"elastic"+str(sigmas[3]),allgraphs)[0]
    background=filter(lambda x:x.GetName()==material+"breakup"+str(sigmas[3]),allgraphs)[0]
    sb2=[]
    for cut in range(0,350,1):
        if(signal.Eval(cut)!=0 or background.Eval(cut)!=0):
            fom=(signal.Eval(cut)/(background.Eval(cut)+signal.Eval(cut)))**2*signal.Eval(cut)
            sb2.append(fom)
    pur2=ROOT.TGraph(len(sb2),asfloatarray(range(0,350,1)),asfloatarray(sb2))
    pur2.SetTitle(str(int(sigmas[3]*100))+"%")
    pur2.SetFillColor(ROOT.kWhite)
    pur2.SetLineColor(ROOT.kRed)
    pur1.SetLineWidth(2)
    pur2.SetLineWidth(2)
    mg2.Add(pur1)
    mg2.Add(pur2)
    mg2.Draw("ALP")
    mg2.GetXaxis().SetLimits(0,350)
    #mg2.GetYaxis().SetRangeUser(0,)
    mg2.GetXaxis().SetTitle("E_{cut} [MeV]")
    mg2.GetYaxis().SetTitle("rel. fom [au]")
    c1.BuildLegend(.8,.8,0.95,0.95)
    c1.Print("fom-"+material+".pdf")
