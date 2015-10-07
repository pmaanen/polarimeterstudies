#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
./polarimeterstudies -g cosmic: -m scripts/de-dx.mac -n $N -b 2>/dev/null 1>/dev/null
./scripts/analyse.py *.root
