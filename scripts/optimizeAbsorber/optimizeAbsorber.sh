#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
DIR=../optimizeAbsorber-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g singlesandwich -m scripts/optimizeAbsorber.mac -n $N -b
cp ./scripts/analyse-bragg.py $DIR
cp ./scripts/CalculateLateral.py $DIR
cp ./scripts/CalculateThickness.py $DIR
cp ./scripts/CalculateRange.py $DIR
cp ./scripts/AnalysisBase.py $DIR
cp libAnalysis* $DIR
find ./ -name "*root*" -exec mv {} $DIR \;
cd $DIR  && nice -n 15 ./analyse-bragg.py -n $N -o bragg.root *.root && ./CalculateThickness.py && cd -
