#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
export PYTHONPATH+=$(pwd)/scripts
mkdir -p ~/sim/de-dx
./polarimeterstudies -g testbench: -m scripts/de-dx.mac -n $N -b -o
find ./ -name "*root" -type f -d 1 -exec {} mv ~/sim/de-dx \;
cp ./scripts/analyse-de-dx.py ~/sim/de-dx/.
cp ./scripts/AnalysisBase.py ~/sim/de-dx/.
cd ~/sim/de-dx && ./analyse-de-dx.py -n $N -o dedx.root *root && cd -
