Wind Models
###########

Python allows for various types of models, which are defined by the following parameters

.. code::

   ### Parameters describing the various winds or coronae in the system
   Wind.radiation(yes,no)                          yes
   Wind.number_of_components                  1
   Wind.type(SV,star,hydro,corona,kwd,homologous,shell,imported)                   sv
   Wind.coord_system(spherical,cylindrical,polar,cyl_var)          cylindrical
   Wind.dim.in.x_or_r.direction               30
   Wind.dim.in.z_or_theta.direction           30

:ref:`Wind.radiation` (WHICH PROBABLY WILL BE MOVED) allows for wind not only to scatter and absorb photons,
but also to emit them by various processes, bound-bound, free-free, and recombination.  It is the default for simple radiative transfer.

:ref:`Wind.number_of_components` is usually 1, but can be greater if one wishes to construct a wind from a combination of several wind models,
for example a fast flow near the poles of a system, and a slow for near the disk.
If the number of components exceeds 1, then the remaining questions relating to the wind will be posed multiple times.

The wind models incorporated into Python currently are:

:ref:`SV`
  The Shlosman and Vitello parameterization of a bi-conical flow

:ref:`Stellar_wind`
  A fairly standard parameterization of a spherical outflow for a hot star

:ref:`hydro`
  A special purpose mode used by us for importing models from Zeus and Pluto

:ref:`Corona`
  A simple model for a corona above the disk

:ref:`KWD`
   The Knigge Woods and Drew parameterization of a bi-conical flow

:ref:`Homologous`
  A homologous expansion law useful for simulating SNe

:ref:`Shell`
  A model of a thin shell useful for diagnostic studies

:ref:`Imported <Importing models>`
  A general purpose mode for importing a wind from an ascii file

.. todo::

   Update paths as they move
