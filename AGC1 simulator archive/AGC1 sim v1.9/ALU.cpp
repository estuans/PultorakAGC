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





void ALU::execRP_RB()
{
	BUS::glbl_RWBUS = register_B.read();
}

void ALU::execRP_RC()
{
	BUS::glbl_RWBUS |= register_B.outmask() & (~register_B.read());
}

void ALU::execRP_RB14()
{
	BUS::glbl_RWBUS |= 0020000;
}

void ALU::execRP_R1()
{
	BUS::glbl_RWBUS |= 0000001;
}

void ALU::execRP_R1C()
{
	BUS::glbl_RWBUS |= 0177776;
}

void ALU::execRP_R2()
{
	BUS::glbl_RWBUS |= 0000002;
}

void ALU::execRP_R22()
{
	BUS::glbl_RWBUS |= 0000022;
}

void ALU::execRP_R24()
{
	BUS::glbl_RWBUS |= 0000024;
}

void ALU::execRP_RSB()
{
	BUS::glbl_RWBUS |= 0100000;
}



	// NOTE: for GOPROG, maybe should gate it into bus and assert WB


void ALU::execWP_GENRST()
{
	register_B.write(GOPROG); // TC GOPROG
}

void ALU::execWP_WB()
{
	register_B.write(BUS::glbl_RWBUS);
}





void ALU::execWP_CI()
{
	register_CI.writeField(1,1,1);
}

void ALU::execWP_WY()
{
	if(!SEQ::isAsserted(CI)) register_CI.writeField(1,1,0);
	register_X.write(0);
	register_Y.write(BUS::glbl_RWBUS);
}










void ALU::execWP_WX()
{
	register_X.write(register_X.read() | BUS::glbl_RWBUS);
}






void ALU::execWP_WYx()
{
	register_Y.write(register_Y.read() | BUS::glbl_RWBUS);
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


void ALU::execRP_RU()
{
	BUS::glbl_RWBUS |= register_U.read();
}



