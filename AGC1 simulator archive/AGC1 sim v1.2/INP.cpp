/****************************************************************************
 *  INP - INPUT REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       INP.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "INP.h"
#include "SEQ.h"
#include "KBD.h"
#include "MON.h"
#include "BUS.h"

regIn0 INP::register_IN0; // input register 0
regIn1 INP::register_IN1; // input register 1
regIn2 INP::register_IN2; // input register 2
regIn3 INP::register_IN3; // input register 3

void regIn0::execRP()
{	
	if(SEQ::isAsserted(RA4))
	{	
			// Sample the state of the inputs at the moment the
			// read pulse is asserted. In the H/W implementation,
			// register 0 is a buffer, not a latch.
		writeField(5,1,KBD::kbd);
		writeField(6,6,0); // actually should be keypressed strobe
		writeField(14,14,MON::SA);
		clk();
		BUS::glbl_RWBUS|=read();
	}					
}

void regIn0::execWP() { }

void regIn1::execRP()		
{	
	if(SEQ::isAsserted(RA5))		
		BUS::glbl_RWBUS|=read();	
}
void regIn1::execWP() { }

void regIn2::execRP()		
{	
	if(SEQ::isAsserted(RA6))		
		BUS::glbl_RWBUS|=read();	
}
void regIn2::execWP() { }

void regIn3::execRP()		
{	
	if(SEQ::isAsserted(RA7))		
		BUS::glbl_RWBUS|=read();	
}
void regIn3::execWP() { }


