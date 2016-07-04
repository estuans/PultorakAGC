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



void OUT::execRP_RA10()
{
	BUS::glbl_READ_BUS = register_OUT0.read();
}


void OUT::execWP_GENRST()
{
	register_OUT0.write(0); 
	DSP::clearOut0();

	register_OUT1.write(0);
	register_OUT2.write(0);
}

void OUT::execWP_WA10()
{
	register_OUT0.write(BUS::glbl_WRITE_BUS); 
	DSP::decodeRelayWord(BUS::glbl_WRITE_BUS); 
}



void OUT::execRP_RA11()
{
	BUS::glbl_READ_BUS = register_OUT1.read();
}





void OUT::execWP_WA11()
{
	register_OUT1.write(BUS::glbl_WRITE_BUS);
}



void OUT::execRP_RA12()
{
	BUS::glbl_READ_BUS = register_OUT2.read();
}





void OUT::execWP_WA12()
{
	register_OUT2.write(BUS::glbl_WRITE_BUS);
}



void OUT::execRP_RA13()
{
	BUS::glbl_READ_BUS = register_OUT3.read();
}



void OUT::execWP_WA13()
{
	register_OUT3.write(BUS::glbl_WRITE_BUS);	
}



void OUT::execRP_RA14()
{
	BUS::glbl_READ_BUS = register_OUT4.read();
}



void OUT::execWP_WA14()
{
	register_OUT4.write(BUS::glbl_WRITE_BUS);	
}

void OUT::execWP_RP2()
{
	if(ADR::EQU_14()) 												
		register_OUT4.writeField(16,16,PAR::register_P2.read());	
}

void OUT::execWP_GP()
{
	if(ADR::EQU_14()) 												
		register_OUT4.writeField(16,16,PAR::register_P2.read());	
}


