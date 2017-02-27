#!/bin/bash
N="4"
NAME=TestBeam2016bIsolation
if [ $# -eq 1 ]
  then
    N="$1"
fi
DIR=../$NAME-$(date "+%Y-%m-%d")
mkdir -p $DIR
nice -n 15 ./polarimeterstudies -g testbeam2016b --physics.use_fast_sim -m scripts/$NAME/init.mac -n $N 1>out.log 2>err.log
mv *.root $DIR/
mv *log* $/DIR/
cp 
cp scripts/AnalysisBase.py $DIR/.
cp *libAnalysis* $DIR/.
cp scripts/$NAME/* $DIR/.
cd $DIR
./$NAME.py -o $NAME.root -n $N *root
mail -s "$NAME finished at $(date)" maanen@physik.rwth-aachen.de </dev/null