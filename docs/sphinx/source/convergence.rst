Convergence
###########

Ionization cycles in Python are intended to establish the correct ion densities and temperature of the various cells
in the wind.  The degree to which this happens for a given number of ionization cycles depends on how far the initial
guess of electrons temperatures in various portions of the wind and the number of photons generated during each
photoionization cycles.  Furthermore, the accuracy of the final model depends on the number of photons that pass through
each cell.  As a result, the accuracy with which ion abundances and temperature are determined  will differ on a cell
by cell basis. In a typical model with a biconical outflow, a small cells at the outer edge of the accretion disk
will record fewer photon passages than one in the middle of the grid that is exposed to large numbers of photons
from the disk.

To estimate whether a a model calculation has reached a steady state, Python carries out three tests, one comparing the
difference in the radiation temperature between the current and the preceding ionization cycle, one comparing the electron
temperature in the same manner and once comparing heating and cooling rates in the current cycle.  More specifically,  if
a cell satisfies the following 3 tests,

.. math ::
    \left | \frac{t_e^n-t_e^{n-1}}{t_e^n+t_e^{n-1}} \right | < \epsilon

.. math ::
    \left | \frac{t_r^n-t_r^{n-1}}{t_r^n+t_r^{n-1}} \right | < \epsilon


.. math ::
    \left | \frac{Heat_{tot}^n-Cooling_{tot}^{n}}{Heat_{tot}^n+Cooling_{tot}^{n}} \right | <\epsilon



where :math:`\epsilon = 0.05`, then cell is said to have converged.


The routine ``run_check.py`` produces two plots related to convergence, one showing the fraction of cells that have passed
each of the tests above as a function of cycle, and the other showing the number of failed checks for each cell in the wind.
