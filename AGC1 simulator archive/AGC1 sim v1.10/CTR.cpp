/****************************************************************************
 *  CTR - INVOLUNTARY PRIORITY COUNTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CTR.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "CTR.h"
#include "INT.h"
#include "BUS.h"
#include "SEQ.h"

regPCELL CTR::register_PCELL; // latches the selected priority counter cell (0-19 (decimal))
regPSEQ CTR::register_PSEQ; // latches the selected priority counter sequence

unsigned CTR::pcUp[20];
unsigned CTR::pcDn[20];

// PRIORITY COUNTERS

// ****************************************************
// The interrupt priorities are stored in RPCELL as 1-5, but
// the priority counter priorities are stored as 0-19; this
// inconsistency should be fixed, probably. Also, the method
// of address determination for the priority counters needs work


void CTR::resetAllpc()
{
	for(int i=0; i<20; i++) { pcUp[i]=0; pcDn[i]=0; }
}

	// priority encoder; outputs (0-19 (decimal))
unsigned getPriority()
{
	for(int i=0; i<20; i++) { if(CTR::pcUp[i] ^ CTR::pcDn[i]) return i; }
	return 0;
}

unsigned getSubseq()
{
	unsigned pc = getPriority();
	if((pc == 5 && CTR::pcDn[pc]) || (pc == 18 && CTR::pcDn[pc]))
		return SHINCSEL;
	else if(CTR::pcUp[pc]) 
		return PINCSEL;
	else if(CTR::pcDn[pc]) 
		return MINCSEL;
	else 
		return NOPSEL;
}

	// register_PSEQ: Disable changes to counter inputs at TP12 and TP1.
	// Selected counter address is requested at TP1.
	// Counter address is latched at TP12



void CTR::execWP_GENRST()
{
	register_PSEQ.write(0);
	resetAllpc();
}

void CTR::execWP_WPCTR()
{
	register_PSEQ.write(getSubseq());
	register_PCELL.write(getPriority());
}


	// *************************************************************
	// NOTE: need a better way to get the priority address;
	// this method requires a parallel adder. Probably change the
	// address of the counters to start at 030.

void CTR::execRP_RSCT()
{
	BUS::glbl_READ_BUS = 034 + register_PCELL.read();
}



void CTR::execWP_WOVR()
{
	CTR::pcUp[register_PCELL.read()]=0; 
	CTR::pcDn[register_PCELL.read()]=0;

			// generate various actions in response to counter overflows:
	switch(BUS::testOverflow(BUS::glbl_WRITE_BUS))
	{
		case POS_OVF: // positive overflow
			switch(register_PCELL.read()) // get the counter
			{
			case TIME1: CTR::pcUp[TIME2]=1; break; // overflow from TIME1 increments TIME2
			case TIME3: INT::rupt[T3RUPT]=1; break; // overflow from TIME3 triggers T3RUPT
			case TIME4: INT::rupt[DSRUPT]=1; break; // overflow from TIME4 triggers DSRUPT
			case UPLINK: INT::rupt[UPRUPT]=1; break; // overflow from UPLINK triggers UPRUPT
			}
			break;
		case NEG_OVF: break; // no actions for negative counter overflow
	}
}

void CTR::execWP_WOVC()
{
	switch(BUS::testOverflow(BUS::glbl_WRITE_BUS))
	{
		case POS_OVF: CTR::pcUp[OVCTR]=1; break; // incr OVCTR (034)
		case NEG_OVF: CTR::pcDn[OVCTR]=1; break; // decr OVCTR (034)
	}

}

		// register_PCELL: Overflow from the selected counter appears
		// on the bus when WOVR or WOVC is asserted;
		// it could be used to trigger an interrupt
		// or routed to increment another counter

