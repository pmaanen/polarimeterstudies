#!/usr/bin/env bash
./polarimeterstudies -g single: -b -n 12 -m scripts/test_elastic.mac
./polarimeterstudies -g singlesandwich:-b -n 12 -m scripts/test_elastic.mac
./polarimeterstudies -g singles:-b -n 12 -m scripts/test_breakup.mac
./polarimeterstudies -g singlesandwich:-b -n 12 -m scripts/test_breakup.mac
cp scripts/analysis-cuts.py . 
python analysis-cuts.py $(ls | grep root)