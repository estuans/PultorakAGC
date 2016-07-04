/****************************************************************************
 *  INT - PRIORITY INTERRUPT subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       INT.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "INT.h"
#include "SEQ.h"
#include "BUS.h"

regRPCELL INT::register_RPCELL;  // latches the selected priority interrupt vector (1-5)
regINHINT1 INT::register_INHINT1;  // inhibits interrupts for 1 instruction (on WOVI)
regINHINT INT::register_INHINT;  // inhibits interrupts on INHINT, reenables on RELINT

// NOTE: the priority cells (rupt[]) are indexed 0-4, but stored in the 
// RPCELL register as 1-5; (0 in RPCELL means no interrupt)
unsigned INT::rupt[5];


void INT::resetAllRupt()
{
	for(int i=0; i<5; i++) { rupt[i]=0; }
}

	// interrupt vector; outputs 1-5 (decimal) == vector; 0 == no interrupt
unsigned INT::getPriorityRupt()
{
	for(int i=0; i<5; i++) { if(rupt[i]) return i+1; }
	return 0;
}


	// latches the selected priority interrupt vector (1-5)
	// also inhibits additional interrupts while an interrupt is being processed
void regRPCELL::execRP()	
{	
	if(SEQ::isAsserted(RRPA))
		BUS::glbl_RWBUS = 02000 + (read() << 2); 
}
void regRPCELL::execWP()	
{	
	if(SEQ::isAsserted(GENRST))	
		write(0);
	if(SEQ::isAsserted(RPT))		
		write(INT::getPriorityRupt()); 
	if(SEQ::isAsserted(KRPT))	
		INT::rupt[read()-1] = 0; 
	if(SEQ::isAsserted(CLRP))	
		write(0); 
}

	// inhibits interrupts for 1 instruction (on WOVI)
void regINHINT1::execRP()	{ }
void regINHINT1::execWP()	
{	
	if(SEQ::isAsserted(WOVI))	
		if(BUS::testOverflow(BUS::glbl_RWBUS) != NO_OVF) 
			write(1);
		if(SEQ::isAsserted(CLINH1))	
		write(0); 
}

	// inhibits interrupts on INHINT, reenables on RELINT
void regINHINT::execRP()	{ }
void regINHINT::execWP()	
{	
	if(SEQ::isAsserted(GENRST))	
		write(1);
	if(SEQ::isAsserted(INH))		
		write(1); 
	if(SEQ::isAsserted(CLINH))	
		write(0); 
}