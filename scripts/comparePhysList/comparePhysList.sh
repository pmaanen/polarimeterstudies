#!/bin/bash
N="4"
if [ $# -eq 1 ]
  then
    N="$1"
fi
nice -n 15 ./polarimeterstudies --general.physics QGSP_BERT -g single -m scripts/test_elastic.mac -n $N -b -o elastic_QGSP_BERT.root 1>/dev/null
nice -n 15 ./polarimeterstudies --general.physics QGSP_BIC -g single -m scripts/test_elastic.mac -n $N -b -o elastic_QGSP_BIC.root 1>/dev/null
nice -n 15 ./polarimeterstudies --general.physics QGSP_INCLXX -g single -m scripts/test_elastic.mac -n $N -b -o elastic_QGSP_INCLXX.root 1>/dev/null
