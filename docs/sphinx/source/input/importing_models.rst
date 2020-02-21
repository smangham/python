.. imported:

Importing Models
################

Python can read 1D or 2.5D grids of density and velocity, instead of setting up the model from an analytic prescription. Caution should be exercised with this mode, as it is still in a development phase, and the mode requires the user to ensure that things like mass and angular momentum conservation are enforced.

This mode is activated via wind type option "imported", which triggers an extra question, e.g.

.. code::

   Wind.type(SV,star,hydro,corona,kwd,homologous,shell,imported)             imported
   Wind.coord_system(spherical,cylindrical,polar,cyl_var)          cylindrical
   Wind.model2import                    cv.import.txt

An example in cylindrical geometry, :code:`cv_import.pf`, is given with a supplementary grid file in :code:`examples/beta/`.
The format expected in the grid input file for such
a cylindrical model is as follows, although the column headers lines are actually not read.

.. code::

   i  j  inwind    x      z     v_x   v_y   v_z     rho   t_r
   -- -- ------  -----  -----  ----- ----- -----   ----- -----
   0  0    -1    1.4e9  3.5e9   0.0   0.0   6e5     0.0   0.0
   0  1     0    1.4e9  3.5e10  1e5   0.0   2e6     1e9   0.0

where all physical units are CGS. i and j refer to the rows and
columns of the wind cells respectively, while inwind tells the code whether
the cell is in the wind (:code:`0`), or out of the wind (:code:`-1`). If a
partially in wind flag is provided (:code:`1`), the code defaults to treating this
cell as not in the wind. This could in principle be adapted, but means that for the moment
this mode is most useful when using models with sufficiently high resolution or covering factors
that partially in wind cells
are unimportant.

The other input files have slightly different formats.  The best way to see the format is use the process described at the end of the page.

Creating your own model
=======================

In order to create your own model, there are a few important things to consider:

* all units should be CGS (except for indices and flags, which are integers)
* x and z for cylindrical (or r and theta for spherical polar) coordinates are supplied at the edges, rather than centres, of cells. Thus, a given cell is described by the location of it's bottom left hand corner in (x,z) space.
* Ghost cells **must** be included. This means that additional rows and columns of cells must be included at the edges of the grid, and they must be excluded from the wind so that their velocities and densities are set to zero, but have a velocity that python can interpolate with.
* i and j correspond to rows and columns respectively, so that the first row of cells at the disk plane has i = 0.
* rho the density of the cell in cgs units
* The t_r column is currently not used, although it could be in future

Although :code:`cv_import.pf` is designed to closely match the
:code:`cv_standard.pf` model, it does not match the model perfectly as
the imported model does not deal with 'partially in wind' cells. As such,
we generally recommend imported models are used for either wind models
that entirely fill the grid or that have sufficiently high resolution
that the partial filled cells are relatively unimportant.

Generating example inputs for testing and familiarizing oneself with Python's import capability
===============================================================================================

If one is trying to use the import capability of Python for the first time,
it will be useful to familiarize oneself with the process, and the file format for a particular coordinate system,
by running first running Python on a model that is something similar to model to be imported,
but which takes advantage of one of the kinematic models available with the code.

For example, suppose you have a hydrodynamical simulation of an AGN wind which
is in polar coordinates and you want to use Python to calculate the spectrum.
Then you might create a model of an AGN with a similar coordinate system using, say, a Knigge Wood & Drew wind (and similar atomic data).
For specificity, suppose this model has the root name "test"

Once you have run the model, you can create an import file file by first running the routine :code:`windsave2table`, or more specifically:

.. code:: bash

   windsave2table test

This produces a large number of ascii tables, which are described elsewhere

In the py_progs directory, you will find 3 scripts, :code:`import_1d.py`, :code:`import_cyl.py` and :code:`import_rtheta.py`,
which will convert one of the output files :code:`test.0.master.txt` to an import file, :code:`test.import.txt`,
that can be used with the import mode of Python. The 3 different routines are for 1d spherical coordinates,
and polar (r-theta) coordinates respectively.

Assuming the py_progs directory is in your PATH, and given that our example is for cylindrical coordinates, one would run:

.. code:: bash

   import_cyl.py test

At that point, you can test this import file, by modifying the first .pf file to import mode (imported).
Running Python on this file, will result in your being asked the name of the import file,
and give you a "baseline" to import the hydrodynamical simulation to work.

Note that one should not assume that spectra produced by the original run of Python and the run of the imported model will be identical.
There are several reasons for this:

First, in creating the original model, Python accounts for the possibility that some cells are partially in the wind.
This is not possible in the imported models. Only cells that are complete in the wind are counted.

Second, within Python, positions and velocities are assumed defined at the corners of cells, whereas densities are assumed to be cell centered.
If one provides a table where all of the quantities are at the same exact position (namely density is at the same position as x),
there will be a slight discrepancy between the way in model as calculated internally and as represented within Python.
