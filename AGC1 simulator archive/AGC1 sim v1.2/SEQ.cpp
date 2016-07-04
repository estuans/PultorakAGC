/****************************************************************************
 *  SEQ - SEQUENCE GENERATOR subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       SEQ.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "SEQ.h"
#include "ADR.h"
#include "BUS.h"


regSNI SEQ::register_SNI;	// select next intruction flag
cpType SEQ::glbl_cp[]; // current set of asserted control pulses (MAXPULSES)

regSQ SEQ::register_SQ; // instruction register
regSTA SEQ::register_STA;	// stage counter A
regSTB SEQ::register_STB;	// stage counter B
regBR1 SEQ::register_BR1;	// branch register1
regBR2 SEQ::register_BR2;	// branch register2
regCTR SEQ::register_LOOPCTR;	// loop counter
subseq SEQ::glbl_subseq; // currently decoded instruction subsequence

char* SEQ::instructionString[] =
{
	"TC",
	"CCS",
	"INDEX",
	"XCH",
	"***",
	"***",
	"***",
	"***",
	"***",
	"MP",
	"DV",
	"SU",
	"CS",
	"TS",
	"AD",
	"MASK"
};

char* SEQ::cpTypeString[] = 
{
	"NO_PULSE", "CI", "CLG", "CLINH", "CLINH1", "CLISQ", "CLCTR", "CLRP", 
	"CLSTA", "CLSTB", "CTR", "GENRST", "GP", "INH", "KRPT", "NISQ",
	"RA0", "RA1", "RA2", "RA3", "RA4", "RA5", "RA6", "RA7", "RA10", "RA11", "RA12", "RA13", "RA14", "RA", "RB",
	"RBK", "RB14", "RC", "RG", "RLP", "RPT", "RP2", "RQ", "RRPA", "RSB", "RSC", "RSCT",
	"RU", "RZ", "R1", "R1C", "R2", "R22", "R24", "SBEWG", "SBFWG", "SETSTB", 
	"ST1", "ST2", "TMZ", 
	"TOV", "TP", "TRSM", "TSGN", "TSGN2", 
	"WA0", "WA1", "WA2", "WA3","WA10", "WA11", "WA12", "WA13", "WA14", 
	"WA", "WALP", "WB", "WBK", "WE", "WG", "WGn", "WGx",
	"WLP", "WOVC", "WOVI", "WOVR", "WP", "WPx", "WP2", "WPCTR", "WQ", "WS", "WSC", "WSQ", "WSTB",
	"WX", "WY", "WYx", "WZ", "W20", "W21", "W22", "W23"
};

void regSQ::execRP() { }
void regSQ::execWP()	
{	
	if(SEQ::isAsserted(GENRST))	write(0);
	if(SEQ::isAsserted(WSQ))	write(BUS::glbl_RWBUS >> 12); 
}

void regSNI::execRP() { }
void regSNI::execWP()	
{	
	if(SEQ::isAsserted(NISQ))	writeField(1,1,1); // change to write(1)??
	if(SEQ::isAsserted(CLISQ))	writeField(1,1,0);  // change to write(0)??
}

bool SEQ::isAsserted(cpType pulse)
{
	for(unsigned i=0; i<MAXPULSES; i++)
		if(glbl_cp[i] == pulse) return true;
	return false;
}




char* SEQ::getControlPulses()
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

void regSTA::execRP() { }
void regSTA::execWP()	
{	
	if(SEQ::isAsserted(ST1))		
		writeField(1,1,1);	
	if(SEQ::isAsserted(ST2))		
		writeField(2,2,1);
	if(SEQ::isAsserted(TRSM))	
		if(ADR::register_S.read()==025) 
			writeField(2,2,1);
	if(SEQ::isAsserted(CLSTA))	
		writeField(2,1,0); 
}

void regSTB::execRP() { }
void regSTB::execWP()	
{	
	if(SEQ::isAsserted(WSTB))	
		write(SEQ::register_STA.read()); 
	if(SEQ::isAsserted(CLSTB))	
		writeField(2,1,0); 
	if(SEQ::isAsserted(SETSTB))	
		writeField(2,1,1); 
}

void regBR1::execRP() { }
void regBR1::execWP()	
{
	if(SEQ::isAsserted(GENRST))	write(0);

		// Set Branch 1 FF
		//	if sign bit is '1' (negative sign)
		//	if negative overflow (sign==1; overflow==0)
	if(SEQ::isAsserted(TSGN))	
		if(BUS::glbl_RWBUS & 0100000) 
			write(1); 
		else 
			write(0);
		if(SEQ::isAsserted(TOV))		
			if((BUS::glbl_RWBUS & 0140000) == 0100000) 
			write(1); 
		else 
			write(0);
} 

void regBR2::execRP() { }
void regBR2::execWP()	
{
	if(SEQ::isAsserted(GENRST))	
		write(0);

		// Set Branch 2 FF
		//	if positive overflow (sign==0; oveflow==1)
		//	if minus zero
		//	if sign bit is '1' (negative sign)
	if(SEQ::isAsserted(TOV))		
		if((BUS::glbl_RWBUS & 0140000) == 0040000) 
			write(1); 
		else 
			write(0);
		if(SEQ::isAsserted(TMZ))		
			if(BUS::glbl_RWBUS == 0177777) 
			write(1); 
		else 
			write(0);
		if(SEQ::isAsserted(TSGN2))	
			if(BUS::glbl_RWBUS  & 0100000) 
			write(1); 
		else 
			write(0);
} 


void regCTR::execRP() { }
void regCTR::execWP()	
{	
	if(SEQ::isAsserted(CTR))		
		write(read()+1);
							
	if(SEQ::isAsserted(CLCTR))	
		write(0); 
}