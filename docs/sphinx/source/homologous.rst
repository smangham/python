The homolgous wind model
############################################################

In the homolgous model the wind/outflow is assumed to have spherical
symmetry and to have a particularly simply velocity law: specifically,
homolgous expansion

.. math::
    v \propto r

This sort of velocity law has the advantage of being very simple to
work with, and is generally a good approximation for supernovae.

In pracise, the outflow (wind) is assumed to extend from an inner
radius :math:`r_{min}` to an outer radius :math:`r_{rmax}`. The
physical idea here is not necessarily that the wind stops at the
maximum radius, but rather that it is sufficiently dilute that
spectrum formation beyond this point becomes unimportant.

In our implementation, the specifics of the velocity law are
determined by giving the outflow speed at :math:`r_{min}` via a
parameter :math:`v_{base}`. It then follows that the velocity at all
other points in the wind is

.. math::
    v = v_{base} \frac{r}{r_{min}}

The density in the wind is determined by setting a mass flux at the
inner boundary ( :math:`boundary\_mdot`, in solar masses per year). The
variation of the density at larger radii is the controlled by an
exponent (:math:`density\_exponent`) such that

.. math::
    \rho \propto r^{- density\_exponent}

