
polarimeterstudies
==================
README

This is the standard simulation of for the EDM polarimeter.

Command line options:

--detector.geometry -g: Choose geometry (file). Currently accepted formats are gdml:file cubic:[file] hexagonal:[file] single: sandwich:

--general.num_threads -n: number of worker threads

--general.config_file -c: config file

--general.macro_file -m: macro file

--general.batch_mode -b: flag for batch mode



===GEOMETRY===

gdml:file load gdml file.

cubic:[file]

===HITS===

Todo

===GENERATOR===

You can set generator mode by /PolarimeterStudies/generator/Mode N

The following modes are available:

1: Particle Gun

2: Read from input file in the format  eventid particleid id px/GeV py/GeV pz/GeV [vx/mm] [vy/mm] [vz/mm] [t/s]

3: Built-In generator

The following generators are available

GENERATE: generate from TGenPhaseSpace

DCELASTIC: dc elastic scattering with the model taken from Ed's note

dcelastictime: dc elastic scattering with the model taken with
intensity and polarization

DCBREAKUP: dc->pnc from the same note
MUON: muon gun with model from PDG booklet
