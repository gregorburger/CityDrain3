// ===========================================================
//							FUNCTION FILE
// ===========================================================


//   ===========================================================
/*
	Program:	Rumba01
	Purpose:	WWTP MODEL for Triton Course
				C Version

	Components:	1   temp
				2	SI
				3	SS
				4	SNO
				5	SNH
				6	SP
				7	XH
				8	XA
				9	XS
				10	XI

	Units		time			[d]
				mass			[g]
				volume			[m3]
				length			[m]
				concentration	[g/m3]

	File I/O	sys_data.txt	Input of system data			(USER)
				wtp_para.txt	Input of parameter data			(USER WITH CAUTION)
				ode_para.txt	Input of ODE parameter data		(!!!! CAUTION!!!)
				wtplog.txt		Output of Error and Warning


	Functions	init_wtp()			Read setup files - initialize
				init_relaxation()	Steady state
				run_timestep()		Run single timestep
				exit_wtp()			Exit program - close file


	Author:		Wolfgang Rauch
	Date:		27.5.98
	Version		01

	Updated		18.8.98
				24.8.98
				20.4.99
*/
//   ===========================================================





// ===========================================================
// ===========================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "rumba01dll.h"


// ===========================================================
// Global Data
sysdata		sys;
reactor		r[NREAC+1];
asm1_para	p;
settler		s;
ODE			o;

float		stoicmat[NPROC+1][NCOMP+1];
float		ceff[NCOMP+1];
FILE		*logfile;
// ===========================================================
// ===========================================================






// ===========================================================
// HELP FUNCTION
// General ODE driver for adaptive timestep algorithm
// Uses adaptive_heun for actual solution
// Pointer to Function that computes derivative dy[]
// as a function of states y[]
//
// WR
// 27.5.98
// ===========================================================
void adaptive_step(	int		n,			// No of variables
					float	h,			// total timestep
					float	t,			// time
					float	yin[],		// Array of states (before)
					float	yout[],		// Array of states (after)
					float	acc,		// accuracy criteria
					float	hh,			// 1.guess for intermedeate timestep
					float	hhmin,		// minimum value for intermedeate timestep
					int		(*count),	// number of intermediate steps
					void(*function)(float, float [], float [])
					)
{

	int		i;
	float	time = 0.0;					// time elapsed in timestep
	float   hhnext;

	float	*y;
	float	*f;
	float	*yscal;

	y		=(float *) malloc((size_t) (n+1)*sizeof(float));
	f		=(float *) malloc((size_t) (n+1)*sizeof(float));
	yscal	=(float *) malloc((size_t) (n+1)*sizeof(float));


	(*count)= 0;
	for (i=1; i<=n; i++) y[i]= yin[i];

	for (;;)
	{
		// Start: scale y values for adaptive hh !!! Can be changed
		(*function)(t+time, y, f);
		for (i=1; i<=n; i++)
			yscal[i]= fabs(y[i]) + fabs(f[i]*hh) + TINY;
		if (time + hh > h) hh = h - time + TINY;	// overshoot?

		// one step
		adaptive_heun(n,time,y,acc,&hh,hhmin,&hhnext,yscal,function);

		time += hh;
		hh = hhnext;
		(*count ++);
		if (time>=h) break;
	}
	for (i=1; i<=n; i++) yout[i]= y[i];

	free (y);
	free (f);
	free (yscal);
}
// ===========================================================




// ===========================================================
// HELP FUNCTION
// Actual Solution procedure for general ODE driver
//
// WR
// 27.5.98
// ===========================================================
void adaptive_heun(	int		n,
					float	t,
					float	yin[],
					float	acc,
					float	*hh,
					float	hhmin,
					float	*hhnext,
					float	yscal[],
					void(*function)(float, float [], float [])
					)

{
	int		i;
	float	errmax;

	float *yt;
	float *f1;
	float *f2;
	float *yerr;

	yt		=(float *) malloc((size_t) (n+1)*sizeof(float));
	f1		=(float *) malloc((size_t) (n+1)*sizeof(float));
	f2		=(float *) malloc((size_t) (n+1)*sizeof(float));
	yerr	=(float *) malloc((size_t) (n+1)*sizeof(float));

	for (;;)
	{
		// make step : Heun's method + error O(h3)
		(*function) (t,yin,f1);
		for (i=1; i<=n; i++)
			yt[i] = yin[i] + (*hh)*f1[i];
		(*function) (t + (*hh),yt,f2);
		for (i=1; i<=n; i++)
			yt[i] = yin[i] + (*hh)/2.0*(f1[i] + f2[i]);
		for (i=1; i<=n; i++)
			yerr[i] = (*hh)/2.0*(f2[i] - f1[i]);

		// check error
		errmax= 0.0;
		for (i=1; i<=n; i++)
			if (fabs(yerr[i] / yscal[i])>errmax)
				errmax = fabs(yerr[i] / yscal[i]);
		if (errmax <= acc) break; // OK done - regular exit

		// else truncation error too large, reduce stepsize
		(*hh) /= 2.0;
		// Emergency exit - caution
		if ((*hh) < hhmin)
		{
			(*hh) *=2.0;		// reset achieved timestep
			fprintf(logfile,"\n WARNING: PROBLEMS WITH ODE SOLVER ");
			break;
		}
	}
	// if possible increase stepsize (factor 2)
	if (pow(acc/errmax,0.5) > 2.0)
		(*hhnext) = (*hh)*2.0;
	else
		(*hhnext) = (*hh);

	// done : return current value to yin[]
	for (i=1; i<=n; i++) yin[i] = yt[i];

	free(yt);
	free(f1);
	free(f2);
	free(yerr);
}
//   ===========================================================




/* =================================================================== */
// Initializing Routine
// Open Logfile: wtplog.txt
// WR
// 27.5.98
// 18.8.98	Change call to logfile
/* =================================================================== */
int init_open_logfile(const char *logfile_path)
{

	if ((logfile = fopen(logfile_path,"w")) == NULL)
	{
		printf("\n ERROR: CANNOT OPEN LOG FILE: wtplog.txt \n");
		wait;
		return -1;
	}
	fprintf(logfile, "\n RUMBA - TREATMENTPLANT LOG FILE  - (c)W.R. \n");

}
/* =================================================================== */




/* =================================================================== */
// Initializing Routine
// Reads sysdata from textfile: sys_data.txt
// Simple errorchecking
// WR
// 27.5.98
// 18.8.98	Change call to logfile
// 20.4.99  change in input text and read text procedure (fgets included)
/* =================================================================== */
int init_read_sysdata(const char *sysdata_path)
{

	char	txt[205];
	FILE    *infile;

	if ((infile = fopen(sysdata_path,"r")) == NULL)
	{
		fprintf(logfile,"\n ERROR: CANNOT OPEN INPUT FILE: sys_data.txt \n");
		fclose (logfile);
		return -1;
	}

	fscanf(infile, "%f",&sys.primclar_vol);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.reactor1_vol);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.reactor2_vol);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.clarif_area);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.clarif_height);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.influent_frac_1);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.recycle);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.int_recycle);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.sludge_age);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.reactor1_O2_set);fgets(txt,200,infile);
	fscanf(infile, "%f",&sys.reactor2_O2_set);fgets(txt,200,infile);

#ifdef internal_test
	internal_print_sysdata();
#endif

	sys.influent_frac_2 = 1.0 - sys.influent_frac_1;

	// Primitive Error checking
	if (	(sys.primclar_vol	< 1.0) ||
			(sys.reactor1_vol	< 1.0) ||
			(sys.reactor2_vol	< 1.0) ||
			(sys.clarif_area	< 1.0) ||
			(sys.clarif_height	< 0.5) ||
			(sys.influent_frac_1< 0.0) ||
			(sys.influent_frac_1> 1.0) ||
			(sys.recycle		< 0.0) ||
			(sys.int_recycle	< 0.0) ||
			(sys.sludge_age		< 1.0) ||
			(sys.reactor1_O2_set< 0.0) ||
			(sys.reactor1_O2_set>10.0) ||
			(sys.reactor2_O2_set< 0.0) ||
			(sys.reactor2_O2_set>10.0)
			)
	{
		fprintf(logfile,"\n ERROR: UNREALISTIC VALUES IN sys_data.txt \n");
		fclose (logfile);
		return -1;
	}

	fclose(infile);

}
/* ===================================================================*/



/* =================================================================== */
// Initializing Routine
// Reads parameter from textfile: wtp_para.txt
// Format :		2 Parameter in each line   X.XXXXXXX X.XXXXXXX
//				1: parameter at 20C		2:temp coeff
// WR
// 27.5.98
// 18.8.98	Change call to logfile
// 20.4.99  change in input text and read text procedure (fgets included)
/* =================================================================== */
int init_read_parameter(const char *parameter_path)
{

	char	txt[205];
	FILE    *infile;

	if ((infile = fopen(parameter_path,"r")) == NULL)
	{
		fprintf(logfile,"\n ERROR: CANNOT OPEN INPUT FILE: wtp_para.txt \n");
		fclose (logfile);
		return -1;
	}

	fscanf(infile, "%f %f",&p.mh20, &p.mh_t);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.Ks);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.Koh);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.Kno);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.bh20, &p.bh_t);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.etag);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.kh20, &p.kh_t);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.Kx20, &p.Kx_t);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.ma20, &p.ma_t);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.Knh20, &p.Knh_t);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.Koa);fgets(txt,200,infile);
	fscanf(infile, "%f %f",&p.ba20, &p.ba_t);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.kP);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.yh);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.ya);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.ix);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.ixi);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.iP);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fp);fgets(txt,200,infile);

	fscanf(infile, "%f",&p.etamax);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.etamin);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.etacoeff);fgets(txt,200,infile);

	fscanf(infile, "%f",&p.vsmax);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.vsmeff);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.rh);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.rp);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fns);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.Xthresh);fgets(txt,200,infile);

	fscanf(infile, "%f",&p.fCODsSI);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fCODsSS);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fCODsXS);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fCODpXS);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fCODpXH);fgets(txt,200,infile);
	fscanf(infile, "%f",&p.fCODpXI);fgets(txt,200,infile);

#ifdef internal_test
	internal_print_parameter();
#endif

	// Simple Errorchecking
	if	((p.fCODsSI+p.fCODsSS+p.fCODsXS != 1.0)	||
		 (p.fCODpXS+p.fCODpXH+p.fCODpXI != 1.0))
	{
		fprintf(logfile,"\n ERROR: UNREALISTIC VALUES IN wtp_para.txt \n");
		fclose (logfile);
		return -1;
	}

	fclose(infile);
}
/* ===================================================================*/




/* =================================================================== */
// Initializing Routine
// Reads ODE parameter from textfile: ode_para.txt
// Format :		X.XXXXXXX
// WR
// 27.5.98
// 18.8.98	Change call to logfile
// 20.4.99  change in input text and read text procedure (fgets included)
/* =================================================================== */
int init_read_odedata(const char *odedata_path)
{

	char	txt[205];
	FILE    *infile;

	if ((infile = fopen(odedata_path,"r")) == NULL)
	{
		fprintf(logfile,"\n ERROR: CANNOT OPEN INPUT FILE: ode_para.txt \n");
		fclose (logfile);
		return -1;
	}

	fscanf(infile, "%f",&o.ACCUR);fgets(txt,200,infile);
	fscanf(infile, "%f",&o.DT_INIT);fgets(txt,200,infile);
	fscanf(infile, "%f",&o.DT_MIN);fgets(txt,200,infile);
	fscanf(infile, "%f",&o.DT_MAX);fgets(txt,200,infile);

#ifdef internal_test
	internal_print_odedata();
#endif

	// Simple Errorchecking
	if	((o.ACCUR	> 0.01)		||
		 (o.DT_INIT > 1.0)		||
		 (o.DT_MIN	> 0.001)	||
		 (o.DT_MAX	> 0.1) )
	{
		fprintf(logfile,"\n ERROR: UNREALISTIC VALUES IN ode_para.txt \n");
		fclose (logfile);
		return -1;
	}

	fclose(infile);
}
/* ===================================================================*/




/* =================================================================== */
// Initializing Routine
// Initializes all systemdata
// Initial conditions implemented direcltly in code
// WR
// 27.5.98
/* =================================================================== */
void init_system()
{

	int		i;

	r[1].vol = sys.primclar_vol;
	r[2].vol = sys.reactor1_vol;
	r[3].vol = sys.reactor2_vol;
	r[4].vol = sys.clarif_area * sys.clarif_height;

	s.hlay	 = (float) sys.clarif_height/NLAY;
	s.vlay	 = s.hlay * sys.clarif_area;

	for (i=1;i<=NREAC;i++)
	{
		r[i].flow =    0.0;

		r[i].c[1] =   20.0;		// Temp
		r[i].c[2] =   20.0;		// SI
		r[i].c[3] =    1.0;		// SS
		r[i].c[4] =   10.0;		// SNO
		r[i].c[5] =    1.0;		// SNH
		r[i].c[6] =    1.0;		// SP
		r[i].c[7] = 1500.0;		// XH
		r[i].c[8] =  100.0;		// XA
		r[i].c[9] =  200.0;		// XS
		r[i].c[10]= 1500.0;		// XI
	}

	for (i=1;i<=NCOMP;i++)	ceff[i]  = 0.0;
	for (i=1;i<=NLAY;i++)	s.Xsl[i] = 1000.0;

}
/* ===================================================================*/


/* =================================================================== */
// Initializing Routine
// Init Stoichiometric matrix
// WR
// 27.5.98
/* =================================================================== */
void init_stoicmat()
{
	int	i,j;

	for (i=1;i<=NPROC;i++)
		for (j=1;j<=NCOMP;j++)
			stoicmat[i][j] = 0.0;


	stoicmat[1][3] = -1.0/p.yh;
	stoicmat[1][5] = -p.ix;
	stoicmat[1][6] = -p.iP;
	stoicmat[1][7] = 1.0;

	stoicmat[2][3] = -1.0/p.yh;
	stoicmat[2][4] = -(1.0-p.yh)/2.86/p.yh;
	stoicmat[2][5] = -p.ix;
	stoicmat[2][6] = -p.iP;
	stoicmat[2][7] = 1.0;

	stoicmat[3][4] = 1.0/p.ya;
	stoicmat[3][5] = -1.0/p.ya - p.ix;
	stoicmat[3][6] = - p.iP;
	stoicmat[3][8] = 1.0;

	stoicmat[4][7] = -1.0;
	stoicmat[4][9] = 1.0-p.fp;
	stoicmat[4][10]= p.fp;

	stoicmat[5][8] = -1.0;
	stoicmat[5][9] = 1.0-p.fp;
	stoicmat[5][10]= p.fp;

	stoicmat[6][3] = 1.0;
	stoicmat[6][5] = p.ix;
	stoicmat[6][6] = p.iP;
	stoicmat[6][9] = -1.0;

	stoicmat[7][6] = -1.0;

}
/* =================================================================== */







/* =================================================================== */
// Initializing Routine
// Initializes treatment plant
// Call only once in a run and use exit - File IO
// WR
// 27.5.98
/* =================================================================== */
DLLexport int init_wtp(const char *logfile_path,
					   const char *sysdata_path,
					   const char *parameter_path,
					   const char *odedata_path)
{
	if (!init_open_logfile(logfile_path)) {
		return -1;
	}
	if (!init_read_sysdata(sysdata_path)) {
		return -1;
	}
	if (!init_read_parameter(parameter_path)) {
		return -1;
	}
	if (!init_read_odedata(odedata_path)) {
		return -1;
	}
	init_system();
	init_stoicmat();

	fprintf(logfile, "\n System is initialized");
	return 0;
}
/* ===================================================================*/



/* =================================================================== */
// Exit Routine
// Close Logfile: wtplog.txt
// WR
// 27.5.98
/* =================================================================== */
DLLexport void exit_wtp()
{
	fprintf(logfile, "\n EXIT RUMBA: SUCCESS \n");
	fclose (logfile);
}
/* =================================================================== */




/* =================================================================== */
// Characterization of influent
// Calculate cin[] to 1st reactor

// Characterisation cccording to Article
// Bornemann et al., Korrespondenz Abwasser 1998(45) No 3, p455-462
// Assumptions Nitrogen:		Norg	in SI,XI		= ixi  (ca 2%)
//								Norg	in XH,XA,XS		= ix   (ca 7%)
//								Norg	in SS			= SNH fraction

//								SNH = Ntot - Norg(SI,XS,XI,XH,XA)
//
// Gives one Warning to logfile if N/P characterization is incorrect
// WR
// 27.5.98
/* =================================================================== */
void influent_character(float CODs,float CODp, float Ntot, float Ptot)
{

	int			i;
	float		Norg;
	float		Porg;
	static int	Nwarn=1;
	static int	Pwarn=1;


	// COD
	r[1].cin[2] = CODs * p.fCODsSI;						//SI
	r[1].cin[3] = CODs * p.fCODsSS;						//SS

	r[1].cin[7] = CODp * p.fCODpXH;						//XH
	r[1].cin[8] = 0.0;									//XA
	r[1].cin[9] = CODs * p.fCODsXS + CODp * p.fCODpXS;	//XS
	r[1].cin[10]= CODp * p.fCODpXI;						//XI


	// NITROGEN
	r[1].cin[4] = 0.0;									//SNO
	Norg =	(r[1].cin[2]+r[1].cin[10]) * p.ixi +
			(r[1].cin[7]+r[1].cin[9]) * p.ix;

	// if Norg>Ntot = ERROR but here only SNH = 0.1 NO OTHER CORECTION
	if ((Ntot-Norg)>0.0)
		r[1].cin[5] = Ntot - Norg;						// SNH
	else
	{
		r[1].cin[5] = 0.1;

		if (Nwarn==1)
		{
			fprintf(logfile,"\n WARNING: NITROGEN CHARACTERIZATION IS INNCORECT - CHECK DATA ");
			Nwarn=0;
		}
	}


	// PHOSPHOR
	Porg = 0.0;
	for (i=NSOL+1;i<=NPAR;i++)	Porg += r[1].cin[i] * p.iP;
	// if Porg>Ptot = ERROR but here only SP = 0.1 NO OTHER CORECTION
	if ((Ptot-Porg)>0.0)
		r[1].cin[6] = Ptot - Porg;						// SP
	else
	{
		r[1].cin[6] = 0.1;

		if (Pwarn==1)
		{
			fprintf(logfile,"\n WARNING: PHOSPHOR CHARACTERIZATION IS INNCORECT - CHECK DATA ");
			Pwarn=0;
		}
	}


}
/* =================================================================== */





/* =================================================================== */
// Characterization of effluent
// Calculate from c[] from Clarifier effluent
// Background and Assumptions see: influent_character
// WR
// 27.5.98
/* =================================================================== */
void effluent_character(float *CODs_eff,float *CODp_eff, float *Ntot_eff, float *Ptot_eff)
{

	*CODs_eff = ceff[2] + ceff[3] + ceff[9] * p.fCODsXS / (p.fCODsXS+p.fCODpXS);
	*CODp_eff = ceff[7] + ceff[8] + ceff[10] + ceff[9] * p.fCODpXS / (p.fCODsXS+p.fCODpXS);

	*Ntot_eff =	(ceff[2]+ceff[10]) * p.ixi +
				(ceff[7]+ceff[8]+ceff[9]) * p.ix +
				 ceff[4] + ceff[5];

	*Ptot_eff = (ceff[7]+ceff[8]+ceff[9]+ceff[10]) * p.iP +
				 ceff[6];

}
/* =================================================================== */




/* =================================================================== */
// Compute temperature dependent parameter values
// Input = actual temp in the reactor
// Input could be also the global variable (Comp temp no longer required)
// WR
// 27.5.98
/* =================================================================== */
void temp_para(float temperatur)
{
	// Temperatur dependent coeff from actual temp in reactor: temperatur
	p.mh = p.mh20*exp(p.mh_t*(temperatur-20.0));
	p.bh = p.bh20*exp(p.bh_t*(temperatur-20.0));
	p.kh = p.kh20*exp(p.kh_t*(temperatur-20.0));
	p.Kx = p.Kx20*exp(p.Kx_t*(temperatur-20.0));
	p.ma = p.ma20*exp(p.ma_t*(temperatur-20.0));
	p.Knh= p.Knh20*exp(p.Knh_t*(temperatur-20.0));
	p.ba = p.ba20*exp(p.ba_t*(temperatur-20.0));

}
/* =================================================================== */





/* ===================================================================*/
// Actual calculation of one timestep
// dt can be any value - internally adapted
// Flow and Plant outline is fixed coded
// WR
// 27.5.98
/* ===================================================================*/
DLLexport void run_timestep(
					float	dt,
					float	temp,
					float	flow,
					float	CODs,
					float	CODp,
					float	Ntot,
					float	Ptot,
					float	*CODs_eff,
					float	*CODp_eff,
					float	*Ntot_eff,
					float	*Ptot_eff,
					float	*sblh_eff
					)

{

	int			i,j;
	int			count=1;
	float		dtODE;
	static int	warning=1;


	// Precaution to ensure internal timestep for ODE calculation is small enough
	// dt can be any time (at least numerically - but uncertainty introduced)
	if (dt>o.DT_MAX)
		dtODE = o.DT_MAX;
	else
		dtODE = dt;


	/* --------------------------------------- */
	influent_character(CODs, CODp, Ntot, Ptot);
	r[1].cin[1] = temp;
	/* --------------------------------------- */



	/* --------------------------------------- */
	// Primary Clarifier
	r[1].flow = flow;
	adaptive_step  (NCOMP,dt,0.0,r[1].c, r[1].c,
					o.ACCUR,dtODE*o.DT_INIT,dtODE*o.DT_MIN,&count,run_primclar);
	/* --------------------------------------- */



	/* --------------------------------------- */
	// Reactor 1
	r[2].flow = flow *(sys.influent_frac_1 + sys.recycle + sys.int_recycle);
	temp_para(r[2].c[1]);

	for(i=1;i<=NCOMP;i++)
	{
		r[2].cin[i] = (	flow * sys.influent_frac_1 * r[1].c[i] +
						flow * sys.recycle * r[4].c[i] +
						flow * sys.int_recycle * r[3].c[i])
						/ r[2].flow;
	}

	adaptive_step (	NCOMP,dt,0.0,r[2].c, r[2].c,
					o.ACCUR,dtODE*o.DT_INIT,dtODE*o.DT_MIN,&count,run_reactor1);
	/* --------------------------------------- */



	/* --------------------------------------- */
	// Reactor 2
	r[3].flow = flow *(1.0 + sys.recycle + sys.int_recycle);
	temp_para(r[3].c[1]);

	for(i=1;i<=NCOMP;i++)
	{
		r[3].cin[i] = (	r[2].flow * r[2].c[i] +
						flow * sys.influent_frac_2 * r[1].c[i])
						/ r[3].flow;
	}

	adaptive_step (	NCOMP,dt,0.0,r[3].c, r[3].c,
					o.ACCUR,dtODE*o.DT_INIT,dtODE*o.DT_MIN,&count,run_reactor2);
	/* --------------------------------------- */



	/* --------------------------------------- */
	// Clarifier
	r[4].flow = flow *(1.0 + sys.recycle);

	for(i=1;i<=NCOMP;i++)
	{
		r[4].cin[i] = r[3].c[i];
	}


	if (FLUX_S)
		run_clarifier(dt,dtODE,sblh_eff);
	else
	{
		point_settler();
		*sblh_eff = 0.0;
	}
	/* --------------------------------------- */



	/* --------------------------------------- */
	waste_sludge_removal(flow);
	/* --------------------------------------- */



	/* --------------------------------------- */
	effluent_character(CODs_eff, CODp_eff, Ntot_eff, Ptot_eff);
	/* --------------------------------------- */


	/* --------------------------------------- */
	// check if components positive - otherwise warning
	for (j=1;j<=NREAC;j++)
	{
		for (i=1;i<=NCOMP;i++)
		{
			if (r[j].c[i] < 0.0)
			{
				r[j].c[i] = 0.01;
				if (warning==1)
				{
					fprintf(logfile,"\n WARNING: ODE PROBLEM - NEGATIVE CONCENTRATIONS ");
					warning=0;
				}
			}
		}
	}
	/* --------------------------------------- */

}
/* ===================================================================*/




/* ===================================================================*/
// Primary clarifier model
// See Rauch 1996
// WR
// 27.5.98
/* ===================================================================*/
void run_primclar(float t,float y[], float dy[])
{
	int		i;
	float	eta;


	eta = p.etamax - (p.etamax-p.etamin)*exp(-p.etacoeff*r[1].vol/r[1].flow);

	// All components as solubles (Mixing)
	for (i=1;i<=NCOMP;i++)
	{
		dy[i]= r[1].flow / r[1].vol * (r[1].cin[i] - y[i]);
	}


	// All particulates are removed with the same relation
	for (i=NSOL+1;i<=NPAR;i++)
	{
		dy[i]= r[1].flow / r[1].vol * (r[1].cin[i]*(1.0-eta) - y[i]);
	}

}
/* ===================================================================*/



/* ===================================================================*/
// Call from ODE - calls run_reactor with specific data
// WR
// 27.5.98
/* ===================================================================*/
void run_reactor1(float t, float y[], float dy[])
{

	run_reactor(t, r[2].flow, r[2].vol, sys.reactor1_O2_set, y, dy, r[2].cin);

}
/* ===================================================================*/



/* ===================================================================*/
// Call from ODE - calls run_reactor with specific data
// WR
// 27.5.98
/* ===================================================================*/
void run_reactor2(float t, float y[], float dy[])
{

	run_reactor(t, r[3].flow, r[3].vol, sys.reactor2_O2_set, y, dy, r[3].cin);

}
/* ===================================================================*/




/* ===================================================================*/
// Actual calculation of reactor: Hydraulics + biokinetic model
// Adapted ASM1 Version with Phosphor
// WR
// 27.5.98
/* ===================================================================*/
void run_reactor(float t, float flow, float vol, float So, float y[], float dy[],float yin[])
{
	int		i,j;
	float	rate[NPROC+1];


	// CODE OPTIMIZATION ALLOWS ONLY LIMITED NUMBER OF EXPRESSIONS IN 1 LINE
	rate[1] = p.mh*(y[3]/(p.Ks+y[3]))*(So/(p.Koh+So));
	rate[1] *=(y[5]/(0.01+y[5]))*(y[6]/(0.01+y[6]))*y[7];
	rate[2] = p.mh*	(y[3]/(p.Ks+y[3]))*(p.Koh/(p.Koh+So))*(y[4]/(p.Kno+y[4]));
	rate[2] *=(y[5]/(0.01+y[5]))*(y[6]/(0.01+y[6])) * p.etag*y[7];
	rate[3] = p.ma*(y[5]/(p.Knh+y[5]))*(So/(p.Koa+So))*(y[6]/(0.01+y[6])) * y[8];
	rate[4] = p.bh*y[7];
	rate[5] = p.ba*y[8];
	rate[6] = p.kh*(y[9]/y[7]/(p.Kx+y[9]/y[7]))* y[7];
	rate[7] = p.kP*y[6];


	// Hydraulics (Mixing for all components)
	for (i=1;i<=NCOMP;i++)
	{
		dy[i] = flow/vol*(yin[i]-y[i]);
	}

	// Actual Processes only [3 .. NCOMP]
	for (j=1;j<=NPROC;j++)
	{
		for (i=3;i<=NCOMP;i++)
		{
			dy[i] += rate[j] * stoicmat[j][i];
		}
	}

}
/* ===================================================================*/




/* ===================================================================*/
// Simple ideal point settler
// WR
// 27.5.98
/* ===================================================================*/
void point_settler()
{
	int i;

	// Solubles
	for(i=1;i<=NCOMP;i++)
	{
		r[4].c[i] = r[4].cin[i];
		ceff[i]   = r[4].cin[i];
	}

	// Particulates
	for(i=NSOL+1;i<=NPAR;i++)
	{
		r[4].c[i] = r[4].cin[i]*(1.0 + sys.recycle);
		ceff[i]   = 0.0;
	}
}
/* ===================================================================*/





/* ===================================================================*/
// WR
// 27.5.98
/* ===================================================================*/
void run_clarifier_CSTR(float t,float y[], float dy[])
{
	int		i;

	// All components as solubles (Mixing)
	for (i=1;i<=NCOMP;i++)
	{
		dy[i]= r[4].flow / r[4].vol * (r[4].cin[i] - y[i]);
	}

}
/* ===================================================================*/




/* ===================================================================*/
// Flux Settler model
// See Tacacs et al. Wat.Res 1991
// WR
// 27.5.98
/* ===================================================================*/
void run_clarifier_flux(float t,float y[], float dy[])
{
	int		i;
	float	Xslred;				// red TSS conc = Xsl - Xsl*fns

	float	vsl[NLAY+1];		// vs in each layer
	float	Fsl[NLAY+1];		// Flux in each layer

	float	Qin = r[4].flow;
	float	Qr	= r[1].flow * sys.recycle;
	float	Qe  = r[1].flow;


	for(i=1;i<=NLAY;i++)
	{
		// settling velocity vsl[] according to Tacacs
		Xslred = y[i] - s.Xml*p.fns;

		vsl[i] = p.vsmax * (exp(-p.rh * Xslred) - exp(-p.rp * Xslred));
		if (vsl[i]<0)			vsl[i] =0.0;
		if (vsl[i]>p.vsmeff)	vsl[i] = p.vsmeff;

		Fsl[i] = vsl[i] * y[i];
		if (i!=1)
		{
			if (((vsl[i-1] * y[i-1]) < Fsl[i]) || (y[i] < p.Xthresh))
			{
				Fsl[i] = vsl[i-1] * y[i-1];
			}
		}
	}


	dy[1]		=	Qe/s.vlay*(y[2]-y[1])-Fsl[2]/s.hlay;
	dy[NLAY]	=	Qr/s.vlay*(y[NLAY-1]-y[NLAY])+Fsl[NLAY]/s.hlay;
	dy[NFEED]	=	Qin/s.vlay*(s.Xml-y[NFEED])+(Fsl[NFEED]-Fsl[NFEED+1])/s.hlay;

	for (i=2;i<=NFEED-1;i++)
		dy[i]	=	Qe/s.vlay*(y[i+1]-y[i])+(Fsl[i]-Fsl[i+1])/s.hlay;

	for (i=NFEED+1;i<=NLAY-1;i++)
		dy[i]	=	Qr/s.vlay*(y[i-1]-y[i])+(Fsl[i]-Fsl[i+1])/s.hlay;


}
/* ===================================================================*/




/* ===================================================================*/
// Actual Flux settler function for 1 timestep
// Solubles are treated as in a CSTR
// Particulates are lumped together when their settling behavior is computed
// CXharacterization of Particulates according to influent character
// i.e. the staorage function with respect to character of part. Comp.is neglected
//
// Sludge blanket: 1 layer where	dX > 50% and X > Xthresh/3		or
//									X  > Xthresh
//
// WR
// 27.5.98
/* ===================================================================*/
void run_clarifier(float dt, float dtODE, float *sludge_blanket)
{
	int		i;
	int		count = 1;
	float	Xml_cc = 0.0;
	float	cc[NCOMP+1];


	// Calculate Sum of particulate Comp.
	s.Xml=0;
	for(i=NSOL+1;i<=NPAR;i++)
	{
		s.Xml += r[4].cin[i];
	}


	// 1.step: Compute settler as CSTR for Sol Component conc - store result in cc
	adaptive_step (	NCOMP,dt,0.0,r[4].c, cc,
					o.ACCUR,dtODE*o.DT_INIT,dtODE*o.DT_MIN,&count,run_clarifier_CSTR);


	// 2.step: Compute flux
	adaptive_step (	NLAY,dt,0.0,s.Xsl,s.Xsl,
					o.ACCUR,dtODE*o.DT_INIT,dtODE*o.DT_MIN,&count,run_clarifier_flux);


	// characterize effluent and recycle soluble Components
	for(i=1;i<=NSOL;i++)
	{
		r[4].c[i] = cc[i];
		ceff[i]   = cc[i];
	}


	// characterize effluent and recycle particulate Components
	// Characterization according to influent character
	for(i=NSOL+1;i<=NPAR;i++)	Xml_cc += r[4].cin[i];

	for(i=NSOL+1;i<=NPAR;i++)
	{
		ceff[i]   = s.Xsl[1] * r[4].cin[i]/Xml_cc;
		r[4].c[i] = s.Xsl[NLAY] * r[4].cin[i]/Xml_cc;
	}


	// calculate sludge blanket in [0..1]
	for(i=2;i<=NLAY;i++)
	{
		if (((s.Xsl[i]-s.Xsl[i-1])/s.Xsl[i-1] > 0.5 ) && (s.Xsl[i] > p.Xthresh/3.0)) break;
		if (s.Xsl[i] > p.Xthresh) break;
	}
	*sludge_blanket = (float) (NLAY+1-i)/NLAY;
}
/* ===================================================================*/




/* ===================================================================*/
// Removal of waste sludge as a function of sludge age
// Loss of X in effluent and mass of X in settler is included
// WR
// 27.5.98
/* ===================================================================*/
void waste_sludge_removal(float flow)
{

	int		i,j,k;
	float	Xeff = 0.0;
	float	Xtot = 0.0;
	float	sludge_tot;			// M
	float	sludge_removed;		// MT-1


	if (FLUX_S)
	{
		for (k=NSOL+1;k<=NPAR;k++)	Xeff += r[4].c[k];
		for (k=1;k<=NLAY;k++)		Xtot +=	s.vlay * s.Xsl[k];
	}


	for (i=NSOL+1;i<=NPAR;i++)
	{

		sludge_tot = 0.0;
		for (j=2;j<=3;j++)
		{
			sludge_tot += r[j].vol * r[j].c[i];
		}

		if (FLUX_S)
			sludge_tot += Xtot * r[4].c[i] / Xeff;

		sludge_removed = sludge_tot/sys.sludge_age - flow * ceff[i];
		if (sludge_removed<0.0)  sludge_removed = 0.0;

		// Adjustment of concentrations in recycle
		// Assumption wasteflow taken from there
		r[4].c[i] -= sludge_removed/flow/sys.recycle;

	}

}
/* ===================================================================*/




/* ===================================================================*/
// Relaxion for steady state conditions
// WR
// 27.5.98
// 24.8.98	Change in call of procedure
//			Write Result in logfile
/* ===================================================================*/
DLLexport void init_relaxation(
						float	dt,
						float	temp,
						float	flow,
						float	CODs,
						float	CODp,
						float	Ntot,
						float	Ptot,
						float	*CODs_eff,
						float	*CODp_eff,
						float	*Ntot_eff,
						float	*Ptot_eff,
						float	*sblh_eff,
						int		n_dt
					)

{
	int i;

	for (i=1; i<=n_dt;i++)
	{
		run_timestep	(dt,temp,flow,CODs,CODp,Ntot,Ptot,
						 CODs_eff,CODp_eff,Ntot_eff,Ptot_eff,sblh_eff);
	}

	fprintf(logfile,"\n\n     Steady state conditions");
	fprintf(logfile,"\n     Result INFLUENT   EFFLUENT");
	fprintf(logfile,"\n     CODs   %6.2f      %6.2f",CODs,*CODs_eff);
	fprintf(logfile,"\n     CODp   %6.2f      %6.2f",CODp,*CODp_eff);
	fprintf(logfile,"\n     Ntot   %6.2f      %6.2f",Ntot,*Ntot_eff);
	fprintf(logfile,"\n     Ptot   %6.2f      %6.2f",Ptot,*Ptot_eff);
	fprintf(logfile,"\n     SBLH              %6.2f",*sblh_eff);
	fprintf(logfile,"\n");
}
/* ===================================================================*/




/* ===================================================================*/
// Internal Routine
// Prints sysdata on screen
// WR
// 27.5.98
/* ===================================================================*/
void internal_print_sysdata()
{

	printf("\n\t        primclar_vol      =  %f",sys.primclar_vol);
	printf("\n\t        reactor1_vol      =  %f",sys.reactor1_vol);
	printf("\n\t        reactor2_vol      =  %f",sys.reactor2_vol);
	printf("\n\t        clarif_area       =  %f",sys.clarif_area);
	printf("\n\t        clarif_height     =  %f",sys.clarif_height);
	printf("\n\t        influent_frac_1   =  %f",sys.influent_frac_1);
	printf("\n\t        influent_frac_2   =  %f",sys.influent_frac_2);
	printf("\n\t        recycle           =  %f",sys.recycle);
	printf("\n\t        int_recycle       =  %f",sys.int_recycle);
	printf("\n\t        sludge_age        =  %f",sys.sludge_age);
	printf("\n\t        reactor1_O2_set   =  %f",sys.reactor1_O2_set);
	printf("\n\t        reactor2_O2_set   =  %f",sys.reactor2_O2_set);

	wait;
}
/* ===================================================================*/



/* ===================================================================*/
// Internal Routine
// Prints ODE data on screen
// WR
// 27.5.98
/* ===================================================================*/
void internal_print_odedata()
{

	printf("\n\t        ACCUR             =  %f",o.ACCUR);
	printf("\n\t        DT_INIT           =  %f",o.DT_INIT);
	printf("\n\t        DT_MIN            =  %f",o.DT_MIN);
	printf("\n\t        DT_MAX            =  %f",o.DT_MAX);

	wait;
}
/* ===================================================================*/




/* ===================================================================*/
// Internal Routine
// Prints parameter on screen
// WR
// 27.5.98
/* ===================================================================*/
void internal_print_parameter ()
{

	printf("\n\t		mh                     =  %f",p.mh);
	printf("\n\t		ma                     =  %f",p.ma);
	printf("\n\t		yh                     =  %f",p.yh);
	printf("\n\t		ya                     =  %f",p.ya);
	printf("\n\t		ix                     =  %f",p.ix);
	printf("\n\t		ixi                    =  %f",p.ixi);
	printf("\n\t		iP                     =  %f",p.iP);
	printf("\n\t		fp                     =  %f",p.fp);
	printf("\n\t		bh                     =  %f",p.bh);
	printf("\n\t		ba                     =  %f",p.ba);
	printf("\n\t		Ks                     =  %f",p.Ks);
	printf("\n\t		Koh                    =  %f",p.Koh);
	printf("\n\t		Kno                    =  %f",p.Kno);
	printf("\n\t		etag                   =  %f",p.etag);
	printf("\n\t		kh                     =  %f",p.kh);
	printf("\n\t		kP                     =  %f",p.kP);
	printf("\n\t		Kx                     =  %f",p.Kx);
	printf("\n\t		Knh                    =  %f",p.Knh);
	printf("\n\t		Koa                    =  %f",p.Koa);

	wait;

	printf("\n\t		etamax                 =  %f",p.etamax);
	printf("\n\t		etamin                 =  %f",p.etamin);
	printf("\n\t		etacoeff               =  %f",p.etacoeff);

	printf("\n\t		vsmax                  =  %f",p.vsmax);
	printf("\n\t		vsmeff                 =  %f",p.vsmeff);
	printf("\n\t		rh                     =  %f",p.rh);
	printf("\n\t		rp                     =  %f",p.rp);
	printf("\n\t		fns                    =  %f",p.fns);
	printf("\n\t		Xthresh                =  %f",p.Xthresh);

	printf("\n\t		fCODsSI                =  %f",p.fCODsSI);
	printf("\n\t		fCODsSS                =  %f",p.fCODsSS);
	printf("\n\t		fCODsXS                =  %f",p.fCODsXS);
	printf("\n\t		fCODpXS                =  %f",p.fCODpXS);
	printf("\n\t		fCODpXH                =  %f",p.fCODpXH);
	printf("\n\t		fCODpXI                =  %f",p.fCODpXI);

	wait;

}
/* =================================================================== */




/* ===================================================================*/
// Internal Routine
// Prints Result (Reactor data) to logfile
// WR
// 27.5.98
// 20.4.99	Change: print now to logfile
/* ===================================================================*/
void internal_print_result()
{

	int		i,j;


	fprintf(logfile,"\n\n\t\t\t\t WTP SIMULATION RESULT \n\n");


	fprintf(logfile,"\nReactor data: inflow and concentration\n");
	fprintf(logfile,"\n\nInfluent     C[]:");
	for(j=1;j<=NCOMP;j++)
	{
			fprintf(logfile,"%4.0f ",r[1].cin[j]);
	}

	for(i=1;i<=NREAC;i++)
	{
		fprintf(logfile,"\n%2d Q:%5.1f  C[]:",i, r[i].flow);
		for(j=1;j<=NCOMP;j++)
		{
			fprintf(logfile,"%4.0f ",r[i].c[j]);
		}
	}

	fprintf(logfile,"\nEffluent     C[]:");
	for(j=1;j<=NCOMP;j++)
	{
			fprintf(logfile,"%4.0f ",ceff[j]);
	}


	fprintf(logfile,"\n\nSettler Concentration of part. COD");
	for(j=1;j<=NLAY;j++)
	{
			fprintf(logfile,"\n %2d      %8.1f ",j,s.Xsl[j]);
	}
	fprintf(logfile,"\n\n");

}
/* =================================================================== */

