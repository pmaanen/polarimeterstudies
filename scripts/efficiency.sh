#!/usr/bin/env bash
nice -n 15 ./polarimeterstudies -g single: -b -n 12 -m scripts/test_elastic.mac
nice -n 15 ./polarimeterstudies -g singlesandwich:-b -n 12 -m scripts/test_elastic.mac
nice -n 15 ./polarimeterstudies -g singles:-b -n 12 -m scripts/test_breakup.mac
nice -n 15 ./polarimeterstudies -g singlesandwich:-b -n 12 -m scripts/test_breakup.mac
cp scripts/analysis-cuts.py . 
nice -n 15 python analysis-cuts.py $(ls | grep root)
