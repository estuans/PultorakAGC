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


void INT::execRP_RRPA()
{
	BUS::glbl_RWBUS = 02000 + (register_RPCELL.read() << 2);
}



	// latches the selected priority interrupt vector (1-5)
	// also inhibits additional interrupts while an interrupt is being processed


void INT::execWP_GENRST()
{
	register_RPCELL.write(0);
	register_INHINT.write(1);
}

void INT::execWP_RPT()
{
	register_RPCELL.write(INT::getPriorityRupt());
}

void INT::execWP_KRPT()
{
	INT::rupt[register_RPCELL.read()-1] = 0;
}

void INT::execWP_CLRP()
{
	register_RPCELL.write(0);
}


	// INHINT1: inhibits interrupts for 1 instruction (on WOVI)

void INT::execWP_WOVI()
{
	if(BUS::testOverflow(BUS::glbl_RWBUS) != NO_OVF) 
		register_INHINT1.write(1);
}

void INT::execWP_CLINH1()
{
	register_INHINT1.write(0);
}


	// INHINT: inhibits interrupts on INHINT, reenables on RELINT


void INT::execWP_INH()
{
	register_INHINT.write(1);
}

void INT::execWP_CLINH()
{
	register_INHINT.write(0);
}

