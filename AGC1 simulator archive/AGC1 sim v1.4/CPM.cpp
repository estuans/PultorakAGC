/****************************************************************************
 *  CPM - CONTROL PULSE MATRIX subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CPM.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "CPM.h"
#include "SEQ.h"
#include "MON.h"
#include "CTR.h"
#include "INT.h"
#include "ADR.h"

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

void CPM::clearControlPulses()
{
	for(unsigned i=0; i<MAXPULSES; i++)
		SEQ::glbl_cp[i] = NO_PULSE;
}

void CPM::assert(cpType* pulse)
{
	int j=0;
	for(unsigned i=0; i<MAXPULSES && j<MAX_IPULSES && pulse[j] != NO_PULSE; i++)
	{
		if(SEQ::glbl_cp[i] == NO_PULSE)
		{
			SEQ::glbl_cp[i] = pulse[j];
			j++;
		}
	}
}

void CPM::assert(cpType pulse)
{
	for(unsigned i=0; i<MAXPULSES; i++)
	{
		if(SEQ::glbl_cp[i] == NO_PULSE)
		{
			SEQ::glbl_cp[i] = pulse;
			break;
		}
	}
}

void CPM::controlPulseMatrix()
{
		// Combination logic decodes time pulse, subsequence, branch register, and
		// "select next instruction" latch to get control pulses associated with 
		// those states.

		// Get rid of any old control pulses.
	clearControlPulses();

	if(MON::PURST)
	{
		// PURST should do an asynchronous clear of register SG to put it in the
		// STBY state; this will trigger a GENRST that initializes everything else.
		TPG::register_SG.write(STBY); // initial state of time pulse generator
	}

	static subsequence* subsp[] = 
	{
		&SUB_TC0,	&SUB_CCS0,	&SUB_CCS1,	&SUB_NDX0,	&SUB_NDX1,	&SUB_RSM3,	
		&SUB_XCH0,	&SUB_CS0,	&SUB_TS0,	&SUB_AD0,	&SUB_MASK0,	&SUB_MP0,	
		&SUB_MP1,	&SUB_MP3,	&SUB_DV0,	&SUB_DV1,	&SUB_SU0,	&SUB_RUPT1,	
		&SUB_RUPT3,	&SUB_STD2,	&SUB_PINC,	&SUB_MINC,	&SUB_SHINC,0
	};

		// Get control pulses for the current instruction subsequence.
	if(SEQ::glbl_subseq != NO_SEQ && 
		TPG::register_SG.read() >= TP1 && 
		TPG::register_SG.read() <= TP11)
	{
		subsequence* subseqP = subsp[SEQ::glbl_subseq];
		if(subseqP)
		{
			    // index t-2 because TP1=2, but array is indexed from zero
			controlStep& csref = subseqP->tp[TPG::register_SG.read()-2];

			brType b = (brType) ((SEQ::register_BR1.read() << 1) | SEQ::register_BR2.read());
			controlSubStep& cssref = csref.substep[b];
			if(cssref.br == NO_BR)
				cssref = csref.substep[0];

			cpType* p = cssref.pulse;
			assert(p);
		}
	}

		// Add any additional control pulses.
	if(SEQ::register_LOOPCTR.read() == 6)
	{
		assert(ST2);	// STA <- 2
		assert(CLCTR);	// CTR <- 0
	}

	// ******* THIS ORIGINALLY WAS AT THE END OF THE FUNCTION	
	if(SEQ::isAsserted(WG))
	{
		switch(ADR::register_S.read())
		{
		case 020:	assert(W20); break;
		case 021:	assert(W21); break;	
		case 022:	assert(W22); break;
		case 023:	assert(W23); break;
		default: if(ADR::register_S.read() >= 020) assert(WGn);
		}
	}
	if(SEQ::isAsserted(RSC))
	{
		switch(ADR::register_S.read())
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
		case 015:	assert(RBK); break;
		default: break; // 016, 017
		}
	}
	if(SEQ::isAsserted(WSC))		
		switch(ADR::register_S.read())
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


	switch(TPG::register_SG.read())
	{
	case STBY:
		assert(GENRST);
		// inhibit all alarms
		CTR::resetAllpc(); // clear priority cells???
		INT::resetAllRupt(); // clear interrupt requests
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
		if(	ADR::register_S.read() > 017		&&	// not a central register
			ADR::register_S.read() < 02000	&&	// not fixed memory
			SEQ::glbl_subseq != DV1			&&	// not a loop counter subseq
			SEQ::glbl_subseq != MP1)
		{
			assert(SBEWG);
		}
		if( ADR::register_S.read() == 017 ) assert (INH);	// INHINT (INDEX 017)
		if( ADR::register_S.read() == 016 ) assert (CLINH);	// RELINT (INDEX 016)
		break;
	case TP6:
		// FMEM must be available in G register by TP7
		if(	ADR::register_S.read() >= 02000	&&	// not eraseable memory
			SEQ::glbl_subseq != DV1			&&	// not a loop counter subseq
			SEQ::glbl_subseq != MP1)
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
		if(	ADR::register_S.read() > 017		&&	// not a central register
			ADR::register_S.read() < 02000	&&	// not fixed memory
			SEQ::glbl_subseq != DV1			&&	// not a loop counter subseq
			SEQ::glbl_subseq != MP1)
		{
			assert(WE);
		}
		// Additional interrupts are inhibited during servicing of an interrupt;
		// Remove the inhibition when RESUME is executed (INDEX 025)
		if(SEQ::glbl_subseq==RSM3) assert(CLRP);
		break;

	case TP12:
		// DISABLE INPUT CHANGE TO PRIORITY COUNTER (reenable after TP1)
		// Check the priority counters; service any waiting inputs on the next
		// memory cycle.
		assert(WPCTR);
		if(SEQ::register_SNI.read() == 1) // if SNI is set, get next instruction
		{
			if(	INT::getPriorityRupt()				// if interrupt requested
				&& INT::register_RPCELL.read() == 0	// and interrupt not currently being serviced
				&& INT::register_INHINT1.read() == 0	// and interrupt not inhibited for 1 instruction
				&& INT::register_INHINT.read() == 0	// and interrupts enabled (RELINT)
				&& MON::RUN) // and the operator is not single-stepping
			{
				// Interrupt: SQ <- 0 (the default RW bus state)
				assert(RPT);		// latch interrupt vector
				assert(SETSTB);	// STB <- 1
			}
			else
			{
				// Normal instruction
				assert(RB);		// SQ <- B
				assert(CLSTB);		// STB <- 0
			}
			assert(WSQ);
			assert(CLSTA);			// STA <- 0

		}
		else if(CTR::register_PSEQ.read() == NOPSEL) // if previous sequence was not a counter
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
