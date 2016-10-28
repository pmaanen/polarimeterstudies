#!/usr/bin/env python
import ROOT,sys,numpy

def doFile(infile,dirname):
    histo=infile.Get(dirname+"/edep")
    if histo.GetEntries()>300:
        edep=histo.GetBinCenter(histo.GetMaximumBin())
        edep_error=histo.GetBinCenter(histo.FindLastBinAbove(histo.GetMaximum()/2))-histo.GetBinCenter(histo.FindFirstBinAbove(histo.GetMaximum()/2))
        return (edep,edep_error)
    else:
        return None
    
def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))

def calculateDerivative(xvalues,yvalues):
    if len(xvalues)!=len(yvalues):
        return None
    if len(xvalues)<2:
        return None
    delta=[]
    for i in range(len(xvalues)-1):
        dx=xvalues[i+1]-xvalues[i]
        dy=yvalues[i+1]-yvalues[i]
        delta.append(dy/dx)
    return delta



def main():
    c1=ROOT.TCanvas("","",1600,900)
    Ekin=range(100,300,50)+[270]
    Leff=range(30,100,5)
    infile=ROOT.TFile("dedx.root")    
    outfile=ROOT.TFile("graphs.root","RECREATE")
    outfile.cd()
    for iEkin in Ekin:
        xvalues=[]
        yvalues=[]
        xerrors=[]
        yerrors=[]
        for iLeff in Leff:
            dirname="deuterons_"+str(iEkin)+"_"+str(iLeff)
            print dirname
            try:
                res=doFile(infile,dirname)
                if res is not None:
                    print res
                    xvalues.append(iLeff)
                    xerrors.append(0)
                    yerrors.append(res[1])
                    yvalues.append(res[0])
            except:
                print "Error analysing",infile
                raise
        if len(xvalues)==0:
            continue
        print "---"
        print xvalues
        print yvalues
        graph=ROOT.TGraphErrors(len(xvalues),asfloatarray(xvalues),asfloatarray(yvalues),asfloatarray(xerrors),asfloatarray(yerrors))
        graph.SetName("edep"+str(iEkin))
        graph.SetTitle("edep"+str(iEkin))
        graph.GetXaxis().SetLimits(30,80)
        graph.GetXaxis().SetTitle("L_{eff} [mm]")
        graph.GetYaxis().SetTitle("<E_dep> [MeV]")
        graph.Draw("AP")
        graph.Write()
        delta=calculateDerivative(xvalues,yvalues)
        if delta is not None:
            graph2=ROOT.TGraph(len(delta),asfloatarray(xvalues[1:]),asfloatarray(delta))
            graph2.SetName("delta_"+str(iEkin))
            graph2.SetTitle("delta_"+str(iEkin))
            graph2.GetXaxis().SetLimits(30,80)
            graph2.GetXaxis().SetTitle("L_{eff} [mm]")
            graph2.GetYaxis().SetTitle("dEd/dz [MeV/mm]")
            graph2.Draw("AP")
            graph2.Write()
    #allGraph.Add(graph)
#allGraph.Draw("ALP")
#allGraph.GetXaxis().SetTitle("E_{kin} / MeV")
#allGraph.GetYaxis().SetTitle("Thickness / mm")
        c1.Print(str(iEkin)+".pdf")
        c1.Print(str(iEkin)+".root")
if __name__=="__main__":
    main()
