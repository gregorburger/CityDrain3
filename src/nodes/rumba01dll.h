// ===========================================================
//							HEADER FILE
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

	Updated		24.8.98			Changed init_relazation routine
				20.4.99			Compiler directive DLL (for switching subroutine-dll)


*/
//   ===========================================================




// ===========================================================
// Definitions
// Compiler directives:
//		<DLL>		allowes to switch between subrourine and dLL
//		<internal_test> enables to read the input data
// ===========================================================
//#define internal_test
//#define DLL

#ifdef __cplusplus
extern "C" {
#endif


#ifdef DLL
	#define DLLexport _declspec(dllexport)
#else
	#define DLLexport
#endif

// ===========================================================
#define wait (printf("\n\t\t\t * Press ENTER *"),rewind(stdin),fgetc(stdin))
// ===========================================================



// ===========================================================

// ODE SOLVER
#define TINY	1.0e-20

// WWTP
#define NCOMP	10				// No of Components
#define NSOL	6				// No of soluble Components
#define NPAR	NSOL+4			// No of particulate Components (ranked after sol)
#define NREAC	4				// No of Reactors - !! direct coded in run_timestep
#define NPROC	7				// No of Processes
#define NLAY	10				// No of Layers for Settler Model
#define NFEED	4

// Settler Option
#define FLUX_S	1				// Flux		0 = Point settler	1 = 1D Flux settler
// ===========================================================



// ===========================================================
//   FUNCTION PROTOTYPES FOR HELP ROUTINES
void adaptive_step(	int		n,			// No of variables
					float	h,			// total timestep
					float	t,			// time
					float	yin[],		// Array of states (before)
					float	yout[],		// Array of states (after)
					float	acc,		// accuracy criteria
					float	hh1,		// 1.guess for intermedeate timestep
					float	hhmin,		// minimum value for intermedeate timestep
					int		(*count),		// number of intermediate steps
					void(*function)(float, float [], float [])
					);

void adaptive_heun(	int		n,
					float	t,
					float	yin[],
					float	acc,
					float	*hh,
					float	hhmin,
					float	*hhnext,
					float	yscal[],
					void(*function)(float, float [], float [])
					);


//   FUNCTION PROTOTYPES FOR WWTP
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
					);

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
					);

void influent_character(float CODs,float CODp, float Ntot, float Ptot);
void effluent_character(float *CODs_eff,float *CODp_eff, float *Ntot_eff, float *Ptot_eff);
void temp_para(float temperatur);
void run_primclar(float t, float y[], float dy[]);
void run_reactor1(float t, float y[], float dy[]);
void run_reactor2(float t, float y[], float dy[]);
void run_reactor(float t, float flow, float vol, float SO, float y[], float dy[],float yin[]);
void point_settler();
void run_clarifier(float dt, float dtODE,float *sludge_blanket);
void run_clarifier_CSTR(float t, float y[], float dy[]);
void run_clarifier_flux(float t, float y[], float dy[]);void point_settler();
void waste_sludge_removal(float flow);

int init_open_logfile(const char *logfile_path);
int init_read_sysdata(const char *sysdata_path);
int init_read_parameter(const char *parameter_path);
int init_read_odedata(const char *odedata_path);
void init_system();
int init_stoichmat();

DLLexport int init_wtp(const char *logfile_path,
					   const char *sysdata_path,
					   const char *parameter_path,
					   const char *odedata_path);
DLLexport void exit_wtp();

void internal_print_sysdata();
void internal_print_odedata();
void internal_print_parameter();
void internal_print_result();
// ===========================================================




// ===========================================================
// GLOBAL DATA AND STRUCTURES
// ===========================================================

// Model Parameter: ASM1, PrimClar, Settler , Characterization
typedef struct
{
	float
			mh,		mh20,	mh_t,
			Ks,
			Koh,
			Kno,
			bh,		bh20,	bh_t,
			etag,
			kh,		kh20,	kh_t,
			Kx,		Kx20,	Kx_t,
			ma,		ma20,	ma_t,
			Knh,	Knh20,	Knh_t,
			Koa,
			ba,    	ba20,	ba_t,
			kP,
			yh,
			ya,
			ix,
			ixi,
			iP,
			fp,

			etamax,
			etamin,
			etacoeff,

			vsmax,
			vsmeff,
			rh,
			rp,
			fns,
			Xthresh,

			fCODsSI,
			fCODsSS,
			fCODsXS,
			fCODpXS,
			fCODpXH,
			fCODpXI
			;
} asm1_para;


// Input data
typedef struct
{
	float	primclar_vol,
			reactor1_vol,
			reactor2_vol,
			clarif_area,
			clarif_height,
			influent_frac_1,
			influent_frac_2,
			recycle,
			int_recycle,
			sludge_age,
			reactor1_O2_set,
			reactor2_O2_set
			;
} sysdata;


// Reactor data
typedef struct
{
	float	flow;
	float	vol;
	float	c[NCOMP+1];
	float	cin[NCOMP+1];
} reactor;


// Settler data
typedef struct
{
	float	hlay;
	float	vlay;
	float	Xml;
	float	sblh;
	float	Xsl[NLAY+1];
} settler;


// ODE Parameter
typedef struct
{
	float	ACCUR;				// 0.001	Accuracy
	float	DT_INIT;			// 1.0		Start guess for ODE:		dtODE*DTINIT
	float	DT_MIN;				// 0.00001	Minimum internal step		dtODE*DTMIN
	float	DT_MAX;				// 1.0/24.0	Maximum dtODE allowed in d
} ODE;
// ===========================================================
// ===========================================================

#ifdef __cplusplus
}
#endif
