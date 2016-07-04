/****************************************************************************
 *  FMM - FIXED MEMORY subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       FMM.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Fixed Memory for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef FMM_H
#define FMM_H

#include "reg.h"

class regFMEM : public reg 
{ 
public: 
	regFMEM() : reg(16, "%06o") { }
	regFMEM& operator= (const unsigned& r) { write(r); return *this; }
};


class FMM
{
public:
	static regFMEM register_FMEM[]; // fixed memory
};

#endif