Spectrum.select_scatters
========================
Advaned command that allows one to extract photons that
have undergone a certain number of scatters.  If n > MAXSCAT,
that is to say a very large number then all scatters are slected.
If lies between 0 and MAXSCAT then photons will be extracted only
at the point a photon has undergone this number of scatters.  If
n is < 0 then photons with n or greater scattters will be extracted.

Type
  Integer

Values
  Greater than 0

File
  `setup.c <https://github.com/agnwinds/python/blob/master/source/setup.c>`_


Parent(s)
  * :ref:`Spectrum.select_specific_no_of_scatters_in_spectra`: ``True``


