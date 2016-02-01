#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
mkdir -p sim/bragg
./polarimeterstudies -g single: -m scripts/bragg.mac -n $N -b 2>/dev/null 1>/dev/null
cp ./scripts/analyse-bragg.py sim/bragg/.
cp ./scripts/CalculateLateral.py sim/bragg/.
cp ./scripts/CalculateThickness.py sim/bragg/.
cd sim/bragg  && ./analyse-bragg.py *.root && cd -
cd sim/bragg  && ./CalculateThickness.py *.root && cd -
cd sim/bragg  && ./CalculateLateral.py *.root && cd -