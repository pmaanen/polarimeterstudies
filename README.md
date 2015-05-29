
polarimeterstudies
==================
README

This is the standard simulation of for the EDM polarimeter.

Command line options:

--detector.geometry -g: Choose geometry (file). Currently accepted formats are gdml:file cubic:[file] hexagonal:[file] single: sandwich:

--general.num_threads -n: number of worker threads

--general.config_file -c: config file

--general.macro_file -m: macro file

--general.batch_mode -b: choose if batch mode or graphical



===GEOMETRY===
gdml:file load gdml file. 
cubic:[file]
===HITS===
Todo
===GENERATOR===
you can set generator mode by /PolarimeterStudies/generator/Mode N
enum GeneratorMode {GUN=1,INPUTFILE=2,GENERATE=3,DCELASTIC=4,DCBREAKUP=5,MUON=6};
GUN: Standard particle gun
INPUTFILE: inputfile in the format eventid particleid id px/GeV py/GeV pz/GeV [vx/mm] [vy/mm] [vz/mm] [t/s]
GENERATE: generate from phase space.
DCELASTIC: dc elastic scattering with the model taken from Ed's note
DCBREAKUP: the same for dc->pnnc from the same note
MUON: muon gun with model from PDG booklet