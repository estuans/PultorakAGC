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
		// all memory bits except bit 15 (parity)
		// bit 15 is not used, so ignore it.
	regG() : reg(16, "%06o") { }
};

class regG15 : public reg 
{ 
public: 
		// memory buffer register bit 15 (parity) only
	regG15() : reg(1, "%01o") { }
};


class MBF
{
public:
	static void execRP_RG();
	static void execWP_CLG();
	static void execWP_RP2();
	static void execWP_GP();
	static void execWP_WGn();
	static void execWP_WGx();
	static void execWP_W20();
	static void execWP_W21();
	static void execWP_W22();
	static void execWP_W23();
	static void execWP_SBEWG();
	static void execWP_SBFWG();
	static void execWP_WE();


		// Bit 15 (parity) is kept in a separate register
		// because it is independently loaded.
	static regG register_G;   // memory buffer register (except for bit 15)
	static regG15 register_G15; // memory buffer register bit 15

	static unsigned conv_RG[];
	static unsigned conv_WGn[];
	static unsigned conv_W20[];
	static unsigned conv_W21[];
	static unsigned conv_W22[];
	static unsigned conv_W23[];
	static unsigned conv_SBWG[];
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