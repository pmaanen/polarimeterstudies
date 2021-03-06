#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../TestBeam2016BMaterialInfluence-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g testbeam2016b -m scripts/TestBeam2016BMaterialInfluence.mac -n $N -b 1>/dev/null
mv *.root $DIR/
cp scripts/AnalysisBase.py $DIR/.
cp libAnalysis* $DIR/.
cp scripts/TestBeam2016BFOM.py $DIR/.
cd $DIR
./TestBeam2016BMaterialInfluence.py -o materialinfluence.root -n $N *root 
