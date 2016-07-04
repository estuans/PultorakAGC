/****************************************************************************
 *  CLK - CLOCK subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       CLK.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "CLK.h"
#include "INP.h"
#include "OUT.h"
#include "MBF.h"
#include "ADR.h"
#include "SEQ.h"
#include "ALU.h"
#include "CRG.h"
#include "CTR.h"
#include "INT.h"
#include "PAR.h"
#include "TPG.h"
#include "SCL.h"


	// A container for all registers. This is kept so we can iterate through
	// all registers to execute the control pulses. For simulation purposes
	// only; this has no counterpart in the hardware AGC.
reg* CLK::registerList[] = // registers are in no particular sequence
{
	&INP::register_IN0, &INP::register_IN1, &INP::register_IN2, &INP::register_IN3,
	&OUT::register_OUT0, &OUT::register_OUT1, &OUT::register_OUT2, 
	&OUT::register_OUT3, &OUT::register_OUT4,
	&MBF::register_G, &ADR::register_S, &ADR::register_BNK, &SEQ::register_SQ, &ALU::register_B,
	&CRG::register_Q, &CRG::register_Z, &CRG::register_LP, &CRG::register_A, &ALU::register_X,
	&ALU::register_Y, &ALU::register_U, 
	&SEQ::register_STA, &SEQ::register_STB, &SEQ::register_SNI,
	&SEQ::register_LOOPCTR, &ALU::register_CI, &SEQ::register_BR1, &SEQ::register_BR2, 
	&CTR::register_PSEQ, &CTR::register_PCELL, 
	&INT::register_RPCELL, &INT::register_INHINT1, &INT::register_INHINT,
	&PAR::register_P, &PAR::register_P2,
	&TPG::register_SG,
	&SCL::register_SCL,
	&SCL::register_F17, &SCL::register_F13, &SCL::register_F10,
	0 // zero is end-of-list flag
};



void CLK::clkAGC()
{
		// Now that all the inputs are set up, clock the registers so the outputs
		// can change state in accordance with the inputs.
	for(int i=0; registerList[i]; i++)
	{
		registerList[i]->clk();
	}
}




	void execR_NOPULSE(){	}
	void execR_RA0()	{ CRG::execRP_RA0(); }
	void execR_RA1()	{ CRG::execRP_RA1(); }
	void execR_RA2()	{ CRG::execRP_RA2(); }
	void execR_RA3()	{ CRG::execRP_RA3(); }
	void execR_RA4()	{ INP::execRP_RA4(); }
	void execR_RA5()	{ INP::execRP_RA5(); }
	void execR_RA6()	{ INP::execRP_RA6(); }
	void execR_RA7()	{ INP::execRP_RA7(); }
	void execR_RA10()	{ OUT::execRP_RA10(); }
	void execR_RA11()	{ OUT::execRP_RA11(); }
	void execR_RA12()	{ OUT::execRP_RA12(); }
	void execR_RA13()	{ OUT::execRP_RA13(); }
	void execR_RA14()	{ OUT::execRP_RA14(); }
	void execR_RA()		{ CRG::execRP_RA();	}
	void execR_RB()		{ ALU::execRP_RB();	}
	void execR_RBK()	{ ADR::execRP_RBK(); }
	void execR_RB14()	{ ALU::execRP_RB14(); }
	void execR_RC()		{ ALU::execRP_RC();	}
	void execR_RG()		{ MBF::execRP_RG();	}
	void execR_RLP()	{ CRG::execRP_RLP(); }
	void execR_RQ()		{ CRG::execRP_RQ();	}
	void execR_RRPA()	{ INT::execRP_RRPA(); }
	void execR_RSB()	{ ALU::execRP_RSB(); }
	void execR_RSCT()	{ CTR::execRP_RSCT(); }
	void execR_RU()		{ ALU::execRP_RU();	}
	void execR_RZ()		{ CRG::execRP_RZ();	}
	void execR_R1()		{ ALU::execRP_R1();	}
	void execR_R1C()	{ ALU::execRP_R1C(); }
	void execR_R2()		{ ALU::execRP_R2();	}
	void execR_R22()	{ ALU::execRP_R22(); }
	void execR_R24()	{ ALU::execRP_R24(); }

EXECTYPE execR[] =
{
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_RA0,
	execR_RA1,
	execR_RA2,
	execR_RA3,
	execR_RA4,
	execR_RA5,
	execR_RA6,
	execR_RA7,
	execR_RA10,
	execR_RA11,
	execR_RA12,
	execR_RA13,
	execR_RA14,
	execR_RA,
	execR_RB,
	execR_RBK,
	execR_RB14,
	execR_RC,
	execR_RG,
	execR_RLP,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_RQ,
	execR_RRPA,
	execR_RSB,
	execR_NOPULSE,
	execR_RSCT,
	execR_RU,
	execR_RZ,
	execR_R1,
	execR_R1C,
	execR_R2,
	execR_R22,
	execR_R24,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE,
	execR_NOPULSE
};

void CLK::doexecR(int pulse) { execR[pulse](); } 



	void execW_NOPULSE(){	}
	void execW_CI()		{ ALU::execWP_CI();	}
	void execW_CLG()	{ MBF::execWP_CLG(); }
	void execW_CLINH()	{ INT::execWP_CLINH(); }
	void execW_CLINH1()	{ INT::execWP_CLINH1();	}
	void execW_CLISQ()	{ SEQ::execWP_CLISQ(); }
	void execW_CLCTR()	{ SEQ::execWP_CLCTR(); }
	void execW_CLRP()	{ INT::execWP_CLRP(); }
	void execW_CLSTA()	{ SEQ::execWP_CLSTA(); }
	void execW_CLSTB()	{ SEQ::execWP_CLSTB(); }
	void execW_CTR()	{ SEQ::execWP_CTR(); }
	void execW_GENRST()	{ SEQ::execWP_GENRST(); ALU::execWP_GENRST(); CTR::execWP_GENRST(); INT::execWP_GENRST(); OUT::execWP_GENRST(); }
	void execW_GP()		{ MBF::execWP_GP();	OUT::execWP_GP(); }
	void execW_INH()	{ INT::execWP_INH(); }
	void execW_KRPT()	{ INT::execWP_KRPT(); }
	void execW_NISQ()	{ SEQ::execWP_NISQ(); }
	void execW_RPT()	{ INT::execWP_RPT(); }
	void execW_RP2()	{ MBF::execWP_RP2(); OUT::execWP_RP2(); }
	void execW_SBEWG()	{ MBF::execWP_SBEWG(); }
	void execW_SBFWG()	{ MBF::execWP_SBFWG(); }
	void execW_SETSTB()	{ SEQ::execWP_SETSTB();	}
	void execW_ST1()	{ SEQ::execWP_ST1(); }
	void execW_ST2()	{ SEQ::execWP_ST2(); }
	void execW_TMZ()	{ SEQ::execWP_TMZ(); }
	void execW_TOV()	{ SEQ::execWP_TOV(); }
	void execW_TRSM()	{ SEQ::execWP_TRSM(); }
	void execW_TSGN()	{ SEQ::execWP_TSGN(); }
	void execW_TSGN2()	{ SEQ::execWP_TSGN2(); }
	void execW_WA0()	{ CRG::execWP_WA0(); }
	void execW_WA1()	{ CRG::execWP_WA1(); }
	void execW_WA2()	{ CRG::execWP_WA2(); }
	void execW_WA3()	{ CRG::execWP_WA3(); }
	void execW_WA10()	{ OUT::execWP_WA10(); }
	void execW_WA11()	{ OUT::execWP_WA11(); }
	void execW_WA12()	{ OUT::execWP_WA12(); }
	void execW_WA13()	{ OUT::execWP_WA13(); }
	void execW_WA14()	{ OUT::execWP_WA14(); }
	void execW_WA()		{ CRG::execWP_WA();	}
	void execW_WALP()	{ CRG::execWP_WALP(); }
	void execW_WB()		{ ALU::execWP_WB();	}
	void execW_WBK()	{ ADR::execWP_WBK(); }
	void execW_WE()		{ MBF::execWP_WE();	}
	void execW_WGn()	{ MBF::execWP_WGn(); }
	void execW_WGx()	{ MBF::execWP_WGx(); }
	void execW_WLP()	{ CRG::execWP_WLP(); }
	void execW_WOVC()	{ CTR::execWP_WOVC(); }
	void execW_WOVI()	{ INT::execWP_WOVI(); }
	void execW_WOVR()	{ CTR::execWP_WOVR(); }
	void execW_WP()		{ PAR::execWP_WP();	}
	void execW_WPx()	{ PAR::execWP_WPx(); }
	void execW_WP2()	{ PAR::execWP_WP2(); }
	void execW_WPCTR()	{ CTR::execWP_WPCTR(); }
	void execW_WQ()		{ CRG::execWP_WQ();	}
	void execW_WS()		{ ADR::execWP_WS();	}
	void execW_WSQ()	{ SEQ::execWP_WSQ(); }
	void execW_WSTB()	{ SEQ::execWP_WSTB(); }
	void execW_WX()		{ ALU::execWP_WX();	}
	void execW_WY()		{ ALU::execWP_WY();	}
	void execW_WYx()	{ ALU::execWP_WYx(); }
	void execW_WZ()		{ CRG::execWP_WZ();	}
	void execW_W20()	{ MBF::execWP_W20(); }
	void execW_W21()	{ MBF::execWP_W21(); }
	void execW_W22()	{ MBF::execWP_W22(); }
	void execW_W23()	{ MBF::execWP_W23(); }


EXECTYPE execW[] =
{
	execW_NOPULSE,
	execW_CI,
	execW_CLG,
	execW_CLINH,
	execW_CLINH1,
	execW_CLISQ,
	execW_CLCTR,
	execW_CLRP,
	execW_CLSTA,
	execW_CLSTB,
	execW_CTR,
	execW_GENRST,
	execW_GP,
	execW_INH,
	execW_KRPT,
	execW_NISQ,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_RPT,
	execW_RP2,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_NOPULSE,
	execW_SBEWG,
	execW_SBFWG,
	execW_SETSTB,
	execW_ST1,
	execW_ST2,
	execW_TMZ,
	execW_TOV,
	execW_NOPULSE,
	execW_TRSM,
	execW_TSGN,
	execW_TSGN2,
	execW_WA0,
	execW_WA1,
	execW_WA2,
	execW_WA3,
	execW_WA10,
	execW_WA11,
	execW_WA12,
	execW_WA13,
	execW_WA14,
	execW_WA,
	execW_WALP,
	execW_WB,
	execW_WBK,
	execW_WE,
	execW_NOPULSE,
	execW_WGn,
	execW_WGx,
	execW_WLP,
	execW_WOVC,
	execW_WOVI,
	execW_WOVR,
	execW_WP,
	execW_WPx,
	execW_WP2,
	execW_WPCTR,
	execW_WQ,
	execW_WS,
	execW_NOPULSE,
	execW_WSQ,
	execW_WSTB,
	execW_WX,
	execW_WY,
	execW_WYx,
	execW_WZ,
	execW_W20,
	execW_W21,
	execW_W22,
	execW_W23
};

void CLK::doexecW(int pulse) { execW[pulse](); } 


