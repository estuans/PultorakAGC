/****************************************************************************
 *  OUT - OUTPUT REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       OUT.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "OUT.h"
#include "SEQ.h"
#include "BUS.h"
#include "DSP.h"
#include "ADR.h"
#include "PAR.h"

regOut0 OUT::register_OUT0;	// output register 0
regOut1 OUT::register_OUT1;	// output register 1
regOut2 OUT::register_OUT2;	// output register 2
regOut3 OUT::register_OUT3;	// output register 3
regOut4 OUT::register_OUT4;	// output register 4


// Writing to OUT0 loads OUT0 and the selected display register interfaced to OUT0.
// Reading from OUT0 returns the current contents of OUT0--not the selected display register.

void regOut0::execRP()		
{	
	if(SEQ::isAsserted(RA10))	BUS::glbl_RWBUS|=read();	
}

void regOut0::execWP()		
{	if(SEQ::isAsserted(GENRST))	
	{ 
		write(0); 
		DSP::clearOut0(); 
	}
	if(SEQ::isAsserted(WA10))	
	{ 
		write(BUS::glbl_RWBUS); 
		DSP::decodeRelayWord(BUS::glbl_RWBUS); 
	}				
}

void regOut1::execRP()		
{	
	if(SEQ::isAsserted(RA11))	BUS::glbl_RWBUS|=read();	
}

void regOut1::execWP()		
{	
	if(SEQ::isAsserted(GENRST))	write(0);
	if(SEQ::isAsserted(WA11))	write(BUS::glbl_RWBUS);	
}

void regOut2::execRP()		
{	
	if(SEQ::isAsserted(RA12))	BUS::glbl_RWBUS|=read();	
}

void regOut2::execWP()		
{	
	if(SEQ::isAsserted(GENRST))	write(0);
	if(SEQ::isAsserted(WA12))	write(BUS::glbl_RWBUS);	
}

void regOut3::execRP()		
{	
	if(SEQ::isAsserted(RA13))	BUS::glbl_RWBUS|=read();	
}

void regOut3::execWP()		
{	
	if(SEQ::isAsserted(WA13))	write(BUS::glbl_RWBUS);	
}

void regOut4::execRP()		
{	
	if(SEQ::isAsserted(RA14))	BUS::glbl_RWBUS|=read();	
}

void regOut4::execWP()		
{	
	if(SEQ::isAsserted(WA14))	
		write(BUS::glbl_RWBUS);	
	if(SEQ::isAsserted(RP2) || SEQ::isAsserted(GP))		
		if(ADR::register_S.read()==014) 												
			writeField(16,16,PAR::register_P2.read());				
}
