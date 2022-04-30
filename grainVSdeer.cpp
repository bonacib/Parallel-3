// Bailey Bonaci
// bonacib@oregonstate.edu
// CS 475/575 -- Spring Quarter 2022
// Project #3
// Functional Decomposition
// 100 Points
// Due: May 4
// Sources Cited: Code structure comes from Professor Mike Bailey


// Global variables
unsigned int seed = 0;

// the state of the systen
int	NowYear;		// 2022 - 2027
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population

// Parameters
const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

// starting date and time:
NowMonth =    0;
NowYear  = 2022;

// starting state (feel free to change this if you want):
NowNumDeer = 1;
NowHeight =  1.;

void
Watcher(){
    while(NowYear < 2028){
        
    }
}

// Parallel 
omp_set_num_threads( 4 );	// same as # of sections
InitBarrier(4)
#pragma omp parallel sections
{
	#pragma omp section
	{
		Deer( );
	}

	#pragma omp section
	{
		Grain( );
	}

	#pragma omp section
	{
		Watcher( );
	}

	#pragma omp section
	{
		FarmerFertilizesLand( );	
	}
}       // implied barrier -- all functions must return in order
	// to allow any of them to get past here


// Temperature and precipitation
// To keep this simple, a year consists of 12 months of 30 days each. The first 
// day of winter is considered to be January 1.

float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

float temp = AVG_TEMP - AMP_TEMP * cos( ang );
NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
if( NowPrecip < 0. )
	NowPrecip = 0.;
