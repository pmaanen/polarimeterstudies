#!/bin/bash
N="4"
NAME=E22EnergyResolution
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../$NAME-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g testbeam2016b -m scripts/$NAME/$NAME.mac -n $N -b 1>/dev/null
mv *.root $DIR/
cp scripts/AnalysisBase.py $DIR/.
cp libAnalysis* $DIR/.
cp scripts/$NAME/$NAME.py $DIR/.
cd $DIR
./$NAME.py -o $NAME.root -n $N 
