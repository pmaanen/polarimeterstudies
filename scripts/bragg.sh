#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
mkdir -p sim/bragg
./polarimeterstudies -g single: -m scripts/bragg-all.mac -n $N -b 2>/dev/null 1>/dev/null
cp ./scripts/analyse.py sim/bragg/.
cd sim/bragg  && ./analyse.py *.root && cd -
