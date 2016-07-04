/****************************************************************************
 *  PAR - PARITY GENERATION AND TEST subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       PAR.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "PAR.h"
#include "SEQ.h"
#include "BUS.h"
#include "MBF.h"

regP PAR::register_P;
regP2 PAR::register_P2;

unsigned PAR::conv_WP[] =
{	
	BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 
};


	// NOTE: WP clears register_P before writing into it. Strictly speaking, WPx isn't
	// supposed to clear the register (should OR into the register), but in the counter
	// sequences where WPx is used, register_P is always cleared in the previous TP by
	// asserting WP with default zeroes on the write bus.


void PAR::execWP_WP()
{
			// set all bits except parity bit
		register_P.writeShift(BUS::glbl_RWBUS, PAR::conv_WP);
			// now set parity bit; in the actual AGC, this is
			// a single operation.
		if(SEQ::isAsserted(RG))
			register_P.writeField(16, 16, MBF::register_G15.read());
		else
			register_P.writeField(16, 16, 0); // clear parity bit
}

void PAR::execWP_WPx()
{
			// set all bits except parity bit
		register_P.writeShift(BUS::glbl_RWBUS, PAR::conv_WP);
			// now set parity bit; in the actual AGC, this is
			// a single operation.
		if(SEQ::isAsserted(RG))
			register_P.writeField(16, 16, MBF::register_G15.read());
		else
			register_P.writeField(16, 16, 0); // clear parity bit
}




void PAR::execWP_WP2()
{
	PAR::register_P2.write(PAR::gen1_15Parity(PAR::register_P.read()));
}



unsigned PAR::gen1_15Parity(unsigned r)
{
	//check the lower 15 bits of 'r' and return the odd parity;
	//bit 16 is ignored.
	unsigned evenParity =
		(1&(r>>0))  ^ (1&(r>>1))  ^ (1&(r>>2))  ^ (1&(r>>3))  ^
		(1&(r>>4))  ^ (1&(r>>5))  ^ (1&(r>>6))  ^ (1&(r>>7))  ^
		(1&(r>>8))  ^ (1&(r>>9))  ^ (1&(r>>10)) ^ (1&(r>>11)) ^
		(1&(r>>12)) ^ (1&(r>>13)) ^ (1&(r>>14));
	return ~evenParity & 1; // odd parity
}

unsigned PAR::genP_15Parity(unsigned r)
{
	//check all 16 bits of 'r' and return the odd parity
	unsigned evenParity =
		(1&(r>>0))  ^ (1&(r>>1))  ^ (1&(r>>2))  ^ (1&(r>>3))  ^
		(1&(r>>4))  ^ (1&(r>>5))  ^ (1&(r>>6))  ^ (1&(r>>7))  ^
		(1&(r>>8))  ^ (1&(r>>9))  ^ (1&(r>>10)) ^ (1&(r>>11)) ^
		(1&(r>>12)) ^ (1&(r>>13)) ^ (1&(r>>14)) ^ (1&(r>>15));
	return ~evenParity & 1; // odd parity
}

