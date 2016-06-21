#!/usr/bin/env python
import ROOT,sys,numpy,math
c1=ROOT.TCanvas("*","*",1600,900)

fitfunc=ROOT.TF1("fa","[0]+[1]*x",40,60); 

def doFile(infile,dirname):
    try:
        EvsZ=infile.Get(dirname+"/ekin_vs_z_pfx")
        last=EvsZ.FindLastBinAbove(50)
        lower=EvsZ.GetBinCenter(EvsZ.FindLastBinAbove(40))
        upper=EvsZ.GetBinCenter(EvsZ.FindLastBinAbove(60))
        EvsZ.Fit(fitfunc,"Q")
        pol1=fitfunc;
        val=(50-pol1.GetParameter(0))/pol1.GetParameter(1)
        err=math.sqrt(1/pol1.GetParameter(1)*1/pol1.GetParameter(1)*pol1.GetParError(0)*pol1.GetParError(0)+((50-pol1.GetParameter(0))/(pol1.GetParameter(1)*pol1.GetParameter(1)))*((50-pol1.GetParameter(0))/(pol1.GetParameter(1)*pol1.GetParameter(1)))*pol1.GetParError(1)*pol1.GetParError(1))
        EvsZ.Draw()
    #c1.Print(dirname+".pdf")
        return val,err
    except:
        print "Problem analysing ",dirname
        raise

def asfloatarray(vec):
    return numpy.asarray(map(lambda x:float(x),vec))

materials=["iron","tungsten"]
Ekin=asfloatarray(range(150,300,50)+[270])
graphs=[]
allGraph=ROOT.TMultiGraph()
iCol=2
infile=ROOT.TFile("bragg.root")
for material in materials:
    print "---",material,"---"
    values=[]
    errors=[]
    for iEkin in Ekin:
        val=0
        err=0
        dirname="deuteron-"+str(material)+"-"+str(int(iEkin))
        try:
            val,err=doFile(infile,dirname)
        except:
            print "Error analysing",infile
            raise
        values.append(val)
        errors.append(err)
#        print iEkin,"MeV: (",'%.2f' % val,"mm +/-",'%.2f' % float(err/val*1000),u"\u2030"
        print iEkin
        print "/PolarimeterStudies/detector/absorberlength ","%.1f"%val
        #print "Optimum thickness for",iEkin,"MeV :",thick,"mm"
    graph=ROOT.TGraphErrors(len(Ekin),Ekin,asfloatarray(values),asfloatarray(len(Ekin)*[0]),asfloatarray(errors))
    graph.SetName(material)
    graph.SetTitle(material)
    graph.GetXaxis().SetTitle("E_{kin} / MeV")
    graph.GetYaxis().SetTitle("Thickness / mm")
    graph.SetLineColor(iCol)
    graph.SetMarkerStyle(24)
    graph.SetFillColor(iCol)
    graph.SetLineWidth(3)
    iCol+=2
    allGraph.Add(graph)
allGraph.Draw("ALP")
allGraph.GetXaxis().SetTitle("E_{kin} / MeV")
allGraph.GetYaxis().SetTitle("Thickness / mm")
c1.BuildLegend(0.4,0.67,0.65,0.88)
ROOT.gPad.Modified()
c1.Print("c1.pdf")
c1.Print("c1.root")
#main()
