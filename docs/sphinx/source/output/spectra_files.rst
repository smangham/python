Spectra Files
#############

Python is intended to produce simulated spectra.  These spectra are all ascii tables intended to be accessible with software packages such as astropy.

All of the ascii begin with commented headers that contain all of the parameters of associated with a run,
along with the date of the run and the specific version of Python used to make the run.
In principle, if one still has access to any of the spectra, one can reproduce the entire run again.

Broad band spectra are created from the last ionization cycle.  Detailed are calculated from all of the spectral cycles.

For a model with root name *cv*, the following broadband spectra will be created:

* **cv.spec_tot** - various spectra
* **cv.log_spec_tot**
* **cv.spec_tot_wind**
* **cv.log_spec_tot_wind**

File types
==========

.spec_tot
  An ascii file that contains various spectra from the ionization-calculation phase of the program on a linear frequency scale.
  The first few lines of the file (omitting the header) are as follows:

  .. code::

     # Freq.        Lambda  Emitted  Star+BL  Disk     Wind     HitSurf  Scattered
     3.023938e+14 9913.975 1.07e+33 2.03e+31 1.05e+33 1.05e+30 4.11e+31        0
     3.049952e+14 9829.418  1.1e+33 2.24e+31 1.07e+33 3.97e+30 4.42e+31        0
     3.075965e+14 9746.292 1.09e+33  2.1e+31 1.07e+33 1.22e+30 3.63e+31        0
     3.101978e+14 9664.559 1.11e+33 1.97e+31 1.09e+33 1.33e+30 4.34e+31        0
     3.127991e+14 9584.186 1.08e+33 2.03e+31 1.06e+33 1.27e+30 4.75e+31        0

  The first two columns are fairly obvious. Lambda is in Angstroms. The remainder indicate the luminosity of the system in specific bands. Emitted is the total emergent spectrum, Star+BL is the emergent spectrum from photons bundles originating on the Star or BL, Disk and Wind are the same for photons originating in the disk and wind respectively. HitSurf represents photons that did not escape the system but ran into a boundary, and Scattered are photons that somewhere along their path out of the system were actually scattered.

.log_spec_tot
  An ascii file which contains the same information as *.spec_tot*, but with a logarithmically space frequency intervals.
  This gives better sampling of the SED in a lot of cases and is much better for plotting things such as the input spectrum.

.spec_tot_wind
  Identical to *.spec_tot* but just including photons that were generated in the wind or scattered by the wind

.log_spec_tot_wind
  A logarithmic version of *.spec_tot_wind*

.spec
  an ascii file that contains the final detailed spectra for the wavelengths of interest at a distance of **100 pc**.

  Photons bundles are generated in cycles in python and the *.spec* file is actually written out at the end of each cycle
  as the program is running in the spectrum-generation phase of the program. So one can inspect the spectrum as it is building up.

  The beginning of the file (omitting the header) is as follows:

  .. code::

     Freq.        Lambda    Created  Emitted   CenSrc     Disk     Wind  HitSurf Scattered A10P0.50 A28P0.50 A45P0.50 A62P0.50 A80P0.50
     1.620713e+15 1849.757  3.8401e-12 3.6348e-12 9.1429e-14 3.5434e-12          0 8.8693e-14 1.7753e-13 9.2741e-12 7.6342e-12 6.3434e-12 2.3932e-12  9.382e-13
     1.620925e+15 1849.514  4.8471e-12 4.7931e-12 2.7382e-13 4.4306e-12 8.8704e-14 1.8213e-13 2.4885e-13 1.0177e-11 7.7666e-12 3.2906e-12 3.4296e-12 1.3389e-12
     1.621138e+15 1849.272  5.3058e-12  5.182e-12 9.1477e-14 4.9992e-12 9.1404e-14  2.674e-13 3.5847e-13 1.2354e-11 6.9236e-12 5.9863e-12 3.3748e-12 1.7905e-12
     1.621351e+15 1849.029  3.9346e-12 3.9028e-12          0 3.8127e-12 9.0124e-14 8.9142e-14 2.6728e-13 1.1158e-11 6.4932e-12 5.1452e-12 3.9074e-12 8.1597e-13

  where the first line indicates the version of python used to generate the spectrum,
  the second gives a brief description of each column, and the remainder of the file is the spectrum.
  The most important columns are 1 and 2, which are respectively the frequency and wavelength and the columns that begin with,
  which give the spectrum that would be observed from the object at various inclination angles and orbital phases
  (The number of columns is therefore variable depending on how many inclinations one asked for in the input files).
  The other columns Emitted, Star+BL, Disk, Wind, HitSurf and Scattered have approximately the same meaning as in the *.spec_tot* file.

The remaining files parallel those generated for the broadband spectra.
