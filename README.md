<<<<<<< HEAD
# Python

Python is a (confusingly named) Monte Carlo radiative transfer code which uses the Sobolev approximation.
It has been developed by Knox Long, Christian Knigge, Stuart Sim, Nick Higginbottom and James Matthews. 

The code is not yet ready for use and should not be installed yet. If you are interested in using
Python please contact long@stsci.edu.

## Travis Build Status

[![Build Status](https://travis-ci.org/agnwinds/python.png?branch=dev)](https://travis-ci.org/agnwinds/python)



## Installation

Python and the various routines associated are set up in a self-contained directory structure. The basic directory structure and the data files that one needs to run Python need to be retrieved and compiled. 

If you want to obtain a stable (!) release, go to the [Releases](https://github.com/agnwinds/python/releases) page.

If you want to download the latest dev version, you can zip up the git repository by clicking on the zip icon to the right of the GitHub page. Aternatively, you can clone the repository using 

    $ git clone https://github.com/agnwinds/python.git 

If you anticipate contributing to development we suggest Forking the repository and submitting pull requests with any proposed changes.

Once you have the files, you need to cd to the new directory and set your environment variables
    
    $ export PYTHON = /path/to/python/
    $ cd $PYTHON 
    $ ./configure
    $ make install
    $ make clean

note that export syntax is for bash- for csh use 
  
    $ setenv PYTHON /path/to/python/

Atomic data is stored in our [data repository](https://github.com/agnwinds/data) with it's own releases page. one should unzip these files and place them in a $PYTHON/data folder.

A development user may want to work on atomic data as part of their work, and pull in changes as they are made, in which case we recommend cloning the data repository:

    $ cd $PYTHON; git clone https://github.com/agnwinds/data data

## Running python

To run python you need to add the following to your $PATH variable:

    $PYTHON/bin

You can then setup your symbolic links by running 

    $ Setup_Py_Dir

and run the code by typing, e.g.

    $ py root.pf


Please see the [wiki](https://github.com/agnwinds/python/wiki/Installing-and-Running-Python) and docs folder for how to use the code.

Any comments, email jm8g08@soton.ac.uk or long@stsci.edu.
=======
## Description of Atomic Data for the MCRT Code Python

This is a directory which contains all of the various types
of data that one might want to use.  This includes spectral
models for use with python as well as atomic data and linelists


### Atomic Datasets

**Masterfiles:** These data sets have their masterfiles in the masterfiles folder. To use put data/masterfile in your parameter file.

* h10: 10 level Hydrogen only macro-atom
* h10_he20_standard73: 10 level Hydrogen macro-atom, 20 level Helium macro-atom, standard73 for simple ions 
* h10_hetop_standard73: 10 level Hydrogen macro-atom, 53 level Helium macro-atom topbase, standard73 for simple ions 
* h10_hetop_standard73: 10 level Hydrogen macro-atom, 53 level Helium macro-atom topbase, standard77 for simple ions 
* h10_standard73: 10 level Hydrogen macro-atom, standard73 for simple ions 
* h20: 20 level Hydrogen macro-atom only (Sim et al. 2005)
* h20_he20_standard73: 20 level Hydrogen macro-atom, 20 level Helium macro-atom, standard73 for simple ions 
* h20_hetop_standard73: 20 level Hydrogen macro-atom, 53 level Helium macro-atom topbase, standard73 for simple ions 
* h20_hetop_standard73: 20 level Hydrogen macro-atom, 53 level Helium macro-atom topbase, standard77 for simple ions 
* h20_standard73: 20 level Hydrogen macro-atom, standard73 for simple ions
* h3: 3 level Hydrogen only macro-atom 
* h4: 4 level Hydrogen only macro-atom
* standard39: standard data set using atomic 39 (Long & Knigge 2002, Noebauer et al. 2010)
* standard70: standard data set using atomic 39 - interim version of atomic data used during AGN development
* standard73: standard data set using atomic 39 (Higginbottom et al. 2013)
* standard77: standard data set using atomic 77- includes extrapolated XSections (Higginbottom et al. 2014)
* standard_sn_kurucz: dataset with Supernovae abundances, for use for comparison with Tardis.

**Folders:** All macro atom data contains in atomic_macro. Atomic data for the required version in atomicxx, i.e. standard77 uses atomic77.


### Disk/Stellar Models

To use put data/masterfile in your parameter file, e.g. data/model_kurucz91. 

* model_kurucz91: Kurucz model atmospheres. Grid goes to 50,000K and log(g) = 5
* kurucz_d14: Kurucz model atmospheres . Grid goes to 50,000K and log(g) = 5
* tlusty_d14: TLUSTY model atmospheres. Grid goes to 50,000K and log(g) = 5
* kur_tlusty_hybrid: Kurucz + TLUSTY hybrid model atmosphere's. Current benchmark SA grid for accretion disk.
* model_disk06: Disk 06 models
* model_lejeune: A set of stellar models from lejeune.  Not yet used. Their advantage is that they go further into the IR than kurucz

**Folders:** Masterfiles have same names as folders, except for the d14 and kur_tlusty_hybrid models which are contained in disk14 folder.


### Other Folders
Not all currently maintained.

* atomic_old: a collection of old atomic data, mainly various macro atom data which has not been used
			since python ~58 but may be used for reference and in future.

* calculate_atomic: this old folder should probably be archived here.

* hutdisk93: A set of stellar spectra calculated with TLUSTY/Synspec that KSL has used extenstively for calculations in the UV

### History:

04july -- ksl -- Created separate directories ../data44 and ../data50 that just contains
	the data needed to store the atomic files for running Python.  This directory
	contains the old data, as well as data that may not be need for standard runs
	of the program.  
	atomic49 is the version of the atomic datafile used up to and including python49
	atomic50 is a version that strips out a lot of the less used portions of the 
	atomic data.  
	Ultimately one may be able to discard both of these directoreis in favor of the
	datafiles contained in ../data44 and ../data50, but I was a little unsure that
	I had kept everything I needed for them.

1301 -- JM -- The data is now all in one folder (data). atomic data has the form 
	 atomic/atomicxx/standardxx where xx is two numbers.

1307 -- JM -- data folder still the same. standardxx now in top level directory,
	addresses bug #13

1405 -- JM -- reorganisation of data sets and updated readme. Now only one symbolic link is required, to the data folder.
>>>>>>> e186f1de7bdd2c0e33ab09d7aae52f3c0f27d583
