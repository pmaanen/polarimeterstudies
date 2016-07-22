#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../SandwichEffectiveEnergy-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g testbeam -m scripts/SandwichEffectiveEnergy.mac -n $N
mv *.root $DIR/
cp scripts/AnalysisBase.py $DIR/.
cp libAnalysis* $DIR/.
cp scripts/SandwichEffectiveEnergy.py $DIR/.
cd $DIR
nice -n 15 ./SandwichEffectiveEnergy.py -o energy.root -n $1 100.root
