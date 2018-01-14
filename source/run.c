/***********************************************************
                                       Space Telescope Science Institute

Synopsis:

This file contains the driving routines to carry out calculaiton of
the ionization of the plasma and also to extract detailed spectra.
 
Arguments:		

Returns:
 
Description:	
		
Notes:

These routines were moved from the main routine as part oa an effort
to make main shorter and to put the main ccaluations into separate
routines.  


History:

	15sep 	ksl	Moved calculating of the ionization of the
			wind to separate routines
**************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "atomic.h"


#include "python.h"


/***********************************************************
        Space Telescope Science Institute

Synopsis: calculate_ionization execucutes the ionization cycles for a 
	python model
 
Arguments:		

Returns:
 
Description:	
		
Notes:


History:

	15sep 	ksl	Moved calculating the ionization from main 
			to a separat routine

**************************************************************/

int
calculate_ionization (restart_stat)
     int restart_stat;
{
  int n, nn;
  double zz, zzz, zze, ztot, zz_adiab;
  int nn_adiab;
  WindPtr w;
  PhotPtr p;

  char dummy[LINELENGTH];

  double freqmin, freqmax;
  long nphot_to_define;
  int iwind;
#ifdef MPI_ON
  int ioniz_spec_helpers;
#endif


  p = photmain;
  w = wmain;

  freqmin = xband.f1[0];
  freqmax = xband.f2[xband.nbands - 1];

#ifdef MPI_ON
  /* the length of the big arrays to help with the MPI reductions of the spectra
     the variables for the estimator arrays are set up in the subroutines themselves */
  ioniz_spec_helpers = 2 * MSPEC * NWAVE;       //we need space for log and lin spectra for MSPEC XNWAVE
#endif

/* XXXX - THE CALCULATION OF THE IONIZATION OF THE WIND */

  geo.ioniz_or_extract = 1;     //SS July 04 - want to compute MC estimators during ionization cycles
  //1 simply implies we are in the ionization section of the code
  //and allows routines to act accordinaly.

/* 67 -ksl- geo.wycle will start at zero unless we are completing an old run */

/* XXXX - BEGINNING OF CYCLE TO CALCULATE THE IONIZATION OF THE WIND */

  if (geo.wcycle == geo.wcycles)
    xsignal (files.root, "%-20s No ionization needed: wcycles(%d)==wcyeles(%d)\n", "COMMENT", geo.wcycle, geo.wcycles);
  else
  {
    geo.pcycle = 0;             /* Set the spectrum cycles executed to 0, because 
                                   we are going to modify the wind and hence any
                                   previously calculated spectra must be recreated
                                 */
  }

  /* SWM - Setup for path tracking */
  if (geo.reverb > REV_NONE)
  {
    reverb_init (wmain);
    delay_dump_prep (restart_stat);
  }


  while (geo.wcycle < geo.wcycles)
  {                             /* This allows you to build up photons in bunches */

    xsignal (files.root, "%-20s Starting %d of %d ionization cycle \n", "NOK", geo.wcycle, geo.wcycles);

    Log ("!!Python: Beginning cycle %d of %d for defining wind\n", geo.wcycle, geo.wcycles);
    Log_flush ();               /* Flush the log file (so that we know where are if there are problems */

    /* Initialize all of the arrays, etc, that need initialization for each cycle
     */

    spectrum_init (freqmin, freqmax, geo.nangles, geo.angle, geo.phase,
                   geo.scat_select, geo.top_bot_select, geo.select_extract, geo.rho_select, geo.z_select, geo.az_select, geo.r_select);

    wind_rad_init ();           /*Zero the parameters pertaining to the radiation field */

    if (modes.ispy)
      ispy_init ("python", geo.wcycle);

    geo.n_ioniz = 0.0;
    geo.cool_tot_ioniz = 0.0;
    ztot = 0.0;                 /* ztot is the luminosity of the disk multipled by the number of cycles, which is used by save_disk_heating */

    if (!geo.wind_radiation || (geo.wcycle == 0 && geo.run_type != SYSTEM_TYPE_PREVIOUS))
      iwind = -1;               /* Do not generate photons from wind */
    else
      iwind = 1;                /* Create wind photons and force a reinitialization of wind parms */


    /* Create the photons that need to be transported through the wind
     *
     * NPHOT is the number of photon bundles which will equal the luminosity; 
     * 0 => for ionization calculation 
     */


    nphot_to_define = (long) NPHOT;

    define_phot (p, freqmin, freqmax, nphot_to_define, 0, iwind, 1);

    /* Zero the arrays, and other variables that need to be zeroed after the photons are generated. */


    geo.lum_star_back = 0;
    geo.lum_disk_back = 0;


    for (n = 0; n < NRINGS; n++)
    {
      qdisk.heat[n] = qdisk.nphot[n] = qdisk.w[n] = qdisk.ave_freq[n] = 0;
    }



    photon_checks (p, freqmin, freqmax, "Check before transport");

    wind_ip ();


    zz = 0.0;
    for (nn = 0; nn < NPHOT; nn++)
    {
      zz += p[nn].w;
    }

    Log ("!!python: Total photon luminosity before transphot %18.12e\n", zz);
    Log_flush ();               /* NSH June 13 Added call to flush logfile */
    ztot += zz;                 /* Total luminosity in all cycles, used for calculating disk heating */

    /* kbf_need determines how many & which bf processes one needs to considere.  It was introduced
     * as a way to speed up the program.  It has to be recalculated evey time one changes
     * freqmin and freqmax
     */

    kbf_need (freqmin, freqmax);

    /* NSH 22/10/12  This next call populates the prefactor for free free heating for each cell in the plasma array */
    /* NSH 4/12/12  Changed so it is only called if we have read in gsqrd data */
    if (gaunt_n_gsqrd > 0)
      pop_kappa_ff_array ();

    /* Transport the photons through the wind */
    trans_phot (w, p, 0);

    /*Determine how much energy was absorbed in the wind */
    zze = zzz = zz_adiab = 0.0;
    nn_adiab = 0;
    for (nn = 0; nn < NPHOT; nn++)
    {
      zzz += p[nn].w;
      if (p[nn].istat == P_ESCAPE)
        zze += p[nn].w;
      if (p[nn].istat == P_ADIABATIC)
      {
        zz_adiab += p[nn].w;
        nn_adiab++;
      }
    }

    Log ("!!python: Total photon luminosity after transphot %18.12e (diff %18.12e). Radiated luminosity %18.12e\n", zzz, zzz - zz, zze);
    if (geo.rt_mode == RT_MODE_MACRO)
      Log ("Luminosity taken up by adiabatic kpkt destruction %18.12e number of packets %d\n", zz_adiab, nn_adiab);





    photon_checks (p, freqmin, freqmax, "Check after transport");

    spectrum_create (p, freqmin, freqmax, geo.nangles, geo.select_extract);



    /* At this point we should communicate all the useful infomation 
       that has been accummulated on differenet MPI tasks */

#ifdef MPI_ON

    communicate_estimators_para ();

    communicate_matom_estimators_para ();       // this will return 0 if nlevels_macro == 0
#endif

    if (modes.ispy)
      ispy_close ();


    /* Calculate and store the amount of heating of the disk due to radiation impinging on the disk */
    /* We only want one process to write to the file */

#ifdef MPI_ON
    if (rank_global == 0)
    {
#endif
      qdisk_save (files.disk, ztot);

#ifdef MPI_ON
    }
    MPI_Barrier (MPI_COMM_WORLD);
#endif

/* Completed writing file describing disk heating */

    Log ("!!python: Number of ionizing photons %g lum of ionizing photons %g\n", geo.n_ioniz, geo.cool_tot_ioniz);

/* This step should be MPI_parallelised too */

    wind_update (w);


    Log ("Completed ionization cycle %d :  The elapsed TIME was %f\n", geo.wcycle, timer ());

    /* Do an MPI reduce to get the spectra all gathered to the master thread */

#ifdef MPI_ON

    gather_spectra_para (ioniz_spec_helpers, MSPEC);

#endif



#ifdef MPI_ON
    if (rank_global == 0)
    {
#endif

/* The variables for spectrum_sumamry are the filename, the attribute for the file write, the minimum and maximum spectra to write out, 
 * the type of spectrum (RAW meaning internal luminosity units, the amount by which to renormalize (1 means use the existing
 * values, loglin (0=linear, 1=log for the wavelength scale), all photons or just wind photons
 */

      spectrum_summary (files.wspec, "w", 0, 6, SPECTYPE_RAW, 1., 0, 0);        /* .spec_tot */
      spectrum_summary (files.lwspec, "w", 0, 6, SPECTYPE_RAW, 1., 1, 0);       /* .log_spec_tot */
      spectrum_summary (files.wspec_wind, "w", 0, 6, SPECTYPE_RAW, 1., 0, 1);   /* .spec_tot_wind  */
      spectrum_summary (files.lwspec_wind, "w", 0, 6, SPECTYPE_RAW, 1., 1, 1);  /* .log_spec_tot_wind */
      phot_gen_sum (files.phot, "w");   /* Save info about the way photons are created and absorbed
                                           by the disk */
#ifdef MPI_ON
    }
    MPI_Barrier (MPI_COMM_WORLD);
#endif

    /* Save everything after each cycle and prepare for the next cycle 
       JM1304: moved geo.wcycle++ after xsignal to record cycles correctly. First cycle is cycle 0. */
    /* NSH1306 - moved geo.wcycle++ back, but moved the log and xsignal statements */


    xsignal (files.root, "%-20s Finished %d of %d ionization cycle \n", "OK", geo.wcycle, geo.wcycles);
    geo.wcycle++;               //Increment ionisation cycles


/* Save only the windsave file from thread 0, to prevent many processors from writing to the same
 * file. */

#ifdef MPI_ON
    if (rank_global == 0)
    {
#endif
      wind_save (files.windsave);
      Log_silent ("Saved wind structure in %s after cycle %d\n", files.windsave, geo.wcycle);

      /* In a diagnostic mode save the wind file for each cycle (from thread 0) */

      if (modes.keep_ioncycle_windsaves)
      {
        strcpy (dummy, "");
        sprintf (dummy, "python%02d.wind_save", geo.wcycle);
        wind_save (dummy);
        Log ("Saved wind structure in %s\n", dummy);
      }
      if (modes.make_tables)
      {
        strcpy (dummy, "");
        sprintf (dummy, "diag_%s/%s%02d", files.root, files.root, geo.wcycle);
        do_windsave2table (dummy);
      }

#ifdef MPI_ON
    }
    MPI_Barrier (MPI_COMM_WORLD);
#endif




    check_time (files.root);
    Log_flush ();               /*Flush the logfile */

  }                             // End of Cycle loop

/* XXXX - END OF CYCLE TO CALCULATE THE IONIZATION OF THE WIND */


  Log (" Completed wind creation.  The elapsed TIME was %f\n", timer ());

  /* SWM - Evaluate wind paths for last iteration */
  if (geo.reverb == REV_WIND || geo.reverb == REV_MATOM)
  {
    wind_paths_evaluate (w, rank_global);
  }

  return (0);
}

/***********************************************************
                                       Space Telescope Science Institute

Synopsis:  make_spectra generates the detailed spectra
 
Arguments:		

Returns:
 
Description:	
		
Notes:


History:

	15sep 	ksl	Moved calculating the detailed spectra to a separata 
                routine
**************************************************************/

int
make_spectra (restart_stat)
     int restart_stat;
{
  WindPtr w;
  PhotPtr p;

  double freqmin, freqmax;
  double renorm;
  long nphot_to_define;
  int iwind;
#ifdef MPI_ON
  char dummy[LINELENGTH];
  int spec_spec_helpers;
#endif

  /* Next three lines have variables that should be a structure, or possibly we
     should allocate the space for the spectra to avoid all this nonsense.  02feb ksl */


  int icheck;



/* XXXX - THE CALCULATION OF A DETAILED SPECTRUM IN A SPECIFIC REGION OF WAVELENGTH SPACE */

  p = photmain;
  w = wmain;

  freqmax = C / (geo.swavemin * 1.e-8);
  freqmin = C / (geo.swavemax * 1.e-8);

#ifdef MPI_ON
  /* the length of the big arrays to help with the MPI reductions of the spectra
     the variables for the estimator arrays are set up in the subroutines themselves */
  spec_spec_helpers = (NWAVE * (MSPEC + geo.nangles));  //We need space for NWAVE wavelengths for nspectra, which will eventually equal nangles + MSPEC
#endif

  /* Perform the initilizations required to handle macro-atoms during the detailed
     calculation of the spectrum.  

     Next lines turns off macro atom estimators and other portions of the code that are
     unnecessary during spectrum cycles.  */

  geo.ioniz_or_extract = 0;

/* 57h -- 07jul -- Next steps to speed up extraction stage */
  if (!modes.keep_photoabs)
  {
    DENSITY_PHOT_MIN = -1.0;    // Do not calculated photoabsorption in detailed spectrum 
  }

  /*Switch on k-packet/macro atom emissivities  SS June 04 */

  if (geo.rt_mode == RT_MODE_MACRO)
  {
    geo.matom_radiation = 1;
  }

  /* Finished initializations required for macro-atom approach */

  /* Calculate and store which bf processess need to be considered in each cell
   * Note that this is not macro-specific but is just to speed the program up.
   */

  kbf_need (freqmin, freqmax);

  /* XXXX - BEGIN CYCLES TO CREATE THE DETAILED SPECTRUM */

  /* the next section initializes the spectrum array in two cases, for the
   * standard one where one is calulating the spectrum for the first time
   * and in the somewhat abnormal case where additional ionization cycles
   * were calculated for the wind
   */

  if (geo.pcycle == 0)
  {
    spectrum_init (freqmin, freqmax, geo.nangles, geo.angle, geo.phase,
                   geo.scat_select, geo.top_bot_select, geo.select_extract, geo.rho_select, geo.z_select, geo.az_select, geo.r_select);

    /* 68b - zero the portion of plasma main that records the numbers of scatters by
     * each ion in a cell
     */

    zero_scatters ();

  }

  /* the next condition should really when one has nothing more to do */

  else if (geo.pcycle >= geo.pcycles)
    xsignal (files.root, "%-20s No spectrum   needed: pcycles(%d)==pcycles(%d)\n", "COMMENT", geo.pcycle, geo.pcycles);

  else
  {
    /* Then we are restarting a run with more spectral cycles, but we 
       have already completed some. The memory for the spectral arrays
       should already have been allocated, and the spectrum was initialised
       on the original run, so we just need to renormalise the saved spectrum */
    /* See issue #134 (JM) */
    if (restart_stat == 0)
      Error ("Not restarting, but geo.pcycle = %i and trying to renormalise!\n", geo.pcycle);

    spectrum_restart_renormalise (geo.nangles);
  }


  while (geo.pcycle < geo.pcycles)
  {                             /* This allows you to build up photons in bunches */

    xsignal (files.root, "%-20s Starting %d of %d spectral cycle \n", "NOK", geo.pcycle, geo.pcycles);

    if (modes.ispy)
      ispy_init ("python", geo.pcycle + 1000);


    Log ("!!Cycle %d of %d to calculate a detailed spectrum\n", geo.pcycle, geo.pcycles);
    Log_flush ();               /*NSH June 13 Added call to flush logfile */
    if (!geo.wind_radiation)
      iwind = -1;               /* Do not generate photons from wind */
    else if (geo.pcycle == 0)
      iwind = 1;                /* Create wind photons and force a reinitialization of wind parms */
    else
      iwind = 0;                /* Create wind photons but do not force reinitialization */

    /* Create the initial photon bundles which need to be trannsported through the wind 

       For the detailed spectra, NPHOT*pcycles is the number of photon bundles which will equal the luminosity, 
       1 implies that detailed spectra, as opposed to the ionization of the wind is being calculated

       JM 130306 must convert NPHOT and pcycles to double precision variable nphot_to_define

     */

    nphot_to_define = (long) NPHOT *(long) geo.pcycles;
    define_phot (p, freqmin, freqmax, nphot_to_define, 1, iwind, 0);

    for (icheck = 0; icheck < NPHOT; icheck++)
    {
      if (sane_check (p[icheck].freq))
      {
        Error ("python after define phot:sane_check unnatural frequency for photon %d\n", icheck);
      }
    }


    /* Tranport photons through the wind */

    trans_phot (w, p, geo.select_extract);



    spectrum_create (p, freqmin, freqmax, geo.nangles, geo.select_extract);

/* Write out the detailed spectrum each cycle so that one can see the statistics build up! */
    renorm = ((double) (geo.pcycles)) / (geo.pcycle + 1.0);

    /* Do an MPI reduce to get the spectra all gathered to the master thread */
#ifdef MPI_ON
    gather_spectra_para (spec_spec_helpers, nspectra);
#endif





#ifdef MPI_ON
    if (rank_global == 0)
    {
#endif

      spectrum_summary (files.spec, "w", 0, nspectra - 1, geo.select_spectype, renorm, 0, 0);
      spectrum_summary (files.lspec, "w", 0, nspectra - 1, geo.select_spectype, renorm, 1, 0);

      /* Next lines  produce spectra from photons in the wind only */
      spectrum_summary (files.spec_wind, "w", 0, nspectra - 1, geo.select_spectype, renorm, 0, 1);
      spectrum_summary (files.lspec_wind, "w", 0, nspectra - 1, geo.select_spectype, renorm, 1, 1);

#ifdef MPI_ON
    }
#endif
    Log ("Completed spectrum cycle %3d :  The elapsed TIME was %f\n", geo.pcycle, timer ());

    /* JM1304: moved geo.pcycle++ after xsignal to record cycles correctly. First cycle is cycle 0. */

    xsignal (files.root, "%-20s Finished %3d of %3d spectrum cycles \n", "OK", geo.pcycle, geo.pcycles);

    geo.pcycle++;               // Increment the spectral cycles

#ifdef MPI_ON
    if (rank_global == 0)
    {
#endif
      wind_save (files.windsave);       // This is only needed to update pcycle
      spec_save (files.specsave);
#ifdef MPI_ON
    }
#endif
    check_time (files.root);
  }


/* XXXX -- END CYCLE TO CALCULATE DETAILED SPECTRUM */
#ifdef MPI_ON
  if (rank_global == 0)
  {
#endif
    phot_gen_sum (files.phot, "a");
#ifdef MPI_ON
  }
#endif

  /* SWM0215: Dump the last photon path details to file */
  if (geo.reverb != REV_NONE)
    delay_dump_finish ();       // Each thread dumps to file
#ifdef MPI_ON
  MPI_Barrier (MPI_COMM_WORLD); // Once all done
  if (rank_global == 0 && geo.reverb != REV_NONE)
    delay_dump_combine (np_mpi_global); // Combine results if necessary
#endif


/* Finally done */

#ifdef MPI_ON
  sprintf (dummy, "End of program, Thread %d only", rank_global);       // added so we make clear these are just errors for thread ngit status    
  error_summary (dummy);        // Summarize the errors that were recorded by the program
  Log ("Run py_error.py for full error report.\n");
#else
  error_summary ("End of program");     // Summarize the errors that were recorded by the program
#endif


#ifdef MPI_ON
  MPI_Finalize ();
  Log_parallel ("Thread %d Finalized. All done\n", rank_global);
#endif


  xsignal (files.root, "%-20s %s\n", "COMPLETE", files.root);
  Log ("Completed entire program.  The elapsed TIME was %f\n", timer ());
  return EXIT_SUCCESS;
}
