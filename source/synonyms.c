
/***********************************************************/
/** @file  synonyms.c
 * @author ksl
 * @date   January, 2018
 *
 * @brief  Translate the keywords used in previous versions of Python to the current keywords
 *
 * The input variable used in Python have evolved over time.  The routine provided hre
 * is intened to make it possible to associte a new keyword with one that was used in 
 * an earlier version of Python.  As long as the keyword has simply been renamed then one 
 * can use a synomym to allow one to extact information from the old parameter file and translate
 * it so, one can use an old parmeter file with a new version of Python.  
 *
 * It is important to note that this process cannot continue indefinitely because , one may want
 * to change keyword/parameters so that they are not simple rplacements.
 ***********************************************************/



//OLD /**************************************************************************
//OLD                     Space Telescope Science Institute
//OLD 
//OLD 
//OLD   Synopsis:  
//OLD 
//OLD   This is a routine which is intended to help with program updates that
//OLD   change the name of a keyword in a parameter file.  It is called from
//OLD   rdpar.string_process_from_file
//OLD 
//OLD   Description:    
//OLD 
//OLD   The routine simply matches the string in new_question to a string in
//OLD   the array new_names, below.  If it finds a match, the old_name is 
//OLD   returned in old_question.
//OLD 
//OLD 
//OLD 
//OLD   Arguments:      
//OLD 
//OLD 
//OLD   Returns:
//OLD 
//OLD           0 if there was no match
//OLD   1 if there was a match of a name in the parameter file to
//OLD     one of the new_names, and in old_question the name of
//OLD     the old_value
//OLD 
//OLD   Notes:
//OLD 
//OLD   To add another variable to the list, just record the new_name and the
//OLD   old_name in the arrays below, and increase the number of names by 1
//OLD 
//OLD   Do not include the material that is in paren, that is for
//OLD 
//OLD   xxx(many_choices) -->  xxxx
//OLD 
//OLD   The routine is completely hardwired as wrtten though clearly this 
//OLD   could be changed.
//OLD           
//OLD 
//OLD 
//OLD   History:
//OLD   16sept  ksl     Coded as part of an effort to make python more 
//OLD                   robust to changes in parameter file names
//OLD 
//OLD  ************************************************************************/

#include <stdio.h>
#include <strings.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"


#define	LINELEN 256


char *old_names[] = { "mstar", "rstar", "Disk.illumination.treatment", "disk.type",
  "Disk_radiation", "Rad_type_for_disk", "disk.mdot", "T_profile_file",
  "disk.radmax",
  "stellar_wind_mdot", "stellar.wind.radmin", "stellar.wind_vbase",
  "stellar.wind.v_infinity", "stellar.wind.acceleration_exponent",
  "spectrum_wavemin", "spectrum_wavemax", "no_observers", "angle",
  "phase", "live.or.die", "spec.type", "mstar", "rstar", "Star_radiation",
  "tstar", "Rad_type_for_star", "Rad_type_for_star",
  "Rad_type_for_disk", "Rad_type_for_bl", "Boundary_layer_radiation",
  "Rad_type_for_bl", "t_bl", "lum_bl", "homologous_boundary_mdot", "msec",
  "period", "shell_wind_mdot", "Photon.sampling.approach", "Num.of.frequency.bands",
  "Lowest_energy_to_be_considered", "Highest_energy_to_be_considered", "Band.boundary",
  "Band.minimum_fraction",
  "agn_bremsstrahlung_temp", "agn_bremsstrahlung_alpha", "agn_blackbody_temp",
  "agn_power_law_cutoff", "geometry_for_pl_source", "lamp_post.height",
  "@Select_specific_no_of_scatters_in_spectra", "@Select_scatters", "@Select_photons_by_position",
  "@Select_location", "@rho", "@z", "@azimuth", "@r",
  "@save_cell_statistics", "@ispymode", "@keep_ioncycle_windsaves", "@make_ioncycle_tables",
  "@save_extract_photons", "@print_dvds_info", "@track_resonant_scatters",
  "@Use.standard.care.factors", "@Fractional.distance.photon.may.travel",
  "@Lowest.ion.density.contributing.to.photoabsorption", "@Keep.photoabs.during.final.spectrum",
  "@adjust_grid",
  "filling_factor", "Coord.system", "@write_atomicdata", "Fixed.concentrations.filename",
  "@Extra.diagnostics", "File.with.model2read", "Number.of.wind.components", "Old_windfile",
  "Model_file", "agn_power_law_index",
  NULL
};

char *new_names[] = { "Central.object.mass", "Central.object.radius",
  "Surface.reflection.or.absorption", "Disk.type", "Disk.radiation",
  "Disk.rad_type_to_make_wind",
  "Disk.mdot", "Disk.T_profile_file", "Disk.radmax",
  "Stellar_wind.mdot", "Stellar_wind.radmin", "Stellar_wind.vbase",
  "Stellar_wind.v_infinity", "Stellar_wind.acceleration_exponent",
  "Spectrum.wavemin", "Spectrum.wavemax", "Spectrum.no_observers",
  "Spectrum.angle", "Spectrum.orbit_phase", "Spectrum.live_or_die",
  "Spectrum.type", "Central_object.mass", "Central_object.radius",
  "Central_object.radiation", "Central_object.temp", "Central_object.rad_type_to_make_wind",
  "Central_object.rad_type_in_final_spectrum",
  "Disk.rad_type_in_final_spectrum", "Boundary_layer.rad_type_in_final_spectrum",
  "Boundary_layer.radiation", "Boundary_layer.rad_type_to_make_wind", "Boundary_layer.temp",
  "Boundary_layer.luminosity", "homologous.boundary_mdot", "Binary.mass_sec",
  "Binary.period", "shell.wind_mdot", "Photon_sampling.approach", "Photon_sampling.nbands",
  "Photon_sampling.low_energy_limit", "Photon_sampling.high_energy_limit", "Photon_sampling.band_boundary",
  "Photon_sampling.band_min_frac",
  "AGN.bremsstrahlung_temp", "AGN.bremsstrahlung_alpha", "AGN.blackbody_temp",
  "AGN.power_law_cutoff", "AGN.geometry_for_pl_source", "AGN.lamp_post_height",
  "@Spectrum.select_specific_no_of_scatters_in_spectra", "@Spectrum.select_scatters", "@Spectrum.select_photons_by_position",
  "@Spectrum.select_location", "@Spectrum.select_rho", "@Spectrum.select_z", "@Spectrum.select_azimuth", "@Spectrum.select_r",
  "@Diag.save_cell_statistics", "@Diag.ispymode", "@Diag.keep_ioncycle_windsaves", "@Diag.make_ioncycle_tables",
  "@Diag.save_extract_photons", "@Diag.print_dvds_info", "@Diag.track_resonant_scatters",
  "@Diag.use_standard_care_factors", "@Diag.fractional_distance_photon_may_travel",
  "@Diag.lowest_ion_density_for_photoabs", "@Diag.keep_photoabs_in_final_spectra",
  "@Diag.adjust_grid",
  "Wind.filling_factor", "Wind.coord_system", "@Diag.write_atomicdata", "Wind.fixed_concntrations_file",
  "@Diag.extra", "Wind.model2import", "Wind.number_of_components", "Wind.old_windfile",
  "Input_spectra.model_file", "AGN.power_law_index",
  NULL
};



int number_of_names = 79;

#define MIN(a,b) ((a)<b ? a:b)


/**********************************************************/
/** 
 * @brief      This is a routine which is intended to help with program updates that
 *   change the name of a keyword in a parameter file.  It is called from
 *   rdpar.string_process_from_file
 *
 * @param [in] char  new_question[]   The currnt keyword
 * @param [out] char  old_question[]   The keyword in an earlier version of Python
 * @return     0 if there was no match;
 * 	1 if there was a match of a name in the parameter file to
 * 	  one of the new_names, and in old_question the name of
 * 	  the old_value
 *
 * The routine simply matches the string in new_question to a string in
 *   the array new_names, below.  If it finds a match, the old_name is 
 *   returned in old_question.
 *
 * ###Notes###
 *
 * To add another variable to the list, just record the new_name and the
 *   old_name in the arrays below, and increase the number of names by 1
 * 
 *   Do not include the material that is in paren, that is for
 * 
 *   xxx(many_choices) -->  xxxx
 * 
 *   The routine is completely hardwired as wrtten though clearly this 
 *   could be changed, so that information was read from a file.
 *
 * Note that there can be multiple old names that are the same due
 * to some old input formats that were not syntaciticaly perfect
 * See #319

 *
 **********************************************************/

int
check_synonyms (new_question, old_question)
     char new_question[], old_question[];
{
  int i, n;
  char *line;
  char firstword[LINELEN];
  int nwords, wordlength;
  char *ccc, *index ();

  /* First check that the synonyms list has the smae number of entries in both lists and that 
   * this agrees with the number that it is supposed to have
   */

  int n_old_names = -1;
  while (old_names[++n_old_names] != NULL)
  {                             /* do nothing */
  }

  int n_new_names = -1;
  while (new_names[++n_new_names] != NULL)
  {                             /* do nothing */
  }

  if (n_new_names != n_old_names || number_of_names != n_old_names)
  {
    Error ("check_synonums: %d %d %d\n", number_of_names, n_old_names, n_new_names);
    n = MIN (n_new_names, n_old_names);
    for (i = 0; i < n; i++)
    {
      Log ("%3d %40s %40s\n", i, old_names[i], new_names[i]);
    }
    exit (0);
  }




// Strip off any extra bits in the new question
  line = new_question;
  strcpy (firstword, "");
  nwords = sscanf (line, "%s ", firstword);
  wordlength = strlen (firstword);


  if (nwords == 0)
  {
    return (0);
  }


/* Strip off everthing prior to open paren, leaving the bare parameter name that was passed as the new
 * question
 */

  if ((ccc = index (firstword, '(')) != NULL)
  {
    wordlength = (int) (ccc - firstword);
    if (wordlength == 0)
      return (0);
  }


/* firstword is the bare parameter name for the current way the parameter is expressed elsewhere.
 * We must find this in the list of new_names
 */


  for (n = 0; n < number_of_names; n++)
  {
    if (strncmp (new_names[n], firstword, wordlength) == 0)
    {
//OLD     Log
//OLD       ("Matched keyword %s in .pf file to %s in current python version\n",
//OLD     new_question, old_names[n]);
      strcpy (old_question, old_names[n]);
      return (1);
    }

  }

  return (0);

}
