/****************************************************************************
 *  CRG - ADDRESSABLE CENTRAL REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CRG.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "CRG.h"
#include "SEQ.h"
#include "BUS.h"

regQ CRG::register_Q; // return address
regZ CRG::register_Z; // program counter
regLP CRG::register_LP; // lower accumulator
regA CRG::register_A; // accumulator

// BUS LINE ASSIGNMENTS
// Specify the assignment of bus lines to the inputs of a register (for a 'write'
// operation into a register). Each 'conv_' array specifies the inputs into a
// single register. The index into the array corresponds to the bit position in
// the register, where the first parameter (index=0) is bit 16 of the register (msb)
// and the last parameter (index=15) is register bit 1 (lsb). The value of
// the parameter identifies the bus line assigned to that register bit. 'BX'
// means 'don't care'; i.e.: leave that register bit alone.



unsigned CRG::conv_WALP_LP[] =
{	BX, BX, B1, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX };

unsigned CRG::conv_WALP_A[] =
{	SG, SG, US, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned CRG::conv_WLP[] =
{	B1, B1, D0, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

void regQ::execRP()		
{	
	if(SEQ::isAsserted(RQ) || SEQ::isAsserted(RA1))		
		BUS::glbl_RWBUS|=read();	
}
void regQ::execWP()		
{	
	if(SEQ::isAsserted(WQ) || SEQ::isAsserted(WA1))		
		write(BUS::glbl_RWBUS);	
}

void regZ::execRP()		
{	
	if(SEQ::isAsserted(RZ) || SEQ::isAsserted(RA2))		
		BUS::glbl_RWBUS|=read();
}
void regZ::execWP()		
{	
	if(SEQ::isAsserted(WZ) || SEQ::isAsserted(WA2))		
		write(BUS::glbl_RWBUS);	
}

void regLP::execRP() 	
{	
	if(SEQ::isAsserted(RLP) || SEQ::isAsserted(RA3))		
		BUS::glbl_RWBUS|=read();	
}

void regLP::execWP()	
{	
	if(SEQ::isAsserted(WALP))	
		writeShift(BUS::glbl_RWBUS, CRG::conv_WALP_LP);
	if(SEQ::isAsserted(WLP) || SEQ::isAsserted(WA3))		
		writeShift(BUS::glbl_RWBUS, CRG::conv_WLP);
}

void regA::execRP()		
{	
	if(SEQ::isAsserted(RA) || SEQ::isAsserted(RA0))
		BUS::glbl_RWBUS|=read();	
}

void regA::execWP()		
{	
	if(SEQ::isAsserted(WA) || SEQ::isAsserted(WA0))		
		write(BUS::glbl_RWBUS);
	if(SEQ::isAsserted(WALP))	
		writeShift(BUS::glbl_RWBUS, CRG::conv_WALP_A);
}
