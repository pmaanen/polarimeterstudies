#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../de-dx-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g testbench: -m scripts/de-dx.mac -n $N -b
mv *.root $DIR/.
cp ./scripts/analyse-de-dx.py $DIR/.
cp ./scripts/AnalysisBase.py $DIR/.
cp ./libAnalysis* $DIR/.
cd $DIR && nice -n 15 ./analyse-de-dx.py -n $N -o dedx.root *root && cd -
