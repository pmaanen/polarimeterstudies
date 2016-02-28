#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
export PYTHONPATH+=$(pwd)/scripts
mkdir -p ~/sim/de-dx-by-angle
mkdir -p ~/sim/de-dx-by-length
mkdir -p sim/de-dx-by-angle
mkdir -p sim/de-dx-by-length
./polarimeterstudies -g cosmic: -m scripts/de-dx.mac -n $N -b
mv sim/de-dx-by-angle ~/sim/
mv sim/de-dx-by-length ~/sim/
cp ./scripts/analyse-de-dx.py ~/sim/de-dx-by-angle/.
cp ./scripts/analyse-de-dx.py ~/sim/de-dx-by-length/.
cd ~/sim/de-dx-by-length && ./analyse-de-dx.py && cd -
cd ~/sim/de-dx-by-angle  && ./analyse-de-dx.py && cd -
