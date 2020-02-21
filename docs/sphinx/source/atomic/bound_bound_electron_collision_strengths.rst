Bound-bound electron collision strengths
########################################

Source
======
We use the Chianti atomic database, specifically the \*.scups files. These "contain the effective electron collision strengths scaled according to the rules formulated by `Burgess & Tully 1992, A&A, 254, 436 <http://articles.adsabs.harvard.edu/full/1992A%26A...254..436B>`_
The values in the file are functional fits to :math:`\Upsilon(T)`$ which we referred to as :math:`\Omega` in our calculations for collisional de-excitation rate coefficient


:math:`q_{21}=\Omega\frac{8.629\times10^{-6}}{g_u\sqrt{T}}`

In the g-bar formulation

:math:`\Omega=4.77\times10^{16}g_l\overline{g}\frac{f_{abs}}{\nu}`

These values of :math:`\Upsilon` simply replace :math:`\Omega`.

Translation to Python format
============================

It is necessary to link each line in our line list with the relevant electron collision strength. This is achieved using the python script "coll_stren_lookup.py" which first reads in the  "lines_linked_ver_2.py" line list, then attempts to work out which lines are which by comparing the energy and the oscillator strength of the line. If these match to within a factor of 10% then the code logs this as a possible match. If better matches come along, then the code adopts those instead.

Each matched line get a line in the data file which is basically all of the line data for the matched line. This is to give Python the best chance of linking it up with the line internally.

Data format
===========

Each record has three lines. The first line has a keyword CSTREN and this contains all the line data for the line to which this collision strength refers as the first 10 fields. These fields are identical to the 10 fields that appear in a standard line file. The next 8 fields are

- 1-2 Upper and lower level of transition - Chianti nomenclature
- 3 Energy of transition - Rydberg
- 4 Oscillator strength x lower level multiplicity (GF)
- 5 High temperature limit value
- 6 Number of scaled temperatures - 5 or 9
- 7 Transition type \cite{1992A&A...254..436B} nomenclature
- 8 Scaling parameter (C) (Burgess & Tully 1992) nomenclature

The next two lines, with labels SCT and SCTUPS are the 5 or 9 point spline fits to  :math:`\Upsilon` vs T
in reduced units y,x.

There are four different types of transitions, each with their own scaling between temperature of the transition and $\Upsilon$

For example, for type 1 (the most common)

:math:`x=1-\frac{lnC}{ln\left(\frac{kT}{E_ij}+C\right)`

and

:math:`y(x)=\frac{\Upsilon}{ln\left(\frac{kT}{E_{ij}}+e\right)`

So, to get :math:`\Upsilon` for a given T, one converts T to x via the correct equation,then linearly interpolate between values of y(x), then convert back to $\Upsilon$

Python structure
================

The data is stored in Python in the Coll\_stren structure which has memebers


- int n - internal index
- int lower,upper - the Chianti levels, not currently used
- double energy - the energy of the transition
- double gf - the effective oscillator strength - just oscillator strength x multiplicity
- double hi_t_lim - the high temperature limit of y
- double n_points -The number of points in the spline fit
- int type - The type of fit, this defines how one computes the scaled temperature and scaled coll strength
- float scaling_param - The scaling parameter C used in the Burgess and Tully calculations
- double sct[N_COLL_STREN_PTS] -The scaled temperature points in the fit
- double scups[N_COLL_STREN_PTS]- The sclaed coll sttengths in ythe fit


There is also a member in the line structure (coll_index) which points to the relevant record

comments
========

This data has been generated to match the Verner line list. If we want to use the Kurukz line list, then a new set of data should be made
