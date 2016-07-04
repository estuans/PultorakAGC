/****************************************************************************
 *  CRG - ADDRESSABLE CENTRAL REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CRG.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Addressable Central Registers for the Block 1 Apollo Guidance Computer 
 *    prototype (AGC4).
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
#ifndef CRG_H
#define CRG_H

#include "reg.h"


class regQ : public reg 
{ 
public: 
	regQ() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};

class regZ : public reg 
{ 
public: 
	regZ() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};

class regLP : public reg 
{ 
public: 
	regLP() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};

class regA : public reg 
{ 
public: 
	regA() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};


class CRG
{
public:
	static regQ register_Q; // return address
	static regZ register_Z; // program counter
	static regLP register_LP; // lower accumulator
	static regA register_A; // accumulator

	static unsigned conv_WALP_LP[];
	static unsigned conv_WALP_A[];
	static unsigned conv_WLP[];
};

#endif