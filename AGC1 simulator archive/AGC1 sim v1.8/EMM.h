/****************************************************************************
 *  EMM - ERASEABLE MEMORY subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       EMM.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Eraseable Memory for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef EMM_H
#define EMM_H

#include "reg.h"

class regEMEM : public reg 
{ 
public: 
	regEMEM() : reg(16, "%06o") { }
	regEMEM& operator= (const unsigned& r) { write(r); return *this; }
};


class EMM
{
public:
	static regEMEM register_EMEM[]; // erasable memory
};

#endif