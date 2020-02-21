System Description
##################

The first set of parameters which Python needs are information about the overall system

.. code::

   System_type(star,cv,bh,agn,previous)                   bh

   ### Parameters for the Central Object
   Central_object.mass(msol)                        10
   Central_object.radius(cm)               8.85667e+06
   Binary.mass_sec(msol)                           15
   Binary.period(hr)                               72

   ### Parameters for the Disk (if there is one)
   Disk.type(none,flat,vertically.extended)                 flat
   Disk.radiation(yes,no)                          yes
   Disk.rad_type_to_make_wind(bb,models)                   bb
   Disk.temperature.profile(standard,readin)             standard
   Disk.mdot(msol/yr)                            1e-6
   Disk.radmax(cm)                                1e13

   ### Parameters for Boundary Layer or the compact object in an X-ray Binary or AGN
   BH.radiation(yes,no)                            yes
   BH.rad_type_to_make_wind(bb,models,power,cloudy,brems)                power
   Boundary_layer.lum(ergs/s)              4.72063e+39
   Boundary_layer.power_law_index                 -1.5

:ref:`System_type` is starting point, a basic classification of the type of object one is trying to model.
This is used to guide further questions about the object and to set defaults.

Most of the other parameters are fairly self-explanatory, and are documented fully in the various Parameters entries.
