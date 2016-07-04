/****************************************************************************
 *  ALU - ARITHMETIC UNIT subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       ALU.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "ALU.h"
#include "SEQ.h"
#include "BUS.h"

regB ALU::register_B; // next instruction
regCI ALU::register_CI; // ALU carry-in flip flop
regX ALU::register_X; // ALU X register
regY ALU::register_Y; // ALU Y register
regU ALU::register_U; // ALU sum


void regB::execRP()		
{	if(SEQ::isAsserted(RB))		BUS::glbl_RWBUS|=read();
	if(SEQ::isAsserted(RC))		BUS::glbl_RWBUS|= outmask() & (~read());
	if(SEQ::isAsserted(RB14))	BUS::glbl_RWBUS|=0020000; // ???
	if(SEQ::isAsserted(R1))		BUS::glbl_RWBUS|=0000001; 
	if(SEQ::isAsserted(R1C))	BUS::glbl_RWBUS|=0177776; 
	if(SEQ::isAsserted(R2))		BUS::glbl_RWBUS|=0000002; 
	if(SEQ::isAsserted(R22))	BUS::glbl_RWBUS|=0000022; 
	if(SEQ::isAsserted(R24))	BUS::glbl_RWBUS|=0000024; 
	if(SEQ::isAsserted(RSB))	BUS::glbl_RWBUS|=0100000; 
}

	// NOTE: for GOPROG, maybe should gate it into bus and assert WB
void regB::execWP()	
{	
	if(SEQ::isAsserted(GENRST))	write(GOPROG); // TC GOPROG
	if(SEQ::isAsserted(WB))		write(BUS::glbl_RWBUS);  
}

void regCI::execRP()	{ }
void regCI::execWP()	
{	
	if(SEQ::isAsserted(CI))		
		writeField(1,1,1);	
	if(SEQ::isAsserted(WY))		
		if(!SEQ::isAsserted(CI)) 
			writeField(1,1,0);	
}

void regX::execRP()		{ }
void regX::execWP()		
{	
	if(SEQ::isAsserted(WY))		
		write(0); 
	if(SEQ::isAsserted(WX))		
		write(read() | BUS::glbl_RWBUS); 
}

void regY::execRP()		{ }
void regY::execWP()		
{	
	if(SEQ::isAsserted(WY))		
		write(BUS::glbl_RWBUS);
	if(SEQ::isAsserted(WYx))		
		write(read() | BUS::glbl_RWBUS); 
}

unsigned regU::read()	
{
	unsigned carry = 
		(outmask()+1) & (ALU::register_X.read() + ALU::register_Y.read()); // end-around carry
	if(carry || ALU::register_CI.read()) 
		carry = 1; 
	else 
		carry = 0;
    return outmask() & (ALU::register_X.read() + ALU::register_Y.read() + carry);
}


void regU::execRP()		
{	
	if(SEQ::isAsserted(RU))		
		BUS::glbl_RWBUS|=read();	
}

void regU::execWP()		{ }