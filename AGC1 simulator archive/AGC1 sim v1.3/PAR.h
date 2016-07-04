/****************************************************************************
 *  PAR - PARITY GENERATION AND TEST subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       PAR.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Parity Generation and Test for the Block 1 Apollo Guidance Computer 
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
#ifndef PAR_H
#define PAR_H

#include "reg.h"

class regP : public reg 
{ 
public: regP() : reg(16, "%06o") { }
	virtual void execRP(); 
	virtual void execWP();
};

class regP2 : public reg 
{ 
public: 
	regP2() : reg(1, "%01o") { }
	virtual void execRP(); 
	virtual void execWP();
};

class PAR
{
public:
	static regP2 register_P2;
	static regP register_P;

	static unsigned gen1_15Parity(unsigned r);
	static unsigned PAR::genP_15Parity(unsigned r);

	static unsigned conv_WP[];
};

#endif