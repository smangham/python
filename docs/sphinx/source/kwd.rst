The Knigge, Wood, and Drew prescription of a bi-conical wind
############################################################

In the KWD paramterization for a bi-conical flow the wind is envisioned to have 
poloidal streamlines that all point to a position a distance d below the disk, as is
shown below:

.. todo:: This figure needs modification to account for the fact that we allow rmin and rmax to be specified.

.. figure:: images/kwd.png
    :width: 300px
    :align: center

As descried by KWD95, streamlines emerge thoughout the entire disk, with the innermost 
streamline just grazing the surface of the central object and the outermost streamline
emerging from the outer radius of the disk.  In the current version of Python, while this
is the default choice, the wind region can be restricted to streamlines that arise from 
between :math:`r_{min}` and :math:`r_{rmax}`.  For fixed values of  :math:`r_{min}` and 
:math:`r_{rmax}`, the wind will tend to be more collimated the larger the value of d.

In the KWD parameritization, the mass loss per unit area per unit area of the disk is given by 

.. math::
    \frac{\delta \dot{m}}{\delta A} \propto T(R)^{4\alpha}

where T(R) is the temperature of the disk at radius R.  With this parameterization, the 
mass loss rate per unit area is constant for :math:`\alpha=0` 
and is porportional to the luminoous flux is :math:`\alpha=1`.

The KWD95 model incorporates a velocity law reminiscent of a stellar wing, viz.

.. math::
    v_l=(nc_{s}) + (v_{\infty} - nc_{s})\left(1- \frac{R_{v}}{l+R_{v}}
    \right)^{\beta}

where :math:`nc_s` is the speed at the base of flow,a multiple of the sound speed, :math:`R_v` is the scale length, :math:`beta` 
is the exponent that determines the number of scale lengths over 
which the wind acclerates, and :math:`v_{\infty}` is defined as a multiple of
the escape velocity at the footpoint of each stream line. 

For the model, the sound speed of the disk is defined to be

.. math::
    c_s(R) = 10 \sqrt{\frac{T_{eff}(R)}{10^4 K}} km s^{-1}


