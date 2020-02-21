Central_object.geometry_for_source
==================================
If the central source in an AGN/BH system is radiating, what geometry should it radiate from?
 This is applicable even for black-body sources, where the *luminosity* depends on :ref:`Central_object.radius`.

Type
  Enumerator

Values
  lamp_post
    The central source radiates from two point sources
    located on the system axis above and below the disk plane.
    Emission is completely isotropic.

  sphere
    The central source radiates from a spherical surface with radius :ref:`Central_object.radius`.
    Emission is cosine-weighted in the direction of the sphere normal at the point of emission.
    Photons that would be spawned in an extended disk (if :ref:`Disk.type` is `vertically.extended`)
    are re-generated.


File
  `setup_star_bh.c <https://github.com/agnwinds/python/blob/master/source/setup_star_bh.c>`_


Parent(s)
  * :ref:`System_type`: ``agn``, ``bh``

  * :ref:`Central_object.radiation`: ``True``


Child(ren)
  * :ref:`Central_object.lamp_post_height`

