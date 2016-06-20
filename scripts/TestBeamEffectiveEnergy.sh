#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../TestBeamEffectiveEnergy-$(date "+%Y-%m-%d")
mkdir -p $DIR
./polarimeterstudies -g testbeam -m scripts/TestBeamEffectiveEnergy.mac -n $N -b 1>/dev/null
#mv *.root $DIR/
#cp scripts/AnalysisBase.py $DIR/.
#cp libAnalysis* $DIR/.
#cp scripts/TestBeamEffectiveEnergy.py $DIR/.
#cd $DIR
#./TestBeamEffectiveEnergy.py -o energy.root -n 4 100.root 150.root 200.root 250.root 270.root