/****************************************************************************
 *  INT - PRIORITY INTERRUPT subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       INT.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Priority Interrupts for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef INT_H
#define INT_H

#include "reg.h"


enum ruptAddress {
		// Addresses for service routines of vectored interrupts
	T3RUPT_ADDR		=02004,	// option 1: overflow of TIME 3
	ERRUPT_ADDR		=02010,	// option 2: error signal
	DSRUPT_ADDR		=02014,	// option 3: telemetry end pulse or TIME 4 overflow
	KEYRUPT_ADDR	=02020,	// option 4: activity from MARK, keyboard, or tape reader
	UPRUPT_ADDR		=02024,	// option 5: UPLINK overflow
};


enum ruptNumber {
		// Option number (selects rupt priority cell)
		// NOTE: the priority cells (rupt[]) are indexed 0-4, but stored in the 
		// RPCELL register as 1-5; (0 in RPCELL means no interrupt)
	T3RUPT		=0,	// option 1: overflow of TIME 3
	ERRUPT		=1,	// option 2: error signal
	DSRUPT		=2,	// option 3: telemetry end pulse or TIME 4 overflow
	KEYRUPT		=3,	// option 4: activity from MARK, keyboard, or tape reader
	UPRUPT		=4,	// option 5: UPLINK overflow
};

class regRPCELL : public reg 
{ 
public: 
	regRPCELL() : reg(5, "%02o") { }
};
// also inhibits additional interrupts while an interrupt is being processed

class regINHINT1 : public reg 
{ 
public:
	regINHINT1() : reg(1, "%01o") { }
};

class regINHINT : public reg 
{ 
public: 
	regINHINT() : reg(1, "%01o") { }
};


class INT
{
public:
	static void execRP_RRPA();
	static void execWP_GENRST();
	static void execWP_RPT();
	static void execWP_KRPT();
	static void execWP_CLRP();
	static void execWP_WOVI();
	static void execWP_CLINH1();
	static void execWP_INH();
	static void execWP_CLINH();


	static regRPCELL register_RPCELL;  // latches the selected priority interrupt vector (1-5)
	static regINHINT1 register_INHINT1;  // inhibits interrupts for 1 instruction (on WOVI)
	static regINHINT register_INHINT;  // inhibits interrupts on INHINT, reenables on RELINT

	static unsigned rupt[];

	static unsigned getPriorityRupt();

private:
	static void resetAllRupt();

};

#endif