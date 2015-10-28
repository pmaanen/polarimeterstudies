#!/usr/bin/bash
#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
echo "N="$N
mkdir -p sim/homogenity-muon
./polarimeterstudies -g cosmic: -m scripts/homogenity.mac -n $N -b 2>/dev/null 1>/dev/null
cp ./scripts/analyse.py sim/homogenity-muon