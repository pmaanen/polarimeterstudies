#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
mkdir sim
mkdir sim/de-dx-by-angle
mkdir sim/de-dx-by-length
./polarimeterstudies -g testbench: -m scripts/de-dx.mac -n $N -b 2>/dev/null 1>/dev/null
cp ./scripts/analyse.py sim/de-dx-by-angle/.
cp ./scripts/analyse.py sim/de-dx-by-length/.
cd sim/de-dx-by-length && ./analyse.py 15 30 45 60 75 && cd -
cd sim/de-dx-by-angle  && ./analyse.py $(seq 30 5 100) && cd -
