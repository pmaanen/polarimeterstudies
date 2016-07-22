#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
DIR=../hodoscope-energy-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g single -m scripts/hodoscope-energy.mac -n $N -b
cp ./scripts/hodoscope-energy.py $DIR
cp ./scripts/AnalysisBase.py $DIR
cp libAnalysis* $DIR
mv *root $DIR
cd $DIR  && nice -n 15 ./hodoscope-energy.py -n $N -o energy.root *.root && cd -
