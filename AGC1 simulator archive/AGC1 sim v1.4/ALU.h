/****************************************************************************
 *  ALU - ARITHMETIC UNIT subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       ALU.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Arithmetic Unit for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef ALU_H
#define ALU_H

#include "reg.h"


class regB : public reg 
{ 
public: 
	regB() : reg(16, "%06o") { }
};

class regCI : public reg 
{ 
public: 
	regCI() : reg(1, "%01o") { }
};

class regX : public reg 
{ 
public: 
	regX() : reg(16, "%06o") { }
};

class regY : public reg 
{ 
public: 
	regY() : reg(16, "%06o") { }
};

class regU : public reg 
{ 
public: 
	regU() : reg(16, "%06o") { }
	virtual unsigned read(); 
};

class ALU
{
public:
	static void execRP_RB();
	static void execRP_RC();
	static void execRP_RB14();
	static void execRP_R1();
	static void execRP_R1C();
	static void execRP_R2();
	static void execRP_R22();
	static void execRP_R24();
	static void execRP_RSB();

	static void execWP_GENRST();
	static void execWP_WB();

	static void execWP_CI();
	static void execWP_WY();

	static void execWP_WX();
	static void execWP_WYx();

	static void execRP_RU();


	static regB register_B; // next instruction
	static regCI register_CI; // ALU carry-in flip flop
	static regX register_X; // ALU X register
	static regY register_Y; // ALU Y register
	static regU register_U; // ALU sum
};

#endif