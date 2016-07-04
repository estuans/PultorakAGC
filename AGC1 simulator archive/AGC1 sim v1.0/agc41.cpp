/*
 *  AGC4 (Apollo Guidance Computer) BLOCK I Simulator version 1.0
 *
 *  Mostly based on information from "Logical Description for the Apollo Guidance
 *  Computer (AGC4)", Albert Hopkins, Ramon Alonso, and Hugh Blair-Smith, R-393,
 *  MIT Instrumentation Laboratory, 1963.
 *
 *  NOTE: set tabs to 4 spaces to keep columns formatted correctly.
 *
 *  8/18/01
 */
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "agc41.h"

enum subseq {
	TC0		=0,
	CCS0	=1,
	CCS1	=2,
	NDX0	=3,
	NDX1	=4,
	RSM3	=5,
	XCH0	=6,
	CS0		=7,
	TS0		=8,
	AD0		=9,
	MASK0	=10,
	MP0		=11,
	MP1		=12,
	MP3		=13,
	DV0		=14,
	DV1		=15,
	SU0		=16,
	RUPT1	=17,
	RUPT3	=18,
	STD2	=19,
	PINC0	=20,
	MINC0	=21,
	SHINC0	=22,
	NO_SEQ	=23
};

char* subseqString[] =
{
	"TC0",
	"CCS0",
	"CCS1",
	"NDX0",
	"NDX1",
	"RSM0",
	"XCH0",
	"CS0",
	"TS0",
	"AD0",
	"MASK0",
	"MP0",
	"MP1",
	"MP3",
	"DV0",
	"DV1",
	"SU0",
	"RUPT1",
	"RUPT3",
	"STD2",
	"PINC0",
	"MINC0",
	"SHINC0",
	"NO_SEQ"
};

enum scType { // identifies subsequence for a given instruction
	SUB0=0,		// ST2=0, ST1=0
	SUB1=1,		// ST2=0, ST1=1
	SUB2=2,		// ST2=1, ST1=0
	SUB3=3		// ST2=1, ST1=1
};

enum brType {
	BR00	=0,	// BR1=0, BR2=0
	BR01	=1,	// BR1=0, BR2=1
	BR10	=2,	// BR1=1, BR2=0
	BR11	=3,	// BR1=1, BR2=1
	NO_BR	=4	// NO BRANCH
};

enum tpType {
	STBY		=0,
	PWRON		=1,

	TP1			=2,		// TIME PULSE 1: start of memory cycle time (MCT)
	TP2			=3,
	TP3			=4,
	TP4			=5,
	TP5			=6,
	TP6			=7,		// EMEM is available in G register by TP6
	TP7			=8,		// FMEM is available in G register by TP7
	TP8			=9,
	TP9			=10,
	TP10		=11,	// G register written to memory beginning at TP10
	TP11		=12,	// TIME PULSE 11: end of memory cycle time (MCT)
	TP12		=13,	// select new subsequence/select new instruction

	SRLSE		=14,	// step switch release
	WAIT		=15
};

char* tpTypestring[] = // must correspond to tpType enumerated type
{
	"STBY", "PWRON", "TP1", "TP2", "TP3", "TP4", "TP5", "TP6", "TP7", "TP8",
	"TP9", "TP10", "TP11", "TP12", "SRLSE", "WAIT"
};

enum clkType {
	MCLK		=0,	// MANUAL (SINGLE, SWITCH-DRIVEN CLOCK
	SCLK		=1,	// SLOW (~1 Hz) CLOCK
	FCLK		=2	// FAST (1 Mhz) CLOCK
};

char* clkTypestring[] = // must correspond to clkType enumerated type
{
	"M", "S", "F"
};

enum pCntrType {
	NOPSEL		=0,	// NO COUNTER
	PINCSEL		=1,	// PINC
	MINCSEL		=2,	// MINC
	SHINCSEL	=4	// SHINC
};

enum cpType { // **inferred; not defined in orignal R393 AGC4 spec.
	NO_PULSE,
	CI,		// Carry in
	CLG,	// Clear G
	CLINH,	// Clear INHINT**
	CLINH1,	// Clear INHINT1**
	CLISQ,	// Clear SNI**
	CLBR,	// Clear branch register** (delete this??)
	CLCTR,	// Clear loop counter**
	CLRP,	// Clear RPCELL**
	CLSTA,	// Clear state counter A (STA)**
	CLSTB,	// Clear state counter B (STB)**
	CTR,	// Loop counter
	GENRST,	// General Reset**
	GP,		// Generate Parity
	INH,	// Set INHINT**
	KRPT,	// Knock down Rupt priority
	NISQ,	// New instruction to the SQ register
	RA0,	// Read register at address 0 (A)
	RA1,	// Read register at address 1 (Q)
	RA2,	// Read register at address 2 (Z)
	RA3,	// Read register at address 3 (LP)
	RA4,	// Read register at address 4
	RA5,	// Read register at address 5
	RA6,	// Read register at address 6
	RA7,	// Read register at address 7
	RA10,	// Read register at address 10 (octal)
	RA11,	// Read register at address 11 (octal)
	RA12,	// Read register at address 12 (octal)
	RA13,	// Read register at address 13 (octal)
	RA14,	// Read register at address 14 (octal)
	RA,		// Read A
	RB,		// Read B
	RB14,	// Read bit 14
	RC,		// Read C
	RG,		// Read G
	RLP,	// Read LP
	RPT,	// Read RUPT opcode **
	RP2,	// Read parity 2
	RQ,		// Read Q
	RRPA,	// Read RUPT address
	RSB,	// Read sign bit
	RSC,	// Read special and central
	RSCT,	// Read selected counter address
	RU,		// Read sum
	RZ,		// Read Z
	R1,		// Read 1
	R1C,	// Read 1 complimented
	R2,		// Read 2
	R22,	// Read 22
	R24,	// Read 24
	SBEWG,	// Write G from E-memory
	SBFWG,	// Write G from F-memory
	SETSTB,	// Set the ST1 bit of STB
	ST1,	// Stage 1
	ST2,	// Stage 2
	TMZ,	// Test for minus zero
	TOV,	// Test for overflow
	TP,		// Test parity
	TRSM,	// Test for resume
	TSGN,	// Test sign
	TSGN2,	// Test sign 2
	WA0,	// Write register at address 0 (A)
	WA1,	// Write register at address 1 (Q)
	WA2,	// Write register at address 2 (Z)
	WA3,	// Write register at address 3 (LP)
	WA10,	// Write register at address 10 (octal)
	WA11,	// Write register at address 11 (octal)
	WA12,	// Write register at address 12 (octal)
	WA13,	// Write register at address 13 (octal)
	WA14,	// Write register at address 14 (octal)
	WA,		// Write A
	WALP,	// Write A and LP
	WB,		// Write B
	WBK,	// Write BNK
	WE,		// Write E-MEM from G
	WG,		// Write G
	WGn,	// Write G (normal gates)**
	WGx,	// Write G (do not reset)
	WLP,	// Write LP
	WOVC,	// Write overflow counter
	WOVI,	// Write overflow RUPT inhibit
	WOVR,	// Write overflow
	WP,		// Write P
	WPx,	// Write P (do not reset)
	WP2,	// Write P2
	WPCTR,	// Write PCTR (latch priority counter sequence)**
	WQ,		// Write Q
	WS,		// Write S
	WSC,	// Write special and central
	WSQ,	// Write SQ
	WSTB,	// Write stage counter B (STB)**
	WX,		// Write X
	WY,		// Write Y
	WYx,	// Write Y (do not reset)
	WZ,		// Write Z
	W20,	// Write into CYR
	W21,	// Write into SR
	W22,	// Write into CYL
	W23		// Write into SL
};

char* cpTypeString[] = 
{
	"NO_PULSE", "CI", "CLG", "CLINH", "CLINH1", "CLISQ", "CLBR", "CLCTR", "CLRP", 
	"CLSTA", "CLSTB", "CTR", "GENRST", "GP", "INH", "KRPT", "NISQ",
	"RA0", "RA1", "RA2", "RA3", "RA4", "RA5", "RA6", "RA7", "RA10", "RA11", "RA12", "RA13", "RA14", "RA", "RB",
	"RB14", "RC", "RG", "RLP", "RPT", "RP2", "RQ", "RRPA", "RSB", "RSC", "RSCT",
	"RU", "RZ", "R1", "R1C", "R2", "R22", "R24", "SBEWG", "SBFWG", "SETSTB", 
	"ST1", "ST2", "TMZ", 
	"TOV", "TP", "TRSM", "TSGN", "TSGN2", 
	"WA0", "WA1", "WA2", "WA3","WA10", "WA11", "WA12", "WA13", "WA14", 
	"WA", "WALP", "WB", "WBK", "WE", "WG", "WGn", "WGx",
	"WLP", "WOVC", "WOVI", "WOVR", "WP", "WPx", "WP2", "WPCTR", "WQ", "WS", "WSC", "WSQ", "WSTB",
	"WX", "WY", "WYx", "WZ", "W20", "W21", "W22", "W23"
};

#define MAX_IPULSES 5 // no more than 5 instruction-generated pulses active at any time
struct controlSubStep {
	brType br; // normally no branch (NO_BR)
	cpType pulse[MAX_IPULSES]; // contains 0 - MAXPULSES control pulses
};

struct controlStep {
	controlSubStep substep[4]; // indexed by brType (BR00, BR01, BR10, BR11)
};

struct subsequence {
	controlStep tp[11]; // indexed by tpType (TP1-TP11)
};

struct sequence {
	subsequence* subseq[4]; // indexed by scType
};

#define STEP_INACTIVE \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}

#define STEP(p1, p2, p3, p4, p5) \
	NO_BR,	{ p1, p2, p3, p4, p5}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}, \
	NO_BR,	{NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE, NO_PULSE}

subsequence SUB_TC0 = {
	STEP (	RB,		WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RA,		WOVI,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RZ,		WQ,			GP,			TP,			NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	NISQ,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_CCS0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RZ,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP (	RG,		RSC,		WB,			TSGN,		WP			), // TP 6
	BR00,	RC,		TMZ,		NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 7
	BR01,	RC,		TMZ,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RB,		TMZ,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RB,		TMZ,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR00,	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 8
	BR01,	R1,		WX,			GP,			TP,			NO_PULSE,
	BR10,	R2,		WX,			GP,			TP,			NO_PULSE,
	BR11,	R1,		R2,			WX,			GP,			TP,
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	BR00,	RC,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 10
	BR01,	WA,		R1C,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RB,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	WA,		R1C,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RU,		ST1,		WZ,			NO_PULSE,	NO_PULSE	) // TP 11
};

subsequence SUB_CCS1 = {		
	STEP (	RZ,		WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP (	RA,		WY,			CI,			NO_PULSE,	NO_PULSE	), // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RU,		WB,			GP,			TP,			NO_PULSE	), // TP 8
	STEP_INACTIVE, // TP 9
	STEP (	RC,		WA,			WOVI,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	RG,		RSC,		WB,			NISQ,		NO_PULSE	)  // TP 11
};

subsequence SUB_NDX0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RA,		WOVI,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	TRSM,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	ST1,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_NDX1 = {		
	STEP (	RZ,		WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP (	RB,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RB,		WX,			GP,			TP,			NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	RU,		WB,			WOVI,		NISQ,		NO_PULSE	), // TP 11
};

subsequence SUB_RSM3 = {
	STEP (	R24,	WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	STEP_INACTIVE, // TP 8
	STEP_INACTIVE, // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	NISQ,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_XCH0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WP,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	WP2,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RA,		WSC,		WG,			RP2,		NO_PULSE	), // TP 9
	STEP (	RB,		WA,			WOVI,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	ST2,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_CS0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RC,		WA,			WOVI,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	ST2,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_TS0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WB,			TOV,		WP,			NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 4
	BR01,	RZ,		WY,			CI,			NO_PULSE,	NO_PULSE,	   // overflow
	BR10,	RZ,		WY,			CI,			NO_PULSE,	NO_PULSE,	   // underflow
	BR11,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 5
	BR01,	R1,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	WA,		R1C,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP_INACTIVE, // TP 6
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 7
	BR01,	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	GP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RA,		WOVI,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	ST2,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_AD0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RB,		WX,			GP,			TP,			NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	RU,		WA,			WOVC,		ST2,		WOVI		), // TP 11
};

// Note: AND is performed using DeMorgan's Theorem: the inputs are inverted, a
// logical OR is performed, and the result is inverted. The implementation of the
// OR (at TP8) is somewhat unorthodox: the inverted inputs are in registers U
// and C. The OR is achieved by gating both registers onto the read/write bus
// simultaneously. (The bus only transfers logical 1's; register-to-register transfers
// are performed by clearing the destination register and then transferring the
// 1's from the source register to the destination). When the 1's from both
// registers are simultaneously gated onto the bus, the word on the bus is a logical
// OR of both registers.
subsequence SUB_MASK0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WB,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RC,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RU,		RC,			WA,			GP,			TP			), // TP 8  (CHANGED)
	STEP_INACTIVE, // TP 9
	STEP (	RA,		WB,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10 (CHANGED)
	STEP (	RC,		WA,			ST2,		WOVI,		NO_PULSE	), // TP 11
};

subsequence SUB_MP0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WB,			TSGN,		NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	RSC,	WG,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	BR00,	RB,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 4
	BR01,	RB,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RC,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RC,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RLP,	WA,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 5
	STEP_INACTIVE, // TP 6
	BR00,	RG,		WY,			WP,			NO_PULSE,	NO_PULSE,	   // TP 7
	BR01,	RG,		WY,			WP,			NO_PULSE,	NO_PULSE,
	BR10,	RG,		WB,			WP,			NO_PULSE,	NO_PULSE,
	BR11,	RG,		WB,			WP,			NO_PULSE,	NO_PULSE,
	BR00,	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 8
	BR01,	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RC,		WY,			GP,			TP,			NO_PULSE,
	BR11,	RC,		WY,			GP,			TP,			NO_PULSE,
	STEP (	RU,		WB,			TSGN2,		NO_PULSE,	NO_PULSE	), // TP 9
	BR00,	RA,		WLP,		TSGN,		NO_PULSE,	NO_PULSE,	   // TP 10
	BR01,	RA,		RB14,		WLP,		TSGN,		NO_PULSE,
	BR10,	RA,		WLP,		TSGN,		NO_PULSE,	NO_PULSE,
	BR11,	RA,		RB14,		WLP,		TSGN,		NO_PULSE,
	BR00,	ST1,	WALP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 11
	BR01,	R1,		ST1,		WALP,		R1C,		NO_PULSE,
	BR10,	RU,		ST1,		WALP,		NO_PULSE,	NO_PULSE,
	BR11,	RU,		ST1,		WALP,		NO_PULSE,	NO_PULSE,
};

subsequence SUB_MP1 = {		
	STEP (	RA,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RLP,	WA,			TSGN,		NO_PULSE,	NO_PULSE	), // TP 2
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 3
	BR01,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RA,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP (	RLP,	TSGN,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 5
	STEP (	RU,		WALP,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 6
	STEP (	RA,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 8
	BR01,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RLP,	WA,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RA,		WLP,		CTR,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	RU,		ST1,		WALP,		NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_MP3 = {		
	STEP (	RZ,		WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP (	RLP,	TSGN,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP (	RA,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 5
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 6
	BR01,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RB,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RLP,	WA,			GP,			TP,			NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RA,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	RU,		WALP,		NISQ,		NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_DV0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WB,			TSGN,		NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	RSC,	WG,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	BR00,	RC,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 4
	BR01,	RC,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR00,	R1,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 5
	BR01,	R1,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	R2,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	R2,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RA,		WQ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 6
	STEP (	RG,		WB,			TSGN,		WP,			NO_PULSE	), // TP 7
	STEP (	RB,		WA,			GP,			TP,			NO_PULSE	), // TP 8
	BR00,	RLP,	R2,			WB,			NO_PULSE,	NO_PULSE,	   // TP 9
	BR01,	RLP,	R2,			WB,			NO_PULSE,	NO_PULSE,
	BR10,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR00,	RB,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 10
	BR01,	RB,		WLP,		NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RC,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RC,		WA,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	R1,		ST1,		WB,			NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_DV1 = {		
	STEP (	R22,	WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RQ,		WG,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	RG,		WQ,			WY,			RSB,		NO_PULSE	), // TP 3
	STEP (	RA,		WX,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP (	RLP,	TSGN2,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RU,		TSGN,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	BR00,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 8
	BR01,	NO_PULSE,NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RU,		WQ,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RU,		WQ,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR00,	RB,		RSB,		WG,			NO_PULSE,	NO_PULSE,	   // TP 9
	BR01,	RB,		RSB,		WG,			NO_PULSE,	NO_PULSE,
	BR10,	RB,		WG,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR11,	RB,		WG,			NO_PULSE,	NO_PULSE,	NO_PULSE,
	STEP (	RG,		WB,			TSGN,		NO_PULSE,	NO_PULSE	), // TP 10
	BR00,	ST1,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,	   // TP 11
	BR01,	ST1,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE,
	BR10,	RC,		WA,			ST2,		NO_PULSE,	NO_PULSE,
	BR11,	RB,		WA,			ST2,		NO_PULSE,	NO_PULSE,
};

subsequence SUB_SU0 = {		
	STEP (	RB,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RA,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	RC,		WX,			GP,			TP,			NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	RU,		WA,			WOVC,		ST2,		WOVI		), // TP 11
};

subsequence SUB_RUPT1 = {		
	STEP (	R24,	WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP_INACTIVE, // TP 7
	STEP_INACTIVE, // TP 8
	STEP (	RZ,		WG,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 10
	STEP (	ST1,	ST2,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_RUPT3 = {		
	STEP (	RZ,		WS,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP (	RRPA,	WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 2
	STEP (	RZ,		KRPT,		WG,			NO_PULSE,	NO_PULSE	), // TP 3
	STEP_INACTIVE, // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP_INACTIVE, // TP 7
	STEP_INACTIVE, // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	ST2,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_STD2 = {		
	STEP (	RZ,		WY,			WS,			CI,			NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	RU,		WZ,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP_INACTIVE, // TP 6
	STEP (	RG,		RSC,		WB,			WP,			NO_PULSE	), // TP 7
	STEP (	GP,		TP,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RB,		WSC,		WG,			NO_PULSE,	NO_PULSE	), // TP 9
	STEP_INACTIVE, // TP 10
	STEP (	NISQ,	NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 11
};

subsequence SUB_PINC = {		
	STEP (	WS,		RSCT,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	R1,		WY,			NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP (	RG,		WX,			WP,			NO_PULSE,	NO_PULSE	), // TP 6
	STEP (	TP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	STEP (	WP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RU,		CLG,		WPx,		NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RU,		WGx,		WOVR,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP_INACTIVE, // TP 11
};

subsequence SUB_MINC = {		
	STEP (	WS,		RSCT,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	WY,		R1C,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP (	RG,		WX,			WP,			NO_PULSE,	NO_PULSE	), // TP 6
	STEP (	TP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	STEP (	WP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RU,		CLG,		WPx,		NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RU,		WGx,		WOVR,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP_INACTIVE, // TP 11
};

subsequence SUB_SHINC = {		
	STEP (	WS,		RSCT,		NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 1
	STEP_INACTIVE, // TP 2
	STEP (	WG,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 3
	STEP (	WY,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 4
	STEP_INACTIVE, // TP 5
	STEP (	RG,		WYx,		WX,			WP,			NO_PULSE	), // TP 6
	STEP (	TP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 7
	STEP (	WP,		NO_PULSE,	NO_PULSE,	NO_PULSE,	NO_PULSE	), // TP 8
	STEP (	RU,		CLG,		WPx,		NO_PULSE,	NO_PULSE	), // TP 9
	STEP (	RU,		WGx,		WOVR,		NO_PULSE,	NO_PULSE	), // TP 10
	STEP_INACTIVE, // TP 11
};

	// Bit positions for the input status word that controls
	// the AGC state.
enum { 
	RUN_SW =1, // RUN/HALT switch (front panel)
	SS_SW  =2, // SINGLE step switch (front panel)
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

//-----------------------------------------------------------------------
// CONTROL LOGIC

#define MAXPULSES 15

// Globals:
subseq		glbl_subseq;				// currently decoded instruction subsequence
cpType		glbl_cp[MAXPULSES];			// current set of asserted control pulses (MAXPULSES)
unsigned	glbl_RWBUS;					// read/write bus for xfer between central regs

// Inputs
keyInType	kbd=KEYIN_NONE;				// latches the last key entry from the DSKY

unsigned	PURST=1;					// power up reset
unsigned	SWRST=0;					// reset switch

unsigned	RUN=0;						// run/halt switch
unsigned	STEP=0;						// single step switch
unsigned	INST=1;						// instruction/sequence step select switch
clkType		CLKRATE=MCLK;				// clock rate

unsigned	SA=0;						// "standby allowed" SW; 0=NO (full power), 1=YES (low power)
unsigned	F17=0;						// scaler output F17
unsigned	F13=0;						// scaler output F13

unsigned	BREQ=0;						// bus request



	// Test the currently asserted control pulses; return true if the specified
	// control pulse is active.
bool isAsserted(cpType pulse)
{
	for(unsigned i=0; i<MAXPULSES; i++)
		if(glbl_cp[i] == pulse) return true;
	return false;
}

	// Clear the list of currently asserted control pulses.
void clearControlPulses()
{
	for(unsigned i=0; i<MAXPULSES; i++)
		glbl_cp[i] = NO_PULSE;
}

	// Assert a control pulse by adding it to the list of currently asserted
	// control pulses.
void assert(cpType pulse)
{
	for(unsigned i=0; i<MAXPULSES; i++)
	{
		if(glbl_cp[i] == NO_PULSE)
		{
			glbl_cp[i] = pulse;
			break;
		}
	}
}

	// Assert the set of control pulses by adding them to the list of currently
	// active control signals.
void assert(cpType* pulse)
{
	int j=0;
	for(unsigned i=0; i<MAXPULSES && j<MAX_IPULSES && pulse[j] != NO_PULSE; i++)
	{
		if(glbl_cp[i] == NO_PULSE)
		{
			glbl_cp[i] = pulse[j];
			j++;
		}
	}
}

	// Return a string containing the names of all asserted control pulses.
char* getControlPulses()
{
	static char buf[MAXPULSES*6];
	strcpy(buf,"");

	for(unsigned i=0; i<MAXPULSES && glbl_cp[i] != NO_PULSE; i++)
	{
		strcat(buf, cpTypeString[glbl_cp[i]]);
		strcat(buf," ");
	}
	//if(strcmp(buf,"") == 0) strcat(buf,"NONE");
	return buf;
}

//-----------------------------------------------------------------------
// BUS LINE ASSIGNMENTS
// Specify the assignment of bus lines to the inputs of a register (for a 'write'
// operation into a register). Each 'conv_' array specifies the inputs into a
// single register. The index into the array corresponds to the bit position in
// the register, where the first parameter (index=0) is bit 16 of the register (msb)
// and the last parameter (index=15) is register bit 1 (lsb). The value of
// the parameter identifies the bus line assigned to that register bit. 'BX'
// means 'don't care'; i.e.: leave that register bit alone.

enum {	PM=16,	// parity bit in memory
		SGM=15,	// sign bit in memory
		SG=16,	// sign (S2; one's compliment)
		P=15,	// parity bit in register G
		US=15,	// uncorrected sign (S1; overflow), except in register G
		B14=14, B13=13, B12=12, B11=11, B10=10, B9=9, B8=8, 
		B7=7, B6=6, B5=5, B4=4, B3=3, B2=2, B1=1, 
		BX=0	// ignore
};

unsigned conv_W20[] =
{	B1, BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned conv_W21[] =
{	SG, BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned conv_W22[] =
{	B14, BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };

unsigned conv_W23[] =
{	SG, BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };

unsigned conv_WGn[] =
{	SG, BX, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned conv_WALP_LP[] =
{	BX, BX, B1, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX };

unsigned conv_WALP_A[] =
{	SG, SG, US, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned conv_WLP[] =
{	B1, B1, BX, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned conv_SBEWG[] =
{	SGM, PM, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned conv_SBFWG[] =
{	SGM, PM, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned conv_RG[] =
{	SG, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned conv_WE[] =
{	P, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned conv_WBK[] =
{	BX, BX, B14, B13, B12, B11, BX, BX, BX, BX, BX, BX, BX, BX, BX, BX };

unsigned conv_WP[] =
{	BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned shiftData(unsigned out, unsigned in, unsigned* ib)
{
		// iterate through each bit of the output word, copying in bits from the input
		// word and transposing bit position according to the specification (ib)
	for(unsigned i=0; i<16; i++)
	{
		if(ib[i] == BX) continue; // BX is 'don't care', so leave it alone

			// zero the output bit at 'ob', where ob specifies a bit
			// position (numbered 16-1, where 1 is lsb)
		unsigned ob = 16-i;
		unsigned obmask = 1 << (ob - 1); // create mask for output bit
		out &= ~obmask;

			// copy input bit ib[i] to output bit 'ob', where ib and ob
			// specify bit positions (numbered 16-1, where 1 is lsb)
		unsigned ibmask = 1 << (ib[i] - 1); // create mask for input bit
		unsigned inbit = in & ibmask;
		
		int shift = ib[i]-ob;
		if(shift<0)
			inbit = inbit << abs(shift);
		else if(shift > 0)
			inbit = inbit >> shift;
		out |= inbit;
	}
	return out;
}

//-----------------------------------------------------------------------
// REGISTERS


class reg
{
public:
	virtual unsigned read() { return mask & slaveVal; }
	virtual void write(unsigned v) { masterVal = mask & v; }
	void clk() { slaveVal = masterVal; }

	virtual void assertP() { }
	virtual void execRP()  { }
	virtual void execWP()  { }

	unsigned readField(unsigned msb, unsigned lsb); // bitfield numbered n - 1
	void writeField(unsigned msb, unsigned lsb, unsigned v); // bitfield numbered n - 1

protected:
	reg(unsigned s, char* fs) : size(s), mask(0), masterVal(0), slaveVal(0), fmtString(fs) { mask = buildMask(size);}

	unsigned outmask() { return mask; }
	static unsigned buildMask(unsigned s);

	friend ostream& operator << (ostream& os, const reg& r)
		{ char buf[32]; sprintf(buf, r.fmtString, r.slaveVal); os << buf; return os; }


private:
	unsigned	size; // bits
	unsigned	masterVal;
	unsigned	slaveVal;
	unsigned	mask;
	char*		fmtString;

	reg(); // prevent instantiation of default constructor
};

unsigned reg::buildMask(unsigned s)
{
	unsigned msk = 0;
	for(unsigned i=0; i<s; i++)
	{
		msk = (msk << 1) | 1;
	}
	return msk;
}

unsigned reg::readField(unsigned msb, unsigned lsb)
{
	return (slaveVal >> (lsb-1)) & buildMask((msb-lsb)+1);
}

void reg::writeField(unsigned msb, unsigned lsb, unsigned v)
{
	unsigned fmask = buildMask((msb-lsb)+1) << (lsb-1);
	v = (v << (lsb-1)) & fmask;
	masterVal = (masterVal &(~fmask)) | v;
}

class regIn0 : public reg { public: regIn0() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_IN0; // input register 0

class regIn1 : public reg { public: regIn1() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_IN1; // input register 1

class regIn2 : public reg { public: regIn2() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_IN2; // input register 2

class regIn3 : public reg { public: regIn3() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_IN3; // input register 3


class regOut0 : public reg { public: regOut0() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_OUT0; // output register 0

class regOut1 : public reg { public: regOut1() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_OUT1; // output register 1

class regOut2 : public reg { public: regOut2() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_OUT2; // output register 2

class regOut3 : public reg { public: regOut3() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_OUT3; // output register 3

class regOut4 : public reg { public: regOut4() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_OUT4; // output register 4


class regG : public reg { public: regG() : reg(16, "%06o") { }
	virtual void assertP(); virtual void execRP(); virtual void execWP();
} register_G; // memory buffer register

class regS : public reg { public: regS() : reg(12, "%04o") { }
	virtual void execRP(); virtual void execWP();
} register_S; // address register

class regSQ : public reg { public: regSQ() : reg(4, "%02o") { }
	virtual void execRP(); virtual void execWP();
} register_SQ; // instruction register

class regB : public reg { public: regB() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_B; // next instruction

class regQ : public reg { public: regQ() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_Q; // return address

class regZ : public reg { public: regZ() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_Z; // program counter

class regLP : public reg { public: regLP() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_LP; // lower accumulator

class regA : public reg { public: regA() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_A; // accumulator

class regCI : public reg { public: regCI() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_CI; // ALU carry-in flip flop

class regX : public reg { public: regX() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_X; // ALU X register

class regY : public reg { public: regY() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_Y; // ALU Y register

class regU : public reg { public: regU() : reg(16, "%06o") { }
	virtual unsigned read(); virtual void execRP(); virtual void execWP();
} register_U; // ALU sum

class regBNK : public reg { public: regBNK() : reg(4, "%02o") { }
	virtual void execRP(); virtual void execWP();
} register_BNK;	// bank register

class regSTA : public reg { public: regSTA() : reg(2, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_STA;	// stage counter A

class regSTB : public reg { public: regSTB() : reg(2, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_STB;	// stage counter B

class regBR1 : public reg { public: regBR1() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_BR1;	// branch register1

class regBR2 : public reg { public: regBR2() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_BR2;	// branch register2

class regSNI : public reg { public: regSNI() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_SNI;	// select next intruction latch

class regCTR : public reg { public: regCTR() : reg(3, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_LOOPCTR;	// loop counter

class regEMEM : public reg { public: regEMEM() : reg(16, "%06o") { }
	regEMEM& operator= (const unsigned& r) { write(r); return *this; }
} register_EMEM[1024]; // erasable memory

class regFMEM : public reg { public: regFMEM() : reg(16, "%06o") { }
	regFMEM& operator= (const unsigned& r) { write(r); return *this; }
} register_FMEM[1024*12]; // fixed memory

//-----------------------------------------------------------------------
// Priority counter

class regPCELL : public reg { public: regPCELL() : reg(5, "%02o") { }
	virtual void execRP(); virtual void execWP();
} register_PCELL; // latches the selected priority counter cell (0-19 (decimal))

class regPSEQ : public reg { public: regPSEQ() : reg(3, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_PSEQ; // latches the selected priority counter sequence

//-----------------------------------------------------------------------
// Priority interrupt

class regRPCELL : public reg { public: regRPCELL() : reg(5, "%02o") { }
	virtual void execRP(); virtual void execWP();
} register_RPCELL;  // latches the selected priority interrupt vector (1-5)
// also inhibits additional interrupts while an interrupt is being processed

class regINHINT1 : public reg { public: regINHINT1() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_INHINT1;  // inhibits interrupts for 1 instruction (on WOVI)

class regINHINT : public reg { public: regINHINT() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_INHINT;  // inhibits interrupts on INHINT, reenables on RELINT

//-----------------------------------------------------------------------
// Parity Test and Generation

class regP : public reg { public: regP() : reg(16, "%06o") { }
	virtual void execRP(); virtual void execWP();
} register_P;

class regP2 : public reg { public: regP2() : reg(1, "%01o") { }
	virtual void execRP(); virtual void execWP();
} register_P2;

//-----------------------------------------------------------------------
// Start/Stop Logic and Time Pulse Generator

class regSG : public reg { public: regSG() : reg(4, "%02o") { }
	virtual void execRP(); virtual void execWP();
} register_SG;

//-----------------------------------------------------------------------
// MEMORY LOGIC

unsigned bankDecoder()
{
	// Memory is organized into 13 banks of 1K words each. The banks are numbered
	// 0-12. Bank 0 is erasable memory; banks 1-12 are fixed (rope) memory. The 10
	// lower bits in the S register address memory inside a bank. The 2 upper bits
	// in the S register select the bank. If the 2 upper bits are both 1, the 4-bit
	// bank register is used to select the bank.
	// 12  11   Bank
	//  0   0      0      erasable memory
	//  0   1      1      fixed-fixed 1 memory
	//  1   0      2      fixed-fixed 2 memory
	//  1   1      3-12   fixed-switchable memory (bank register selects bank)
	unsigned bank = register_S.readField(12,11);
	if(bank == 3)
	{
			// fixed-switchable
		if(register_BNK.read() <= 03)	// defaults to 6000 - 7777
			return 03;
		else
			return register_BNK.read();	// 10000 - 31777
	}
	else
		return bank;	// erasable or fixed-fixed
}

unsigned readMemory()
{
		// Return memory value addressed by lower 10 bits of the S register (1K) and the
		// bank decoder (which selects the 1K bank)
	unsigned lowAddress = register_S.readField(10,1);

	if(bankDecoder() == 0)
		return register_EMEM[lowAddress].read();

	unsigned highAddress = (bankDecoder()-1) << 10;
	return register_FMEM[highAddress | lowAddress].read();
}

void writeMemory(unsigned data)
{
		// Write into erasable memory addressed by lower 10 bits of the S register (1K) 
		// and the bank decoder (which selects the 1K bank)
	unsigned lowAddress = register_S.readField(10,1);

	if(bankDecoder() == 0)
	{
		register_EMEM[lowAddress].write(data);
		register_EMEM[lowAddress].clk(); // not a synchronous FF, so execute immediately
	}
}

unsigned readMemory(unsigned address)
{
		// Address is 14 bits. This function is used by the simulator for examining
		// memory; it is not part of the AGC design.
	unsigned lowAddress = address & 01777;
	unsigned bank = (address & 036000) >> 10;

	if(bank == 0)
		return register_EMEM[lowAddress].read();

	unsigned highAddress = (bank-1) << 10;
	return register_FMEM[highAddress | lowAddress].read();
}

void writeMemory(unsigned address, unsigned data)
{
		// Address is 14 bits. This function is used by the simulator for depositing into
		// memory; it is not part of the AGC design.
	//************************************************************
	// This function could also write the parity into memory
	//************************************************************
	unsigned lowAddress = address & 01777;
	unsigned bank = (address & 036000) >> 10;

	if(bank == 0)
	{
		register_EMEM[lowAddress].write(data);
		register_EMEM[lowAddress].clk();  // not a synchronous FF, so execute immediately
	}

	unsigned highAddress = (bank-1) << 10;
	register_FMEM[highAddress | lowAddress].write(data);
	register_FMEM[highAddress | lowAddress].clk();  // not a synchronous FF, so execute immediately
}

//-----------------------------------------------------------------------

unsigned gen1_15Parity(unsigned r)
{
	//check the lower 15 bits of 'r' and return the odd parity;
	//bit 16 is ignored.
	unsigned evenParity =
		(1&(r>>0))  ^ (1&(r>>1))  ^ (1&(r>>2))  ^ (1&(r>>3))  ^
		(1&(r>>4))  ^ (1&(r>>5))  ^ (1&(r>>6))  ^ (1&(r>>7))  ^
		(1&(r>>8))  ^ (1&(r>>9))  ^ (1&(r>>10)) ^ (1&(r>>11)) ^
		(1&(r>>12)) ^ (1&(r>>13)) ^ (1&(r>>14));
	return ~evenParity & 1; // odd parity
}

unsigned genP_15Parity(unsigned r)
{
	//check all 16 bits of 'r' and return the odd parity
	unsigned evenParity =
		(1&(r>>0))  ^ (1&(r>>1))  ^ (1&(r>>2))  ^ (1&(r>>3))  ^
		(1&(r>>4))  ^ (1&(r>>5))  ^ (1&(r>>6))  ^ (1&(r>>7))  ^
		(1&(r>>8))  ^ (1&(r>>9))  ^ (1&(r>>10)) ^ (1&(r>>11)) ^
		(1&(r>>12)) ^ (1&(r>>13)) ^ (1&(r>>14)) ^ (1&(r>>15));
	return ~evenParity & 1; // odd parity
}

//-----------------------------------------------------------------------

void regIn0::execRP()		{	if(isAsserted(RA4))
								{	
									// Sample the state of the inputs at the moment the
									// read pulse is asserted. In the H/W implementation,
									// register 0 is a buffer, not a latch.
									writeField(5,1,kbd);
									writeField(6,6,0); // actually should be keypressed strobe
									writeField(14,14,SA);
									clock();
									glbl_RWBUS|=read();
								}
							}
void regIn0::execWP()		{ }

void regIn1::execRP()		{	if(isAsserted(RA5))		glbl_RWBUS|=read();	}
void regIn1::execWP()		{ }

void regIn2::execRP()		{	if(isAsserted(RA6))		glbl_RWBUS|=read();	}
void regIn2::execWP()		{ }

void regIn3::execRP()		{	if(isAsserted(RA7))		glbl_RWBUS|=read();	}
void regIn3::execWP()		{ }



void regOut1::execRP()		{	if(isAsserted(RA11))	glbl_RWBUS|=read();	}
void regOut1::execWP()		{	if(isAsserted(GENRST))	write(0);
								if(isAsserted(WA11))	write(glbl_RWBUS);	}

void regOut2::execRP()		{	if(isAsserted(RA12))	glbl_RWBUS|=read();	}
void regOut2::execWP()		{	if(isAsserted(GENRST))	write(0);
								if(isAsserted(WA12))	write(glbl_RWBUS);	}

void regOut3::execRP()		{	if(isAsserted(RA13))	glbl_RWBUS|=read();	}
void regOut3::execWP()		{	if(isAsserted(WA13))	write(glbl_RWBUS);	}

void regOut4::execRP()		{	if(isAsserted(RA14))	glbl_RWBUS|=read();	}
void regOut4::execWP()		{	if(isAsserted(WA14))	write(glbl_RWBUS);	
								if(isAsserted(RP2) || 
									isAsserted(GP))		if(register_S.read()==014) 
															writeField(16,16,register_P2.read());
							}


void regG::assertP()	{	if(isAsserted(WG))		switch(register_S.read())
													{
														case 020:	assert(W20); break;
														case 021:	assert(W21); break;	
														case 022:	assert(W22); break;
														case 023:	assert(W23); break;
														default: if(register_S.read() >= 020) assert(WGn);
													}
							if(isAsserted(RSC))		switch(register_S.read())
													{
														case 00:	assert(RA0); break;
														case 01:	assert(RA1); break;
														case 02:	assert(RA2); break;
														case 03:	assert(RA3); break;
														case 04:	assert(RA4); break;
														case 05:	assert(RA5); break;
														case 06:	assert(RA6); break;
														case 07:	assert(RA7); break;
														case 010:	assert(RA10); break;
														case 011:	assert(RA11); break;
														case 012:	assert(RA12); break;
														case 013:	assert(RA13); break;
														case 014:	assert(RA14); break;
														default: break; // 015, 016, 017
													}
							if(isAsserted(WSC))		switch(register_S.read())
													{
														case 00:	assert(WA0); break;
														case 01:	assert(WA1); break;
														case 02:	assert(WA2); break;
														case 03:	assert(WA3); break;
														case 010:	assert(WA10); break;
														case 011:	assert(WA11); break;
														case 012:	assert(WA12); break;
														case 013:	assert(WA13); break;
														case 014:	assert(WA14); break;
														case 015:	assert(WBK); break;
														default: break; // 016, 017
													}

}
void regG::execRP()		{	if(isAsserted(RG))		if(register_S.read() >= 020) 
														glbl_RWBUS|=shiftData(glbl_RWBUS, read(), conv_RG); }
void regG::execWP()		{	if(isAsserted(CLG))		write(0); 

							if(isAsserted(RP2))		writeField(15, 15, register_P2.read());
							if(isAsserted(GP))		writeField(15, 15, gen1_15Parity(register_P.read()));
							if(isAsserted(WGn))		write(shiftData(read(), glbl_RWBUS, conv_WGn));

							// This is only used in PINC, MINC, and SHINC. Does not clear G
							// register; writes (ORs) into G from RWBus and writes into parity
							// from 1-15 generator. All done in one operation, although I show
							// it in two steps here. The sequence calls CLG in a previous TP.
							if(isAsserted(WGx))
							{
								write(read() | shiftData(read(), glbl_RWBUS, conv_WGn));
								writeField(15, 15, gen1_15Parity(register_P.read()));
							}

							if(isAsserted(W20))		write(shiftData(read(), glbl_RWBUS, conv_W20));
							if(isAsserted(W21))		write(shiftData(read(), glbl_RWBUS, conv_W21));
							if(isAsserted(W22))		write(shiftData(read(), glbl_RWBUS, conv_W22));
							if(isAsserted(W23))		write(shiftData(read(), glbl_RWBUS, conv_W23));
							if(isAsserted(SBEWG))	write(shiftData(read(), readMemory(), conv_SBEWG));
							if(isAsserted(SBFWG))	write(shiftData(read(), readMemory(), conv_SBFWG));
							if(isAsserted(WE))		writeMemory(shiftData(0, register_G.read(), conv_WE));
						}

void regS::execRP()		{ }
void regS::execWP()		{	if(isAsserted(WS))		write(glbl_RWBUS);	}

void regSQ::execRP()	{ }
void regSQ::execWP()	{	if(isAsserted(GENRST))	write(0);
							if(isAsserted(WSQ))		write(glbl_RWBUS >> 12); }

void regB::execRP()		{	if(isAsserted(RB))		glbl_RWBUS|=read();
							if(isAsserted(RC))		glbl_RWBUS|= outmask() & (~read());
							if(isAsserted(RB14))	glbl_RWBUS|=0020000; // ???
							if(isAsserted(R1))		glbl_RWBUS|=0000001; 
							if(isAsserted(R1C))		glbl_RWBUS|=0177776; 
							if(isAsserted(R2))		glbl_RWBUS|=0000002; 
							if(isAsserted(R22))		glbl_RWBUS|=0000022; 
							if(isAsserted(R24))		glbl_RWBUS|=0000024; 
							if(isAsserted(RSB))		glbl_RWBUS|=0100000; }
							// NOTE: for GOPROG, maybe should gate it into bus and assert WB
void regB::execWP()		{	if(isAsserted(GENRST))	write(GOPROG); // TC GOPROG
							if(isAsserted(WB))		write(glbl_RWBUS);  }


void regQ::execRP()		{	if(isAsserted(RQ) ||
							   isAsserted(RA1))		glbl_RWBUS|=read();	}
void regQ::execWP()		{	if(isAsserted(WQ) ||
							   isAsserted(WA1))		write(glbl_RWBUS);	}

void regZ::execRP()		{	if(isAsserted(RZ) ||
							   isAsserted(RA2))		glbl_RWBUS|=read();	}
void regZ::execWP()		{	if(isAsserted(WZ) ||
							   isAsserted(WA2))		write(glbl_RWBUS);	}

void regLP::execRP() 	{	if(isAsserted(RLP) ||
							   isAsserted(RA3))		glbl_RWBUS|=read();	}
void regLP::execWP()	{	if(isAsserted(WALP))	write(shiftData(read(), glbl_RWBUS, conv_WALP_LP));
							if(isAsserted(WLP) ||
							   isAsserted(WA3))		write(shiftData(read(), glbl_RWBUS, conv_WLP));
						}

void regA::execRP()		{	if(isAsserted(RA) ||
							   isAsserted(RA0))		glbl_RWBUS|=read();	}
void regA::execWP()		{	if(isAsserted(WA) ||
							   isAsserted(WA0))		write(glbl_RWBUS);
							if(isAsserted(WALP))	write(shiftData(read(), glbl_RWBUS, conv_WALP_A));
						}

void regCI::execRP()	{ }
void regCI::execWP()	{	if(isAsserted(CI))		writeField(1,1,1);	
							if(isAsserted(WY))		if(!isAsserted(CI)) writeField(1,1,0);	}

void regX::execRP()		{ }
void regX::execWP()		{	if(isAsserted(WY))		write(0); 
							if(isAsserted(WX))		write(read() | glbl_RWBUS); }

void regY::execRP()		{ }
void regY::execWP()		{	if(isAsserted(WY))		write(glbl_RWBUS);
							if(isAsserted(WYx))		write(read() | glbl_RWBUS); }

unsigned regU::read()	
{
	unsigned carry = 
		(outmask()+1) & (register_X.read() + register_Y.read()); // end-around carry
	if(carry || register_CI.read()) carry = 1; else carry = 0;
    return outmask() & (register_X.read() + register_Y.read() + carry);
}


void regU::execRP()		{	if(isAsserted(RU))		glbl_RWBUS|=read();	}
void regU::execWP()		{ }

void regBNK::execRP()	{ }
void regBNK::execWP()	{ 
							if(isAsserted(WBK))		write(shiftData(read(), glbl_RWBUS, conv_WBK));
						}

void regSTA::execRP()	{   }
void regSTA::execWP()	{	if(isAsserted(ST1))		writeField(1,1,1);	
							if(isAsserted(ST2))		writeField(2,2,1);
							if(isAsserted(TRSM))	if(register_S.read()==025) writeField(2,2,1);
							if(isAsserted(CLSTA))	writeField(2,1,0); }

void regSTB::execRP()	{ }
void regSTB::execWP()	{	if(isAsserted(WSTB))	write(register_STA.read()); 
							if(isAsserted(CLSTB))	writeField(2,1,0); 
							if(isAsserted(SETSTB))	writeField(2,1,1); }

void regBR1::execRP()	{ }
void regBR1::execWP()	{
					if(isAsserted(GENRST))	write(0);
					if(isAsserted(CLBR))	write(0);

						// Set Branch 1 FF
						//	if sign bit is '1' (negative sign)
						//	if negative overflow (sign==1; overflow==0)
					if(isAsserted(TSGN))	if(glbl_RWBUS & 0100000) write(1); else write(0);
					if(isAsserted(TOV))		if((glbl_RWBUS & 0140000) == 0100000) write(1); else write(0);
						} 

void regBR2::execRP()	{ }
void regBR2::execWP()	{
					if(isAsserted(GENRST))	write(0);
					if(isAsserted(CLBR))	write(0);

						// Set Branch 2 FF
						//	if positive overflow (sign==0; oveflow==1)
						//	if minus zero
						//	if sign bit is '1' (negative sign)
					if(isAsserted(TOV))		if((glbl_RWBUS & 0140000) == 0040000) write(1); else write(0);
					if(isAsserted(TMZ))		if(glbl_RWBUS == 0177777) write(1); else write(0);
					if(isAsserted(TSGN2))	if(glbl_RWBUS  & 0100000) write(1); else write(0);
						} 

void regSNI::execRP()	{ }
void regSNI::execWP()	{	if(isAsserted(NISQ))	writeField(1,1,1); // change to write(1)??
							if(isAsserted(CLISQ))	writeField(1,1,0); } // change to write(0)??

void regCTR::execRP()	{ }
void regCTR::execWP()	{	if(isAsserted(CTR))		write(read()+1);
							if(isAsserted(CLCTR))	write(0); }

//--------------------------------------------------------------------------------
// PRIORITY COUNTERS

// ****************************************************
// The interrupt priorities are stored in RPCELL as 1-5, but
// the priority counter priorities are stored as 0-19; this
// inconsistency should be fixed, probably. Also, the method
// of address determination for the priority counters needs work

unsigned pcUp[20];
unsigned pcDn[20];

enum ovfState { NO_OVF, POS_OVF, NEG_OVF };
ovfState testOverflow(unsigned bus)
{
	if((bus & 0100000) && !(bus & 0040000))
		return NEG_OVF; // negative overflow
	else if(!(bus & 0100000) && (bus & 0040000))
		return POS_OVF; // positive overflow
	else
		return NO_OVF;
}

void resetAllpc()
{
	for(int i=0; i<20; i++) { pcUp[i]=0; pcDn[i]=0; }
}

	// priority encoder; outputs (0-19 (decimal))
unsigned getPriority()
{
	for(int i=0; i<20; i++) { if(pcUp[i] ^ pcDn[i]) return i; }
	return 0;
}

unsigned getSubseq()
{
	unsigned pc = getPriority();
	if((pc == 5 && pcDn[pc]) || (pc == 18 && pcDn[pc]))
		return SHINCSEL;
	else if(pcUp[pc]) 
		return PINCSEL;
	else if(pcDn[pc]) 
		return MINCSEL;
	else 
		return NOPSEL;
}

							// Disable changes to counter inputs at TP12 and TP1.
							// Selected counter address is requested at TP1.
							// Counter address is latched at TP12
void regPSEQ::execRP()	{ }
void regPSEQ::execWP()	{	if(isAsserted(GENRST))	write(0);
							if(isAsserted(WPCTR))	write(getSubseq()); }
							// *************************************************************
							// NOTE: need a better way to get the priority address;
							// this method requires a parallel adder. Probably change the
							// address of the counters to start at 030.
void regPCELL::execRP()	{	if(isAsserted(RSCT))	glbl_RWBUS = 034 + read(); }
void regPCELL::execWP()	{	if(isAsserted(WPCTR))	write(getPriority()); 
													// Overflow from the selected counter appears
													// on the bus when this signal is asserted;
													// it could be used to trigger an interrupt
													// or routed to another counter
							if(isAsserted(WOVR))	pcUp[read()]=0; pcDn[read()]=0;
							if(isAsserted(WOVC))	switch(testOverflow(glbl_RWBUS))
													{
													case POS_OVF: pcUp[0]=1; break; // incr OVCTR (034)
													case NEG_OVF: pcDn[0]=1; break; // decr OVCTR (034)
													}
							}
//--------------------------------------------------------------------------------
// INTERRUPTS

// NOTE: the priority cells (rupt[]) are indexed 0-4, but stored in the 
// RPCELL register as 1-5; (0 in RPCELL means no interrupt)
unsigned rupt[5];

void resetAllRupt()
{
	for(int i=0; i<5; i++) { rupt[i]=0; }
}

	// interrupt vector; outputs 1-5 (decimal) == vector; 0 == no interrupt
unsigned getPriorityRupt()
{
	for(int i=0; i<5; i++) { if(rupt[i]) return i+1; }
	return 0;
}

	// latches the selected priority interrupt vector (1-5)
	// also inhibits additional interrupts while an interrupt is being processed
void regRPCELL::execRP()	{	if(isAsserted(RRPA))	glbl_RWBUS = 02000 + (read() << 2); }
void regRPCELL::execWP()	{	if(isAsserted(GENRST))	write(0);
								if(isAsserted(RPT))		write(getPriorityRupt()); 
								if(isAsserted(KRPT))	rupt[read()-1] = 0; 
								if(isAsserted(CLRP))	write(0); }

	// inhibits interrupts for 1 instruction (on WOVI)
void regINHINT1::execRP()	{ }
void regINHINT1::execWP()	{	if(isAsserted(WOVI))	if(testOverflow(glbl_RWBUS) != NO_OVF) write(1);
								if(isAsserted(CLINH1))	write(0); }

	// inhibits interrupts on INHINT, reenables on RELINT
void regINHINT::execRP()	{ }
void regINHINT::execWP()	{	if(isAsserted(GENRST))	write(1);
								if(isAsserted(INH))		write(1); 
								if(isAsserted(CLINH))	write(0); }
//--------------------------------------------------------------------------------
// PARITY GENERATION

	// NOTE: WP clears register_P before writing into it. Strictly speaking, WPx isn't
	// supposed to clear the register (should OR into the register), but in the counter
	// sequences where WPx is used, register_P is always cleared in the previous TP by
	// asserting WP with default zeroes on the write bus.
void regP::execRP()	{ }
void regP::execWP()	{	if(isAsserted(WP) || isAsserted(WPx))	// see NOTE above
						{
								// set all bits except parity bit
							write(shiftData(read(), glbl_RWBUS, conv_WP));
								// now set parity bit; in the actual AGC, this is
								// a single operation.
							if(isAsserted(RG))
								writeField(16, 16, register_G.readField(15,15));
							else
								writeField(16, 16, 0); // clear parity bit
						}
					}
void regP2::execRP() { }
void regP2::execWP() {	if(isAsserted(WP2))	register_P2.write(gen1_15Parity(register_P.read())); }

//--------------------------------------------------------------------------------
// DSKY keyboard

void keypress(keyInType c)
{
		// latch the keycode
	kbd = c;
		// generate KEYRUPT interrupt
	rupt[KEYRUPT] = 1;
}

//--------------------------------------------------------------------------------
// DSKY display

char MD1, MD2;	// major mode display
char VD1, VD2;	// verb display
char ND1, ND2;	// noun display
char R1S, R1D1, R1D2, R1D3, R1D4, R1D5;	// R1
char R2S, R2D1, R2D2, R2D3, R2D4, R2D5;	// R2
char R3S, R3D1, R3D2, R3D3, R3D4, R3D5;	// R3

// These flags control the sign; if both bits are 0 or 1, there is no sign.
// Otherwise, the sign is set by the selected bit.
unsigned R1SP, R1SM;
unsigned R2SP, R2SM;
unsigned R3SP, R3SM;

void clearOut0()
{
	MD1 = MD2 = ' ';	// major mode display
	VD1 = VD2 = ' ';	// verb display
	ND1 = ND2 = ' ';	// noun display
	R1S = R1D1 = R1D2 = R1D3 = R1D4 = R1D5 = ' ';	// R1
	R2S = R2D1 = R2D2 = R2D3 = R2D4 = R2D5 = ' ';	// R2
	R3S = R3D1 = R3D2 = R3D3 = R3D4 = R3D5 = ' ';	// R3

	R1SP = R1SM = 0;
	R2SP = R2SM = 0;
	R3SP = R3SM = 0;
}

char signConv(unsigned p, unsigned m)
{
	if(p && !m)
		return '+';
	else if(m && !p)
		return '-';
	else
		return ' ';
}

char outConv(unsigned in)
{
	switch(in)
	{
	case 000:	return ' ';
	case 025:	return '1';
	case 003:	return '2';
	case 033:	return '3';
	case 017:	return '4';
	case 036:	return '5';
	case 034:	return '6';
	case 023:	return '7';
	case 035:	return '8';
	case 037:	return '9';
	}
	return ' '; // error
}

void decodeRelayWord(unsigned in)
{
	unsigned charSelect	= (in & 074000) >> 11;	// get bits 15-12
	unsigned b11		= (in & 02000) >> 10;	// get bit 11
	unsigned bHigh		= (in & 01740) >> 5;	// get bits 10-6
	unsigned bLow		= in & 037;

	switch(charSelect)
	{
	case 013:	MD1 = outConv(bHigh);	MD2 = outConv(bLow); break;
	case 012:	VD1 = outConv(bHigh);	VD2 = outConv(bLow); break;		// FLASH not implemented
	case 011:	ND1 = outConv(bHigh);	ND2 = outConv(bLow); break;
	case 010:							R1D1 = outConv(bLow); break;	// UPACT not implemented

	case 007:	R1SP = b11; R1S = signConv(R1SP, R1SM);
				R1D2 = outConv(bHigh);	R1D3 = outConv(bLow); break;

	case 006:	R1SM = b11; R1S = signConv(R1SP, R1SM);
				R1D4 = outConv(bHigh);	R1D5 = outConv(bLow); break;

	case 005:	R2SP = b11; R2S = signConv(R2SP, R2SM);
				R2D1 = outConv(bHigh);	R2D2 = outConv(bLow); break;

	case 004:	R2SM = b11; R2S = signConv(R2SP, R2SM);
				R2D3 = outConv(bHigh);	R2D4 = outConv(bLow); break;

	case 003:	R2D5 = outConv(bHigh);	R3D1 = outConv(bLow); break;

	case 002:	R3SP = b11; R3S = signConv(R3SP, R3SM);
				R3D2 = outConv(bHigh);	R3D3 = outConv(bLow); break;

	case 001:	R3SM = b11; R3S = signConv(R3SP, R3SM);
				R3D4 = outConv(bHigh);	R3D5 = outConv(bLow); break;
	}
}

// Writing to OUT0 loads OUT0 and the selected display register interfaced to OUT0.
// Reading from OUT0 returns the current contents of OUT0--not the selected display register.

void regOut0::execRP()		{	if(isAsserted(RA10))	glbl_RWBUS|=read();	}
void regOut0::execWP()		{	if(isAsserted(GENRST))	
									{ write(0); clearOut0(); }
								if(isAsserted(WA10))	
									{ write(glbl_RWBUS); decodeRelayWord(glbl_RWBUS); }
							}

//--------------------------------------------------------------------------------
// START/STOP LOGIC and TIME PULSE GENERATOR

void getNextState()
{
	unsigned mystate = register_SG.read();
	if(PURST) 
		mystate = STBY;
	else
	switch(mystate)
	{
	case STBY:	if(!SWRST && !PURST && ((CLKRATE != FCLK) || F17)) mystate = PWRON; break;
	case PWRON:	if(((CLKRATE != FCLK) || F13)) mystate = TP1; break;

	case TP1:	mystate = TP2;	break;
	case TP2:	mystate = TP3;	break;
	case TP3:	mystate = TP4;	break;
	case TP4:	mystate = TP5;	break;
	case TP5:	mystate = TP6;	break;
	case TP6:	mystate = TP7;	break;
	case TP7:	mystate = TP8;	break;
	case TP8:	mystate = TP9;	break;
	case TP9:	mystate = TP10;	break;
	case TP10:	mystate = TP11;	break;
	case TP11:	mystate = TP12;	break;
	case TP12:
		if(!BREQ && register_SNI.read() && register_OUT1.readField(8,8) && SA)
			mystate = STBY;
			// the next transition to TP1 is incompletely decoded; it works because
			// the transition to STBY has already been tested.
		else if((RUN && !BREQ) || (!register_SNI.read() && INST))
			mystate = TP1;
		else
			mystate = SRLSE;
		break;
	case SRLSE:	if(!STEP) mystate = WAIT; break;
	case WAIT:	
		if(STEP || (RUN && !BREQ)) 
			mystate = TP1; 
		else if(SWRST) 
			mystate = STBY; 
		break;
	//case F17RST: if(!F17 || (CLKRATE != FCLK)) mystate = STBY; break;
	default:		break;
	}
	register_SG.write(mystate);
}
void regSG::execRP() { }
void regSG::execWP() { 	getNextState(); }

//--------------------------------------------------------------------------------

	// A container for all registers. This is kept so we can iterate through
	// all registers to execute the control pulses. For simulation purposes
	// only; this has no counterpart in the hardware AGC.
reg* registerList[] = // registers are in no particular sequence
{
	&register_IN0, &register_IN1, &register_IN2, &register_IN3,
	&register_OUT0, &register_OUT1, &register_OUT2, &register_OUT3, &register_OUT4,
	&register_G, &register_S, &register_BNK, &register_SQ, &register_B,
	&register_Q, &register_Z, &register_LP, &register_A, &register_X,
	&register_Y, &register_U, &register_STA, &register_STB, &register_SNI,
	&register_LOOPCTR, &register_CI, &register_BR1, &register_BR2, 
	&register_PSEQ, &register_PCELL, 
	&register_RPCELL, &register_INHINT1, &register_INHINT,
	&register_P, &register_P2,
	&register_SG,
	0 // zero is end-of-list flag
};


//-----------------------------------------------------------------------
// CONTROL LOGIC

subseq instructionSubsequenceDecoder(pCntrType pc, instruction i, scType s)
{
		// Combinational logic decodes instruction and the stage count 
		// to get the instruction subsequence.
	static subseq decode[16][4] = {
		{	TC0,		RUPT1,		STD2,		RUPT3	}, // 00
		{	CCS0,		CCS1,		NO_SEQ,		NO_SEQ	}, // 01
		{	NDX0,		NDX1,		NO_SEQ,		RSM3	}, // 02
		{	XCH0,		NO_SEQ,		STD2,		NO_SEQ	}, // 03

		{	NO_SEQ,		NO_SEQ,		NO_SEQ,		NO_SEQ	}, // 04
		{	NO_SEQ,		NO_SEQ,		NO_SEQ,		NO_SEQ	}, // 05
		{	NO_SEQ,		NO_SEQ,		NO_SEQ,		NO_SEQ	}, // 06
		{	NO_SEQ,		NO_SEQ,		NO_SEQ,		NO_SEQ	}, // 07
		{	NO_SEQ,		NO_SEQ,		NO_SEQ,		NO_SEQ	}, // 10

		{	MP0,		MP1,		NO_SEQ,		MP3		}, // 11
		{	DV0,		DV1,		STD2,		NO_SEQ	}, // 12
		{	SU0,		NO_SEQ,		STD2,		NO_SEQ	}, // 13

		{	CS0,		NO_SEQ,		STD2,		NO_SEQ	}, // 14
		{	TS0,		NO_SEQ,		STD2,		NO_SEQ	}, // 15
		{	AD0,		NO_SEQ,		STD2,		NO_SEQ	}, // 16
		{	MASK0,		NO_SEQ,		STD2,		NO_SEQ	}  // 17

	};

	switch(pc)
	{
	case PINCSEL:	return PINC0;
	case MINCSEL:	return MINC0;
	case SHINCSEL:	return SHINC0;
	default: return decode[i][s];
	}
}

void controlPulseMatrix(tpType t, subseq s, unsigned b1, unsigned b2, unsigned sni)
{
		// Combination logic decodes time pulse, subsequence, branch register, and
		// "select next instruction" latch to get control pulses associated with 
		// those states.

		// Get rid of any old control pulses.
	clearControlPulses();

	if(PURST)
	{
		// PURST should do an asynchronous clear of register SG to put it in the
		// STBY state; this will trigger a GENRST that initializes everything else.
		register_SG.write(STBY); // initial state of time pulse generator
	}

	static subsequence* subsp[] = 
	{
		&SUB_TC0,	&SUB_CCS0,	&SUB_CCS1,	&SUB_NDX0,	&SUB_NDX1,	&SUB_RSM3,	
		&SUB_XCH0,	&SUB_CS0,	&SUB_TS0,	&SUB_AD0,	&SUB_MASK0,	&SUB_MP0,	
		&SUB_MP1,	&SUB_MP3,	&SUB_DV0,	&SUB_DV1,	&SUB_SU0,	&SUB_RUPT1,	
		&SUB_RUPT3,	&SUB_STD2,	&SUB_PINC,	&SUB_MINC,	&SUB_SHINC,0
	};

		// Get control pulses for the current instruction subsequence.
	if(s != NO_SEQ && t >= TP1 && t <= TP11)
	{
		subsequence* subseqP = subsp[s];
		if(subseqP)
		{
			    // index t-2 because TP1=2, but array is indexed from zero
			controlStep& csref = subseqP->tp[t-2];

			brType b = (brType) ((b1 << 1) | b2);
			controlSubStep& cssref = csref.substep[b];
			if(cssref.br == NO_BR)
				cssref = csref.substep[0];

			cpType* p = cssref.pulse;
			assert(p);
		}
	}

		// Add any additional control pulses.
	if(register_LOOPCTR.read() == 6)
	{
		assert(ST2);	// STA <- 2
		assert(CLCTR);	// CTR <- 0
	}

	switch(register_SG.read())
	{
	case STBY:
		assert(GENRST);
		// inhibit all alarms
		resetAllpc(); // clear priority cells???
		resetAllRupt(); // clear interrupt requests
		// init "SQ" complex
		//  clear branch registers
		//  stage registers are not cleared; should they be?
		//  TC GOPROG copied to B
		break;
	case PWRON:	break;
	case TP1:	break;
	case TP2:	break;
	case TP3:	break;
	case TP4:	break;
	case TP5:	
		// EMEM must be available in G register by TP6
		if(	register_S.read() > 017		&&	// not a central register
			register_S.read() < 02000	&&	// not fixed memory
			glbl_subseq != DV1			&&	// not a loop counter subseq
			glbl_subseq != MP1)
		{
			assert(SBEWG);
		}
		if( register_S.read() == 017 ) assert (INH);	// INHINT (INDEX 017)
		if( register_S.read() == 016 ) assert (CLINH);	// RELINT (INDEX 016)
		break;
	case TP6:
		// FMEM must be available in G register by TP7
		if(	register_S.read() >= 02000	&&	// not eraseable memory
			glbl_subseq != DV1			&&	// not a loop counter subseq
			glbl_subseq != MP1)
		{
			assert(SBFWG);
		}
		break;
	case TP7:	break;
	case TP8:	break;
	case TP9:	break;
	case TP10:
		break;

	case TP11:	
		// G register written to memory beginning at TP11; Memory updates are in
		// G by TP10 for all normal and extracode instructions, but the PINC, MINC,
		// and SHINC sequences write to G in TP10 because they need to update the
		// parity bit.
		if(	register_S.read() > 017		&&	// not a central register
			register_S.read() < 02000	&&	// not fixed memory
			glbl_subseq != DV1			&&	// not a loop counter subseq
			glbl_subseq != MP1)
		{
			assert(WE);
		}
		// Additional interrupts are inhibited during servicing of an interrupt;
		// Remove the inhibition when RESUME is executed (INDEX 025)
		if(glbl_subseq==RSM3) assert(CLRP);
		break;

	case TP12:
		// DISABLE INPUT CHANGE TO PRIORITY COUNTER (reenable after TP1)
		// Check the priority counters; service any waiting inputs on the next
		// memory cycle.
		assert(WPCTR);
		if(register_SNI.read() == 1) // if SNI is set, get next instruction
		{
			if(	getPriorityRupt()				// if interrupt requested
				&& register_RPCELL.read() == 0	// and interrupt not currently being serviced
				&& register_INHINT1.read() == 0	// and interrupt not inhibited for 1 instruction
				&& register_INHINT.read() == 0)	// and interrupts enabled (RELINT)
			{
				// Interrupt: SQ <- 0 (the default RW bus state)
				assert(RPT);		// latch interrupt vector
				assert(SETSTB);		// STB <- 1
			}
			else
			{
				// Normal instruction
				assert(RB);			// SQ <- B
				assert(CLSTB);		// STB <- 0
			}
			assert(WSQ);
			assert(CLSTA);		// STA <- 0

		}
		else if(register_PSEQ.read() == NOPSEL) // if previous sequence was not a counter
		{
			// get next sequence for same instruction.
			assert(WSTB);		// STB <- STA
			assert(CLSTA);		// STA <- 0

		}
		assert(CLISQ);	// SNI <- 0

			// Remove inhibition of interrupts (if they were) AFTER the next instruction
		assert(CLINH1); // INHINT1 <- 0
		break;
	case SRLSE:		break;
	case WAIT:
		//if(BREQ) assert(BGRANT); // BGRANT IS NOT DEFINED YET
		break;
	//case F17RST:	break;
	default: ;
	}
}

void genAGCStates()
{
		// 1) Decode the current instruction subsequence (glbl_subseq).
	glbl_subseq = instructionSubsequenceDecoder(
		(pCntrType) register_PSEQ.read(),
		(instruction) register_SQ.read(), (scType) register_STB.read());

		// 2) Build a list of control pulses for this state.
	controlPulseMatrix((tpType) register_SG.read(), 
		glbl_subseq, 
		register_BR1.read(), register_BR2.read(), register_SNI.read());

		// 3) Execute the control pulses for this state. In the real AGC, these occur
		// simultaneously. Since we can't achieve that here, we break it down into the
		// following steps:
		// Most operations involve data transfers--usually reading data from
		// a register onto a bus and then writing that data into another register. To
		// approximate this, we first iterate through all registers to perform
		// the 'read' operation--this transfers data from register to bus.
		// Then we again iterate through the registers to do 'write' operations,
		// which move data from the bus back into the register.

	glbl_RWBUS = 0; // clear bus; necessary because words are logical OR'ed onto the bus.

		// First, assert any additional control pulses derived from other pulses.
	unsigned i;
	for(i=0; registerList[i]; i++)
	{
		registerList[i]->assertP();
	}
		// Now start executing the pulses: 
		// First, read register outputs onto the bus or anywhere else.
	for(i=0; registerList[i]; i++)
	{
		registerList[i]->execRP();
	}
		// Now, write the bus and any other signals into the register inputs.
	for(i=0; registerList[i]; i++)
	{
		registerList[i]->execWP();
	}

}

void clkAGC()
{
		// Now that all the inputs are set up, clock the registers so the outputs
		// can change state in accordance with the inputs.
	for(int i=0; registerList[i]; i++)
	{
		registerList[i]->clk();
	}
}

//-----------------------------------------------------------------------
// SIMULATION LOGIC

char* getCommand(char* prompt)
{
	static char s[80];
	char* sp = s;

	cout << prompt; cout.flush();

	char key;
	while((key = _getch()) != 13)
	{
		if(isprint(key))
		{
			cout << key; cout.flush();
			*sp = key; sp++;
		}
		else if(key == 8 && sp != s)
		{
			cout << key << " " << key; cout.flush();
			sp--;
		}
	}
	*sp = '\0';
	return s;
}

void incrCntr()
{
	char cntrname[80];
	strcpy(cntrname, getCommand("Increment counter: -- enter pcell (0-19): "));
	cout << endl;

	int pc = atoi(cntrname);
	pcUp[pc] = 1;
}

void decrCntr()
{
	char cntrname[80];
	strcpy(cntrname, getCommand("Decrement counter: -- enter pcell (0-19): "));
	cout << endl;

	int pc = atoi(cntrname);
	pcDn[pc] = 1;
}

void interrupt()
{
	char iname[80];
	strcpy(iname, getCommand("Interrupt: -- enter priority (1-5): "));
	cout << endl;

	int i = atoi(iname) - 1;
	rupt[i] = 1;
}

	// Load AGC memory from the specified file.
void loadMemory()
{
	char filename[80];
	strcpy(filename, getCommand("Load Memory -- enter filename: "));
	cout << endl;

		// Valid object files will have a .obj extension.
	char* p = filename;
	while(*p != '\0') { p++; if(*p == '.') break; }
	if(strcmp(p,".obj") != 0)
	{
		cerr << "*** ERROR: Object file not *.obj" << endl;
		return;
	}

	FILE* fp = fopen(filename, "r");
	if(!fp)
	{
		perror("fopen failed:");
		cout << "*** ERROR: Can't load memory for file: " << filename << endl;
		return;
	}
	unsigned addr;
	unsigned data;
	while(fscanf(fp, "%o %o", &addr, &data) != EOF)
	{
		writeMemory(addr, data);
	}
	fclose(fp);
	cout << "Memory loaded." << endl;
}

	// Write the entire contents of fixed and
	// eraseable memory to the specified file.
	// Does not write the registers
void saveMemory(char* filename)
{
	FILE* fp = fopen(filename, "w");
	if(!fp)
	{
		perror("*** ERROR: fopen failed:");
		exit(-1);
	}
	char buf[100];
	for(unsigned addr=020; addr<=031777; addr++)
	{
		sprintf(buf, "%06o %06o\n", addr, readMemory(addr));
		fputs(buf, fp);
	}
	fclose(fp);
}

void examineMemory()
{
	char theAddress[20];
	strcpy(theAddress, getCommand("Examine Memory -- enter address (octal): "));
	cout << endl;

	unsigned address = strtol(theAddress, 0, 8);

	char buf[100];
	for(unsigned i=address; i<address+6; i++)
	{
		sprintf(buf, "%06o:  %06o", i, readMemory(i));
		cout << buf << endl;
	}
}

	// Returns true if time (s) elapsed since last time it returned true; does not block
	// search for "Time Management"
bool checkElapsedTime(time_t s)
{
	if(!s) return true;

	static clock_t start = clock();
	clock_t finish = clock();
	double duration = (double)(finish - start) / CLOCKS_PER_SEC;
	if(duration >= s)
	{
	    start = finish;
		return true;
	}
	return false;
}

	// Blocks until time (s) has elapsed.
void delay(time_t s)
{
	if(!s) return;

	clock_t start = clock();
	clock_t finish = 0;
	double duration = 0;

	do
	{
		finish = clock();
	}
	while((duration = (double)(finish - start) / CLOCKS_PER_SEC) < s);
}

void displayAGC()
{

	char buf[100];
	cout << "AGC4 SIMULATOR -------------------------------" << endl;
	sprintf(buf," TP: %-5s  F17:%1d  F13:%1d", 
		tpTypestring[register_SG.read()], F17, F13);
    cout << buf << endl;   

	sprintf(buf, "    STA:%01o   STB:%01o   BR1:%01o   BR2:%01o   SNI:%01o   CI:%01o   LOOPCTR:%01o", 
		register_STA.read(), register_STB.read(), register_BR1.read(), register_BR2.read(),
		register_SNI.read(), register_CI.read(), register_LOOPCTR.read());
	cout << buf << endl;

	sprintf(buf, " RPCELL:%05o  INH1:%01o  INH:%01o  PSEQ:%01o  PCELL:%02o  SQ:%02o  %-6s  %-6s", 
		register_RPCELL.read(), register_INHINT1.read(), register_INHINT.read(),
		register_PSEQ.read(), register_PCELL.read(), 
		register_SQ.read(), instructionString[register_SQ.read()], subseqString[glbl_subseq]);
	cout << buf << endl;

	sprintf(buf, "     CP:%s", getControlPulses());
	cout << buf << endl;

	sprintf(buf, "      S:  %04o   G:%06o   P:%06o   (r)RUN :%1d  (p)PURST:%1d (F2,3,4)CLK:%s", 
		register_S.read(), register_G.read(), register_P.read(),
		RUN, PURST, clkTypestring[CLKRATE]);
	cout << buf << endl;

	sprintf(buf, "  RWBUS:%06o             P2:%01o        (s)STEP:%1d  (w)SWRST:%1d", 
		glbl_RWBUS & 0177777, register_P2.read(), STEP, SWRST);
	cout << buf << endl;

	sprintf(buf, "      B:%06o                         (n)INST:%1d  (c)BREQ :%1d", 
		register_B.read(), INST, BREQ);
	cout << buf << endl;

	sprintf(buf, "      X:%06o   Y:%06o   U:%06o   (a)SA  :%1d", 
		register_X.read(), register_Y.read(), register_U.read(), SA);
	cout << buf << endl;

	cout << endl;
	sprintf(buf, "00    A:%06o   15   BANK:%02o       36  TIME2:%06o   53   OPT Y:%06o", 
		register_A.read(), register_BNK.read(), readMemory(036), readMemory(053));
	cout << buf << endl;
	sprintf(buf, "01    Q:%06o   16 RELINT:%6s   37  TIME3:%06o   54  TRKR X:%06o", 
		register_Q.read(),"", readMemory(037), readMemory(054));
	cout << buf << endl;
	sprintf(buf, "02    Z:%06o   17 INHINT:%6s   40  TIME4:%06o   55  TRKR Y:%06o", 
		register_Z.read(),"", readMemory(040), readMemory(055));
	cout << buf << endl;
	sprintf(buf, "03   LP:%06o   20    CYR:%06o   41 UPLINK:%06o   56  TRKR Z:%06o", 
		register_LP.read(), readMemory(020), readMemory(041), readMemory(056));
	cout << buf << endl;

	sprintf(buf, "04  IN0:%06o   21     SR:%06o   42 OUTCR1:%06o", 
		register_IN0.read(), readMemory(021), readMemory(042));
	cout << buf << endl;
	sprintf(buf, "05  IN1:%06o   22    CYL:%06o   43 OUTCR2:%06o    DSKY: [%c%c]", 
		register_IN1.read(), readMemory(022), readMemory(043),
		' ', ' ');
	cout << buf << endl;
	sprintf(buf, "06  IN2:%06o   23     SL:%06o   44 PIPA X:%06o", 
		register_IN2.read(), readMemory(023), readMemory(044));
	cout << buf << endl;
	sprintf(buf, "07  IN3:%06o   24  ZRUPT:%06o   45 PIPA Y:%06o     A:[%c%c] M:[%c%c]", 
		register_IN3.read(), readMemory(024), readMemory(045),
		' ', ' ', MD1, MD2);
	cout << buf << endl;
	sprintf(buf, "10 OUT0:%06o   25  BRUPT:%06o   46 PIPA Z:%06o     V:[%c%c] N:[%c%c]", 
		register_OUT0.read(), readMemory(025), readMemory(046),
		VD1, VD2, ND1, ND2);
	cout << buf << endl;
	sprintf(buf, "11 OUT1:%06o   26  ARUPT:%06o   47  CDU X:%06o    R1:[  %c%c%c%c%c%c ]", 
		register_OUT1.read(), readMemory(026), readMemory(047),
		R1S, R1D1, R1D2, R1D3, R1D4, R1D5);
	cout << buf << endl;
	sprintf(buf, "12 OUT2:%06o   27  QRUPT:%06o   50  CDU Y:%06o    R2:[  %c%c%c%c%c%c ]", 
		register_OUT2.read(), readMemory(027), readMemory(050),
		R2S, R2D1, R2D2, R2D3, R2D4, R2D5);
	cout << buf << endl;
	sprintf(buf, "13 OUT3:%06o   34  OVCTR:%06o   51  CDU Z:%06o    R3:[  %c%c%c%c%c%c ]", 
		register_OUT3.read(), readMemory(034), readMemory(051),
		R3S, R3D1, R3D2, R3D3, R3D4, R3D5);
	cout << buf << endl;
	sprintf(buf, "14 OUT4:%06o   35  TIME1:%06o   52  OPT X:%06o", 
		register_OUT4.read(), readMemory(035), readMemory(052));
	cout << buf << endl;
}

void updateAGCDisplay()
{
	static bool displayTimeout = false;
	static int clockCounter = 0;

	if(checkElapsedTime(2)) displayTimeout = true;
	if(CLKRATE == FCLK)
	{
		//if(RUN)
		//{
			// update every 2 seconds at the start of a new instruction
			if(displayTimeout)
			{
				clockCounter++;
				if((register_SG.read() == TP12 && register_SNI.read() == 1) || clockCounter > 500)
				{
				displayAGC();
				displayTimeout = false;
				clockCounter = 0;
				}
			}
		//}
#ifdef NOTDEF
		else // (!RUN)
		{
			// update upon entry to WAIT state
			static bool waitDisplayed = false;
			if(register_SG.read() == WAIT)
			{
				if(!waitDisplayed)
				{
				    displayAGC();
				    waitDisplayed = true;
				}
			}
			else
				waitDisplayed = false;
		}
#endif
	}
	else
		displayAGC(); // When the clock is manual or slow, always update.

}

void showMenu()
{
	cout << "AGC4 EMULATOR MENU:" << endl;
	cout << " 'r' = RUN:  toggle RUN/HALT switch upward to the RUN position." << endl;
}

void main(int argc, char* argv[])
{
//	register_G.write(040000);
//	register_G.clk();
//	cout << "register G before: " << oct << register_G.read() << dec << endl;

//	register_G.writeField(15,15,0);
//	register_G.clk();
//	cout << "register G after: " << oct << register_G.read() << dec << endl;

	bool singleClock = false;


	genAGCStates();
	displayAGC();

	while(1)
	{
		// NOTE: assumes that the display is always pointing to the start of
		// a new line at the top of this loop!

	    // Clock the AGC, but between clocks, poll the keyboard
	    // for front-panel input by the user. This uses a Microsoft function;
    	// substitute some other non-blocking function to access the keyboard
    	// if you're porting this to a different platform.

		cout << "> "; cout.flush();  // display prompt

		while( !_kbhit() )
		{
			if((CLKRATE != MCLK) || singleClock)
			{
				clkAGC(); singleClock = false;
				genAGCStates();

				updateAGCDisplay();

				if(CLKRATE == SCLK)
					delay(1);
			}
			// for convenience, clear the single step switch on TP1; in the
			// hardware AGC, this happens when the switch is released
			if(STEP && register_SG.read() == TP1) STEP = 0;
		}
        char key = _getch();

	    // Keyboard controls for front-panel:
		switch(key)
		{
			// AGC controls
            // simulator controls

		case 'q': cout << "QUIT..." << endl; exit(0);
		case 'm': showMenu(); break;

		case 'd': genAGCStates(); displayAGC(); break; // update display

		case 'l': loadMemory(); break;
		case 'e': examineMemory(); break;



		case ']': 
			incrCntr(); 
			//genAGCStates();
			//displayAGC(EVERY_CYCLE);
			break;

		case '[': 
			decrCntr(); 
			//genAGCStates();
			//displayAGC(EVERY_CYCLE);
			break;

		case 'i': 
			interrupt(); 
			//genAGCStates();
			//displayAGC(EVERY_CYCLE);
			break;

		case 'z':
			F17 = (F17 + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;

		case 'x':
			F13 = (F13 + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;

		case 'r': 
			RUN = (RUN + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;	

		case 's': 
			STEP = (STEP + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;	

		case 'a': 
			SA = (SA + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;
			
		case 'n': 
			INST = (INST + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;	

		case 'c': 
			BREQ = (BREQ + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;

		case 'p': 
			PURST = (PURST + 1) % 2;
			genAGCStates();
			displayAGC();
			break;	

		case 'w': 
			SWRST = (SWRST + 1) % 2; 
			genAGCStates();
			displayAGC();
			break;	

		// DSKY:
		case '0': keypress(KEYIN_0); break;
		case '1': keypress(KEYIN_1); break;
		case '2': keypress(KEYIN_2); break;
		case '3': keypress(KEYIN_3); break;
		case '4': keypress(KEYIN_4); break;
		case '5': keypress(KEYIN_5); break;
		case '6': keypress(KEYIN_6); break;
		case '7': keypress(KEYIN_7); break;
		case '8': keypress(KEYIN_8); break;
		case '9': keypress(KEYIN_9); break;
		case '+': keypress(KEYIN_PLUS); break;
		case '-': keypress(KEYIN_MINUS); break;
		case '.': keypress(KEYIN_CLEAR); break;
		case '/': keypress(KEYIN_VERB); break;
		case '*': keypress(KEYIN_NOUN); break;
		case 'g': keypress(KEYIN_KEY_RELEASE); break;
		case 'h': keypress(KEYIN_ERROR_RESET); break;
		case 'j': keypress(KEYIN_ENTER); break;

		case '\0': // must be a function key
			key = _getch();
			switch(key)
			{
			case 0x3b: // F1: single clock pulse (when system clock off)
				singleClock = true; break;
			case 0x3c: // F2: manual clocking (MCLK)
				CLKRATE = MCLK; genAGCStates(); displayAGC(); break;
			case 0x3d: // F3: slow clock (SCLK)
				CLKRATE = SCLK; genAGCStates(); break;
		    case 0x3e: // F4: fast clock (FCLK)
				CLKRATE = FCLK; genAGCStates(); displayAGC(); break;
			default: cout << "function key: " << key << "=" << hex << (int) key << dec << endl;
			}
			break;

		//default: cout << "??" << endl;
		default: cout << key << "=" << hex << (int) key << dec << endl;
		}
    }
}

//***********************************************************************
/*
Things to do:

Implement TP (test parity)
Implement BGRANT signal

Implement alarms
Implement flash for DSKY

Known bug: TP9 of XCH doesn't work properly for setting the parity bit. RP2 executes
first and sets the parity bit in the masterVal side of the G register properly. But then,
WGn executes and copies the old parity value from the slaveVal side of the G register
back over into the masterVal side, wiping out the RP2 value.

Issue: check all the FFs and registers that need to be cleared during GENRST.


On kbd command polling, consider adding genAGCStates and displayAGC for counter inputs
and interrupts, too. Need to check this out, so we don't break it.

Change TP pulses to tpType (??)

Convert interrupt and counter cells to register objects so they can reset on GENRST
during WP.
*/