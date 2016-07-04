/****************************************************************************
 *  CLK - CLOCK subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CLK.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "CLK.h"
#include "INP.h"
#include "OUT.h"
#include "MBF.h"
#include "ADR.h"
#include "SEQ.h"
#include "ALU.h"
#include "CRG.h"
#include "CTR.h"
#include "INT.h"
#include "PAR.h"
#include "TPG.h"
#include "SCL.h"


	// A container for all registers. This is kept so we can iterate through
	// all registers to execute the control pulses. For simulation purposes
	// only; this has no counterpart in the hardware AGC.
reg* CLK::registerList[] = // registers are in no particular sequence
{
	&INP::register_IN0, &INP::register_IN1, &INP::register_IN2, &INP::register_IN3,
	&OUT::register_OUT0, &OUT::register_OUT1, &OUT::register_OUT2, 
	&OUT::register_OUT3, &OUT::register_OUT4,
	&MBF::register_G, &ADR::register_S, &ADR::register_BNK, &SEQ::register_SQ, &ALU::register_B,
	&CRG::register_Q, &CRG::register_Z, &CRG::register_LP, &CRG::register_A, &ALU::register_X,
	&ALU::register_Y, &ALU::register_U, 
	&SEQ::register_STA, &SEQ::register_STB, &SEQ::register_SNI,
	&SEQ::register_LOOPCTR, &ALU::register_CI, &SEQ::register_BR1, &SEQ::register_BR2, 
	&CTR::register_PSEQ, &CTR::register_PCELL, 
	&INT::register_RPCELL, &INT::register_INHINT1, &INT::register_INHINT,
	&PAR::register_P, &PAR::register_P2,
	&TPG::register_SG,
	&SCL::register_SCL,
	&SCL::register_F17, &SCL::register_F13, &SCL::register_F10,
	0 // zero is end-of-list flag
};

void CLK::clkAGC()
{
		// Now that all the inputs are set up, clock the registers so the outputs
		// can change state in accordance with the inputs.
	for(int i=0; registerList[i]; i++)
	{
		registerList[i]->clk();
	}
}
