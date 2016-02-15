"""Start of the analysis"""

# ROOT interfers with argv -> import it later
# see: http://root.cern.ch/phpBB3/viewtopic.php?t=8501
import argparse
import pickle
import time
from os.path import dirname, abspath
import sys

parser = argparse.ArgumentParser()
tmpargv = sys.argv[:]    # [:] for a copy, not reference
sys.argv = []

from ROOT import TProof, TFileCollection, TChain
sys.argv = tmpargv

proof = TProof.Open('')

# giving some name to collection to shut up warning
fc = TFileCollection('analysis_files')

chain = TChain("Calorimeter")
for filename in sys.argv[1:]:
    fc.Add(filename)
    chain.AddFile(filename)
chain.SetProof()    

# add the current folder to the python path of root (or something)
proof.Exec('TPython::Exec("%s");' %
        ("import sys; sys.path.insert(0,'"+dirname(abspath("AnalysisBase.py"))+"')"))

time.sleep(1)  # needed for GUI to settle

proof.Process(fc, 'TPySelector', 'AnalysisBase')
#chain.Process('TPySelector', 'AnalysisBase')
