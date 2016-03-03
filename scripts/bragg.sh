#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
mkdir -p ~/sim/bragg
./polarimeterstudies -g single: -m scripts/bragg.mac -n $N -b
mv *root ~/sim/bragg
cp ./scripts/analyse-bragg.py ~/sim/bragg/.
cp ./scripts/CalculateLateral.py ~/sim/bragg/.
cp ./scripts/CalculateThickness.py ~/sim/bragg/.
cp ./scripts/CalculateRange.py ~/sim/bragg/.
cp ./scripts/AnalysisBase.py ~/sim/bragg
cp ./scripts/analyse-bragg.py ~/sim/bragg
cp ./libAnalysis* ~/sim/bragg
cd ~/sim/bragg  && ./scripts/analyse-bragg.py -o bragg -n $N $(ls | grep *root | grep -v histos) && cd -