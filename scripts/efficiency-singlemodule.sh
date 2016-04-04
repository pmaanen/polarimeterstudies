#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
mkdir -p ~/sim/efficiency
./polarimeterstudies -g single -m scripts/efficiency-lyso.mac -n $N -b
./polarimeterstudies -g singlesandwich -m scripts/efficiency-plastic.mac -n $N -b
find ./ -name "*root" -exec mv {} ~/sim/efficiency