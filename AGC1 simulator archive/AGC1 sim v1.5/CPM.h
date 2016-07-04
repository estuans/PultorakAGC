/****************************************************************************
 *  CPM - CONTROL PULSE MATRIX subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CPM.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Control Pulse Matrix for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef CPM_H
#define CPM_H

#include "TPG.h"
#include "SEQ.h"


class CPM
{
public:
	static void controlPulseMatrix();

private:
		// Clear the list of currently asserted control pulses.
	static void clearControlPulses();

	// Assert the set of control pulses by adding them to the list of currently
	// active control signals.
	static void assert(cpType* pulse);

	// Assert a control pulse by adding it to the list of currently asserted
	// control pulses.
	static void assert(cpType pulse);
};

#endif