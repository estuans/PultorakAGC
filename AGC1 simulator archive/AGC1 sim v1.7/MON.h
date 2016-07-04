/****************************************************************************
 *  MON - AGC MONITOR subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       MON.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    AGC Monitor for the Block 1 Apollo Guidance Computer prototype (AGC4).
 *
 *  SOURCES:
 *    Mostly based on information from "Logical Description for the Apollo 
 *    Guidance Computer (AGC4)", Albert Hopkins, Ramon Alonso, and Hugh 
 *    Blair-Smith, R-393, MIT Instrumentation Laboratory, 1963.
 *
 *  NOTES: 
 *    
 *****************************************************************************
 */
#ifndef MON_H
#define MON_H

enum clkType {
	MCLK		=0,	// MANUAL (SINGLE, SWITCH-DRIVEN CLOCK
	SCLK		=1,	// SLOW (~1 Hz) CLOCK
	FCLK		=2	// FAST (1 Mhz) CLOCK
};

class MON
{
public:
	static void displayAGC();

	static char* MON::clkTypestring[];

	static unsigned	PURST;	// power up reset (*** MOVE TO PWR SUBSYS?? *** )
	static unsigned	SWRST;	// reset switch
	static unsigned	RUN;	// run/halt switch
	static unsigned	STEP;	// single step switch
	static unsigned	INST;	// instruction/sequence step select switch
	static clkType CLKRATE;	// clock rate
	static unsigned	SA;		// "standby allowed" SW; 0=NO (full power), 1=YES (low power)
	static unsigned BREQ;	// bus request

};

#endif