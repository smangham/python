
/***********************************************************/
/** @file   setup_disk.c
 * @author ksl
 * @date   January, 2018
 *
 * @brief  Read parameters that define a disk
 *
 * File containing reverberation mapping functions.
 ***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "atomic.h"
#include "python.h"


/***********************************************************
             University of Southampton

Synopsis:
  get_disk_params sets up the disk parameters according to user inputs,
  e.g. the temperature profile, accretion rate etc.

Arguments:

Returns:

Description:

Notes:

History:
	1502  JM 	Moved here from main()
	1510	ksl	Modified to restore illumination
			options, which were brokedn
    1712    ksl Collected input parameters for disk into
                a single routine and modified some of the
                input fil names to be more uniform

**************************************************************/

/**********************************************************/
/**
 * @brief       get the parameters need to define a disk
 *
 * @param [in] None
 * @return
 *
 * Read the parameters, such as the type of disk, the
 * temperature profile, that define a disk
 *
 * The parameters fill variables defined in the geo
 * data structure.
 *
 * The routine itself simple returns 0
 *
 * ###Notes###
***********************************************************/


double
get_disk_params ()
{
  char values[LINELENGTH], answer[LINELENGTH];

  rdpar_comment ("Parameters for the Disk (if there is one)");

  strcpy (answer, "flat");

  if (geo.system_type == SYSTEM_TYPE_STAR)
  {
    strcpy (answer, "none");
  }
  sprintf (values, "%d,%d,%d", DISK_NONE, DISK_FLAT, DISK_VERTICALLY_EXTENDED);
  geo.disk_type = rdchoice ("Disk.type(none,flat,vertically.extended)", values, answer);


  if (geo.disk_type == DISK_NONE)
  {
    geo.disk_radiation = 0;
    geo.diskrad = 0;
    return (0);
  }


  strcpy (answer, "yes");
  geo.disk_radiation = rdchoice ("Disk.radiation(yes,no)", "1,0", answer);

  if (geo.disk_radiation)
    get_spectype (geo.disk_radiation,
                  //"Disk.rad_type_to_make_wind(0=bb,1=models)", &geo.disk_ion_spectype);
                  "Disk.rad_type_to_make_wind(bb,models)", &geo.disk_ion_spectype);


  geo.disk_tprofile = DISK_TPROFILE_STANDARD;

  strcpy (answer, "standard");
//OLD  sprintf (values, "%d,%d,%d", DISK_TPROFILE_STANDARD, DISK_TPROFILE_READIN, DISK_TPROFILE_YSO);
//OLD  geo.disk_tprofile = rdchoice ("Disk.temperature.profile(standard,readin,yso)", values, answer);
  sprintf (values, "%d,%d", DISK_TPROFILE_STANDARD, DISK_TPROFILE_READIN);
  geo.disk_tprofile = rdchoice ("Disk.temperature.profile(standard,readin)", values, answer);

  if (geo.disk_tprofile == DISK_TPROFILE_STANDARD)
  {
    geo.disk_mdot /= (MSOL / YR);       // Convert to msol/yr to simplify input
    rddoub ("Disk.mdot(msol/yr)", &geo.disk_mdot);
    geo.disk_mdot *= (MSOL / YR);
  }
  else if (geo.disk_tprofile == DISK_TPROFILE_READIN)
  {
    rdstr ("Disk.T_profile_file", files.tprofile);
    geo.disk_mdot = 0;
  }
  else
  {
    geo.disk_mdot = 0;
  }

  /* Set a default for diskrad for an AGN */
  if (geo.system_type == SYSTEM_TYPE_CV)
  {
    geo.diskrad = diskrad (geo.mstar, geo.m_sec, geo.period);
  }
  else if (geo.system_type == SYSTEM_TYPE_AGN || geo.system_type == SYSTEM_TYPE_BH)
  {
    geo.diskrad = 100. * geo.rstar;
  }

  rddoub ("Disk.radmax(cm)", &geo.diskrad);
  Log ("geo.diskrad  %e\n", geo.diskrad);

  geo.diskrad_sq = geo.diskrad * geo.diskrad;

/* If diskrad <= geo.rstar set geo.disk_type = DISK_NONE to make any disk transparent anyway. */

  if (geo.diskrad < geo.rstar)
  {
    Log ("Disk radius is less than star radius, so assuming no disk)\n");
    geo.disk_type = DISK_NONE;
  }

  if (geo.disk_type == DISK_VERTICALLY_EXTENDED)
  {                             /* Get the additional variables need to describe a vertically extended disk */
    rddoub ("Disk.z0(fractional.height.at.diskrad)", &geo.disk_z0);
    rddoub ("Disk.z1(powerlaw.index)", &geo.disk_z1);
  }
  return (0);
}
