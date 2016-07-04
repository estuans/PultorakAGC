/****************************************************************************
 *  SEQ - SEQUENCE GENERATOR subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       SEQ.h
 *
 *  VERSIONS:
 * 
 *  DESCRIPTION:
 *    Sequence Generator for the Block 1 Apollo Guidance Computer prototype (AGC4).
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
#ifndef SEQ_H
#define SEQ_H

#include "reg.h"


#define MAXPULSES 15
#define MAX_IPULSES 5 // no more than 5 instruction-generated pulses active at any time


enum cpType { // **inferred; not defined in orignal R393 AGC4 spec.
	NO_PULSE,
	CI,		// Carry in
	CLG,	// Clear G
	CLINH,	// Clear INHINT**
	CLINH1,	// Clear INHINT1**
	CLISQ,	// Clear SNI**
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
	RBK,	// Read BNK
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
	R2000,	// Read 2000 **
	SBWG,	// Write G from memory
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


#ifdef NOTDEF
	//*************************************************************
	// EXTERNAL OUTPUTS FROM SUBSYSTEM A
	//
	CI,		// Carry in
	CLCTR,	// Clear loop counter**
	CLG,	// Clear G
	CTR,	// Loop counter
	GP,		// Generate Parity
	KRPT,	// Knock down Rupt priority
	NISQ,	// New instruction to the SQ register
	RA,		// Read A
	RB,		// Read B
	RB14,	// Read bit 14
	RC,		// Read C
	RG,		// Read G
	RLP,	// Read LP
	RP2,	// Read parity 2
	RQ,		// Read Q
	RRPA,	// Read RUPT address
	RSB,	// Read sign bit
	RSCT,	// Read selected counter address
	RU,		// Read sum
	RZ,		// Read Z
	R1,		// Read 1
	R1C,	// Read 1 complimented
	R2,		// Read 2
	R22,	// Read 22
	R24,	// Read 24
	ST1,	// Stage 1
	ST2,	// Stage 2
	TMZ,	// Test for minus zero
	TOV,	// Test for overflow
	TP,		// Test parity
	TRSM,	// Test for resume
	TSGN,	// Test sign
	TSGN2,	// Test sign 2
	WA,		// Write A
	WALP,	// Write A and LP
	WB,		// Write B
	WGx,	// Write G (do not reset)
	WLP,	// Write LP
	WOVC,	// Write overflow counter
	WOVI,	// Write overflow RUPT inhibit
	WOVR,	// Write overflow
	WP,		// Write P
	WPx,	// Write P (do not reset)
	WP2,	// Write P2
	WQ,		// Write Q
	WS,		// Write S
	WX,		// Write X
	WY,		// Write Y
	WYx,	// Write Y (do not reset)
	WZ,		// Write Z
	//*************************************************************



	//*************************************************************
	// OUTPUTS FROM SUBSYSTEM A; USED AS INPUTS TO SUBSYSTEM B ONLY;
	// NOT USED OUTSIDE CPM
	RSC,	// Read special and central (output to B only, not outside CPM)
	WSC,	// Write special and central (output to B only, not outside CPM)
	WG,		// Write G (output to B only, not outside CPM)
	//*************************************************************
		
		
		
	//*************************************************************
	// EXTERNAL OUTPUTS FROM SUBSYSTEM B
	//
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
	RBK,	// Read BNK
	WA0,	// Write register at address 0 (A)
	WA1,	// Write register at address 1 (Q)
	WA2,	// Write register at address 2 (Z)
	WA3,	// Write register at address 3 (LP)
	WA10,	// Write register at address 10 (octal)
	WA11,	// Write register at address 11 (octal)
	WA12,	// Write register at address 12 (octal)
	WA13,	// Write register at address 13 (octal)
	WA14,	// Write register at address 14 (octal)
	WBK,	// Write BNK
	WGn,	// Write G (normal gates)**
	W20,	// Write into CYR
	W21,	// Write into SR
	W22,	// Write into CYL
	W23,	// Write into SL
	//*************************************************************
		
		
		
	//*************************************************************
	// THESE ARE THE LEFTOVERS -- THEY'RE PROBABLY USED IN SUBSYSTEM C
	//
	CLINH,	// Clear INHINT**
	CLINH1,	// Clear INHINT1**
	CLISQ,	// Clear SNI**
	CLRP,	// Clear RPCELL**
	CLSTA,	// Clear state counter A (STA)**
	CLSTB,	// Clear state counter B (STB)**
	GENRST,	// General Reset**
	INH,	// Set INHINT**
	RPT,	// Read RUPT opcode **
	SBWG,	// Write G from memory
	SETSTB,	// Set the ST1 bit of STB
	WE,		// Write E-MEM from G
	WPCTR,	// Write PCTR (latch priority counter sequence)**
	WSQ,	// Write SQ
	WSTB,	// Write stage counter B (STB)**
	//*************************************************************
#endif
};




// INSTRUCTIONS

	// Op Codes, as they appear in the SQ register.
enum instruction {
	// The code in the SQ register is the same as the op code for these
	// four instructions.
	TC		=00,		// 00	TC K		Transfer Control			1 MCT
	CCS		=01,		// 01	CCS K		Count, Compare, and Skip	2 MCT
	INDEX	=02,		// 02	INDEX K									2 MCT
	XCH		=03,		// 03	XCH K		Exchange					2 MCT
	
	// The SQ register code is the op code + 010 (octal). This happens because all
	// of these instructions have bit 15 set (the sign (SG) bit) while in memory. When the
	// instruction is copied from memory to the memory buffer register (G) to register
	// B, the SG bit moves from bit 15 to bit 16 and the sign is copied back into bit
	// 15 (US). Therefore, the CS op code (04) becomes (14), and so on.
	CS		=014,		// 04	CS K		Clear and Subtract			2 MCT
	TS		=015,		// 05	TS K		Transfer to Storage			2 MCT
	AD		=016,		// 06	AD K		Add							2 or 3 MCT
	MASK	=017,		// 07	MASK K		Bitwise AND					2 MCT

	// These are extended instructions. They are accessed by executing an INDEX 5777
	// before each instruction. By convention, address 5777 contains 47777. The INDEX
	// instruction adds 47777 to the extended instruction to form the SQ op code. For
	// example, the INDEX adds 4 to the 4 op code for MP to produce the 11 (octal; the
	// addition generates an end-around-carry). SQ register code (the 7777 part is a 
	// negative zero).
	MP		=011,		// 04	MP K		Multiply					10 MCT
	DV		=012,		// 05	DV K		Divide						18 MCT
	SU		=013,		// 06	SU K		Subtract					4 or 5 MCT
};

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

const int GOPROG	=02000;	// bottom address of fixed memory

class regSQ : public reg 
{ 
public: 
	regSQ() : reg(4, "%02o") { }
};

class regSTA : public reg 
{ 
public: 
	regSTA() : reg(2, "%01o") { }
};

class regSTB : public reg 
{ 
public: 
	regSTB() : reg(2, "%01o") { }
};

class regBR1 : public reg 
{ 
public: 
	regBR1() : reg(1, "%01o") { }
};

class regBR2 : public reg 
{ 
public: 
	regBR2() : reg(1, "%01o") { }
};

class regCTR : public reg 
{ 
public: 
	regCTR() : reg(3, "%01o") { }
};


class regSNI : public reg 
{ 
public: regSNI() : reg(1, "%01o") { }
};

class SEQ
{
public:
	static void execWP_GENRST();
	static void execWP_WSQ(); 
	static void execWP_NISQ();
	static void execWP_CLISQ();
	static void execWP_ST1();
	static void execWP_ST2();
	static void execWP_TRSM();
	static void execWP_CLSTA();
	static void execWP_WSTB();
	static void execWP_CLSTB();
	static void execWP_SETSTB();
	static void execWP_TSGN();
	static void execWP_TOV();
	static void execWP_TMZ();
	static void execWP_TSGN2();
	static void execWP_CTR();							
	static void execWP_CLCTR();


	static regSNI register_SNI;	// select next intruction flag
	static cpType glbl_cp[MAXPULSES]; // current set of asserted control pulses (MAXPULSES)

	static char* cpTypeString[];

	// Test the currently asserted control pulses; return true if the specified
	// control pulse is active.
	static bool isAsserted(cpType pulse);

	// Return a string containing the names of all asserted control pulses.
	static char* getControlPulses();

	static subseq glbl_subseq; // currently decoded instruction subsequence

	static regSQ register_SQ; // instruction register
	static regSTA register_STA;	// stage counter A
	static regSTB register_STB;	// stage counter B
	static regBR1 register_BR1;	// branch register1
	static regBR2 register_BR2;	// branch register2
	static regCTR register_LOOPCTR;	// loop counter

	static char* instructionString[];
};

#endif