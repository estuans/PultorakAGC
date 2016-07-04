/****************************************************************************
 *  MBF - MEMORY BUFFER REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       MBF.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Memory Buffer Register for the Block 1 Apollo Guidance Computer prototype (AGC4).
 *
 *  SOURCES:
 *    Mostly based on information from "Logical Description for the Apollo 
 *    Guidance Computer (AGC4)", Albert Hopkins, Ramon Alonso, and Hugh 
 *    Blair-Smith, R-393, MIT Instrumentation Laboratory, 1963.
 *
 *  NOTES: 
 *    
 *****************************************************************************
 */
#ifndef MBF_H
#define MBF_H

#include "reg.h"


class regG : public reg 
{ 
public: 
	regG() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};


class MBF
{
public:
	static regG register_G; // memory buffer register

	static unsigned conv_RG[];
	static unsigned conv_WGn[];
	static unsigned conv_W20[];
	static unsigned conv_W21[];
	static unsigned conv_W22[];
	static unsigned conv_W23[];
	static unsigned conv_SBEWG[];
	static unsigned conv_SBFWG[];
	static unsigned conv_WE[];

	static unsigned getEffectiveAddress();

	static unsigned readMemory();
	static void writeMemory(unsigned data);

		// The following functions are used in the simulator,
		// but are implemented in the AGC design.
	static unsigned readMemory(unsigned address);
	static void writeMemory(unsigned address, unsigned data);
};

#endif