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
./polarimeterstudies -g cosmic: -m scripts/de-dx-by-angle.mac -n $N -b 2>/dev/null 1>/dev/null
./polarimeterstudies -g cosmic: -m scripts/de-dx-by-length.mac -n $N -b 2>/dev/null 1>/dev/null
cp ./scripts/analyse.py sim/de-dx-by-angle/.
cp ./scripts/analyse.py sim/de-dx-by-length/.
./sim/de-dx-by-length/analyse.py
./sim/de-dx-by-length/analyse.py
