#!/usr/bin/python
import os
import argparse
parser = argparse.ArgumentParser(description='Help=Read tka data and plot it.')
parser.add_argument("file",help="Choose input file")
args=parser.parse_args()
def read_dat(file):
    f=open(file,'r')
    ii=0
    data=[]
    for line in f:
        datum=[]
        for part in line.split():
            datum.append(part)
        data.append(datum)
    return data
def main():
        from ROOT import TCanvas, TGraph
        from numpy import asarray,array
        data=read_dat(args.file)
        axis=data[0]
        points=array(data[1:])
        #print points[:,0]
        matscan=TGraph(len(points),asarray([float(i) for i in points[:,0]]),asarray([float(i) for i in points[:,3]]))
        matscan.GetXaxis().SetTitle(axis[0])
        matscan.GetYaxis().SetTitle(axis[3])
        c1=TCanvas("matscan","matscan",800,600)
        matscan.Draw("ALP")
        matscan.DrawPanel()
        raw_input()
        c1.Print("matscan.pdf")
main()
