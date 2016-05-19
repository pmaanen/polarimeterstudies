#!/usr/bin/env ipython
import ROOT
infile=ROOT.TFile("edep.root","READ")
hQGSP_BERT=infile.Get("elastic_QGSP_BERT/hEdep")
hQGSP_BERT.SetTitle("QGSP_BERT")
hQGSP_BERT.SetLineColor(ROOT.kBlue)
hQGSP_BIC=infile.Get("elastic_QGSP_BIC/hEdep")
hQGSP_BIC.SetTitle("QGSP_BIC")
hQGSP_BIC.SetLineColor(ROOT.kRed)
hQGSP_INCLXX=infile.Get("elastic_QGSP_INCLXX/hEdep")
hQGSP_INCLXX.SetTitle("QGSP_INCLXX")
hQGSP_INCLXX.SetLineColor(ROOT.kGreen+10)
c1=ROOT.TCanvas("E_{dep}")
stack=ROOT.THStack()
stack.Add(hQGSP_INCLXX)
stack.Add(hQGSP_BIC)
stack.Add(hQGSP_BERT)
stack.Draw("nostack")
stack.GetXaxis().SetTitle("E_{dep} [MeV]")
stack.GetYaxis().SetTitle("rel. freq. [%]")
c1.BuildLegend(0.1,0.75,0.5,0.95)
c1.SetLogy()
c1.Print("edep.pdf")
hQGSP_BERT_REAL=infile.Get("elastic_QGSP_BERT/hEdep_real")
hQGSP_BERT_REAL.SetTitle("QGSP_BERT")
hQGSP_BERT_REAL.SetLineColor(ROOT.kBlue)
hQGSP_BIC_REAL=infile.Get("elastic_QGSP_BIC/hEdep_real")
hQGSP_BIC_REAL.SetTitle("QGSP_BIC")
hQGSP_BIC_REAL.SetLineColor(ROOT.kRed)
hQGSP_INCLXX_REAL=infile.Get("elastic_QGSP_INCLXX/hEdep_real")
hQGSP_INCLXX_REAL.SetTitle("QGSP_INCLXX")
hQGSP_INCLXX_REAL.SetLineColor(ROOT.kGreen+10)
stack2=ROOT.THStack()
stack2.Add(hQGSP_INCLXX_REAL)
stack2.Add(hQGSP_BIC_REAL)
stack2.Add(hQGSP_BERT_REAL)
stack2.Draw("nostack,e1p")
stack2.GetXaxis().SetTitle("E_{dep} [MeV]")
stack2.GetYaxis().SetTitle("rel. freq. [%]")
stack2.Draw("nostack,e1p")
c1.BuildLegend(0.1,0.75,0.5,0.95)
c1.Print("edep_real.pdf")
