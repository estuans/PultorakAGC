/****************************************************************************
 *  CTR - INVOLUNTARY PRIORITY COUNTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CTR.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Involuntary Counters for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef CTR_H
#define CTR_H

#include "reg.h"

enum ctrNumber { // indexes for priority cells
	OVCTR	=0,

	TIME1	=1,
	TIME2	=2,
	TIME3	=3,
	TIME4	=4,

	UPLINK	=5,

	OUTCR1	=6,
	OUTCR2	=7,

	PIPAX	=8,
	PIPAY	=9,
	PIPAZ	=10,

	CDUX	=11,
	CDUY	=12,
	CDUZ	=13,

	OPTX	=14,
	OPTY	=15,

	TRKRX	=16,
	TRKRY	=17,
	TRKRZ	=18
};

enum ctrAddr { // octal addresses of counters
		// 030 - 056: counter registers (p. 5-31)
		// MYSTERY: what are 0030 - 0033 ????????
	OVCTR_ADDR	=0034,

	TIME1_ADDR	=0035,
	TIME2_ADDR	=0036,
	TIME3_ADDR	=0037,
	TIME4_ADDR	=0040,

	UPLINK_ADDR	=0041,

	OUTCR1_ADDR	=0042,
	OUTCR2_ADDR	=0043,

	PIPAX_ADDR	=0044,
	PIPAY_ADDR	=0045,
	PIPAZ_ADDR	=0046,

	CDUX_ADDR	=0047,
	CDUY_ADDR	=0050,
	CDUZ_ADDR	=0051,

	OPTX_ADDR	=0052,
	OPTY_ADDR	=0053,

	TRKRX_ADDR	=0054,
	TRKRY_ADDR	=0055,
	TRKRZ_ADDR	=0056
};
enum pCntrType {
	NOPSEL		=0,	// NO COUNTER
	PINCSEL		=1,	// PINC
	MINCSEL		=2,	// MINC
	SHINCSEL	=4	// SHINC
};

class regPCELL : public reg 
{ 
public: 
	regPCELL() : reg(5, "%02o") { }
};

class regPSEQ : public reg 
{ 
public: 
	regPSEQ() : reg(3, "%01o") { }
};


class CTR
{
public:
	static void execWP_GENRST();
	static void execWP_WPCTR();
	static void execRP_RSCT();
	static void execWP_WOVR();
	static void execWP_WOVC();


	static regPCELL register_PCELL; // latches the selected priority counter cell (0-19 (decimal))
	static regPSEQ register_PSEQ; // latches the selected priority counter sequence

	static unsigned pcUp[];
	static unsigned pcDn[];

	static void resetAllpc();
};

#endif