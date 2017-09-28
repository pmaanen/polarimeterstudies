
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

The following generators are available

gun: particle gun

infile: read from input file

dcelastic: dc elastic scattering with the model taken from Ed's note

dcelastictime: as above, polarization and intensity time dependent (hardcoded)

breakup: dc->pnc from the same note

muon: muon gun with model from PDG booklet

===KNOWN ISSUES===

Automatic change of sensitive detector type on geometry update does not work. Has to be done manually by command.

