Evaluation
##########

Determining whether Python has run successfully from a a scientific point of view depends very specifically on one's goals.  Did the spectra turn out to be what one expected?  Here by evaluation we mean, did my run complete without significant errors and did the ionization structure converge to a "steady state" given the number of ionization cycles, the number of photons, and the frequency distributions of the photons we chose.

Convergence
===========

A very basic question about a particular run is,  has it reached a "steady state" and if it is in a steady state are cells stable in the sense that fluctuations are small.

.. todo::

   Define what converged means here, and provide examples

Note that it is not always important that all cells be converged.
The Monte Carlo process preferentially picks out the cells which affect the emergent radiation field.
Portions of the grid which do not get many photons are typically the ones that are "not converged",
but since they don't contribute much to the emergent radiation, one does not need them to be converged
(except if one wants to make nice plots of the temperature as a function of position in the wind or of the density of a particular ion species).
On the other hand, if one is using Python in conjunction with a hydrodynamical code one wants all the cells to be converged.


Errors
======

Python is designed to continue to run unless something catastrophic happens.
As it runs, it logs error messages that can be found in the .diag files.
These messages are a combinations of warnings, and/or unusual occurrences,
that if they start occurring often suggest a real problem.

These error messages are all of the form:

.. code::

   Error: wind2d: Cell   0 ( 0, 0) in domain 0 has 1 corners in wind, but zero volume

that is they begin with the word :code:`Error`. followed by the subroutine in the code where the error occurred followed by what is hopefully a helpful.
If one is concerned about a particular message, one can hopefully determine what is happening by looking for that message in the log files.

Python keeps a count of the number of times a particular message has occurred and at the end of the program, and the very end of the
diag files contain a listing of how many times a particular error has occurred.

.. code::

   Error summary: End of program, Thread 2 only
   Recurrences --  Description
        7 -- getatomic_data: line input incomplete: %s
      128 -- get_atomicdata: Could not interpret line %d in file %s: %s
        1 -- error_count: This error will no longer be logged: %s
        1 -- get_wind_params: zdom[ndom].rmax 0 for wind type %d
        1 -- wind2d: Cell %3d (%2d,%2d) in domain %d has %d corners in wind, but zero volume
        1 -- check_grid: velocity changes by >1,000 km/s in %i cells
        1 -- check_grid: some cells have large changes. Consider modifying zlog_scale or grid dims

As indicated here, these are the errors for only thread 2 of a program.
In order to get a summary of all the threads, there is a script py_error.py that be run as :code:`py_error.py rootname` from the main run directory.
Note that in many cases, the summary will be the number times an error occurred in one thread times the number of threads, but not always.

One should be aware of these errors, and watch out for situations where the number of errors  of a particular type is much larger than usual.
