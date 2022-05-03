// Bailey Bonaci
// bonacib@oregonstate.edu
// CS 475/575 -- Spring Quarter 2022
// Project #3
// Functional Decomposition
// 100 Points
// Due: May 4
// Sources Cited: Code structure comes from Professor Mike Bailey


// include
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdlib.h>
unsigned int seed = 0;


omp_lock_t	Lock;
volatile int	NumInThreadTeam;
volatile int	NumAtBarrier;
volatile int	NumGone;

// the state of the systen
int	NowYear = 2022;		// 2022 - 2027
int	NowMonth = 0;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight = 20;		// grain height in inches
int	NowNumDeer = 2;		// number of deer in the current population

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

// Random Seed
float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

float x = Ranf( &seed, -1.f, 1.f );

// WaitBarrier Set up
// specify how many threads will be in the barrier:
//	(also init's the Lock)

void
InitBarrier( int n ) 
{
        NumInThreadTeam = n;
        NumAtBarrier = 0;
	omp_init_lock( &Lock );
}


// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier( )
{
        omp_set_lock( &Lock );
        {
                NumAtBarrier++;
                if( NumAtBarrier == NumInThreadTeam )
                {
                        NumGone = 0;
                        NumAtBarrier = 0;
                        // let all other threads get back to what they were doing
						// before this one unlocks, knowing that they might immediately
						// call WaitBarrier( ) again:
                        while( NumGone != NumInThreadTeam-1 );
                        omp_unset_lock( &Lock );
                        return;
                }
        }
        omp_unset_lock( &Lock );

        while( NumAtBarrier != 0 );	// this waits for the nth thread to arrive

        #pragma omp atomic
        NumGone++;			// this flags how many threads have returned
}

void
Watcher(){
    while(NowYear < 2028){

		// do nothing
		WaitBarrier(); // 1 

		// do nothing
		WaitBarrier();  // 2 

		// the now state of the data
		fprintf(stdout, "%2d Year; %2d Month ; Temp = %6.2f ; Precip = %6.2lf; Height of grain = %6.2lf; Deer = %2d\n", 
			NowYear, NowMonth, NowTemp, NowPrecip, NowHeight, NowNumDeer);
			
		// advance time 
		if (NowMonth == 12){
			NowMonth = 0;
			NowYear = NowYear + 1;
		}
		else{
			NowMonth = NowMonth + 1;
		}
		
		// recompute temp and precip
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;



		WaitBarrier();  // 3 
    }
}

void
Deer(){
	while(NowYear < 2028){
			// compute a temporary next-value for this quantity
			// based on the current state of the simulation:
			int nextNumDeer = NowNumDeer;
			int carryingCapacity = (int)( NowHeight );
			if( nextNumDeer <= carryingCapacity ){
				nextNumDeer++;
			}

			else{
				if( nextNumDeer > carryingCapacity ){
					nextNumDeer--;
				}
			}
					
			if( nextNumDeer < 0 ){
				nextNumDeer = 0;
			}
			
			//printf ("%s; %2d \n", "next deer", nextNumDeer);

			// DoneComputing barrier:
			WaitBarrier( );
			int NowNumDeer = nextNumDeer;
			//printf ("%s; %2d \n", "updated deer", NowNumDeer);

			// DoneAssigning barrier:
			// Do nothing
			WaitBarrier( );
			
			// DonePrinting barrier:
			// Do nothing
			WaitBarrier( );
	}
}

//grain set up 
float
SQR( float x )
{
        return x*x;
}

float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );

float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );


void
Grain(){
	while(NowYear < 2028){
			// compute a temporary next-value for this quantity
			// based on the current state of the simulation:
			float nextHeight = NowHeight;
			nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
			nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
			
			// Be sure to clamp nextHeight against zero, that is:
			if( nextHeight < 0. ){ 
				nextHeight = 0;
			}

			//fprintf(stderr, "new grain height = %6.2f; \n", nextHeight);

			// DoneComputing barrier:
			WaitBarrier( );
			float NowHeight = nextHeight;

			// DoneAssigning barrier:
			WaitBarrier( );
			

			// DonePrinting barrier:
			WaitBarrier( );
			
	}
}

void
SwarmofBeesPollinate(){
	while(NowYear < 2028){
			// compute a temporary next-value for this quantity
			// based on the current state of the simulation:
			float nextSwarmofBees = NowSwarmofBees;
			if (tempFactor > 60){
				if precipFactor < 1{
					nextSwarmofBees = 1
				}
			}
			else{
				nextSwarmofBees = 0
			}
			nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
			
			// Be sure to clamp nextHeight against zero, that is:
			if( nextHeight < 0. ){ 
				nextHeight = 0;
			}

			//fprintf(stderr, "new grain height = %6.2f; \n", nextHeight);

			// DoneComputing barrier:
			WaitBarrier( );
			float NowHeight = nextHeight;

			// DoneAssigning barrier:
			WaitBarrier( );
			

			// DonePrinting barrier:
			WaitBarrier( );
			
	}
}

int
main(int argc, char*argv[])
{
	// function prototypes
	void	InitBarrier( int );
	void	WaitBarrier( );
	// Parallel 
	omp_set_num_threads( 3 );	// same as # of sections
	InitBarrier(3);
	
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
			SwarmofBeesPollinate( );	
		}
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here

	
}

