/****************************************************************************
 *  SCL - SCALER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       SCL.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "SCL.h"
#include "CTR.h"


regSCL SCL::register_SCL;
regF17 SCL::register_F17;
regF13 SCL::register_F13;
regF10 SCL::register_F10;

enum oneShotType { // **inferred; not defined in orignal R393 AGC4 spec.
	WAIT_FOR_TRIGGER=0,
	OUTPUT_PULSE=1,		// LSB (bit 1) is the output bit for the one-shot
	WAIT_FOR_RESET=2
};


void regF17::execRP()		
{	
}
void regF17::execWP()		
{
	int bit = SCL::register_SCL.readField(17,17);
	switch(read())
	{
	case WAIT_FOR_TRIGGER:	if(bit==1) write(OUTPUT_PULSE); break;
	case OUTPUT_PULSE:		write(WAIT_FOR_RESET); break;
	case WAIT_FOR_RESET:	if(bit==0) write(WAIT_FOR_TRIGGER); break;
	default: ;
	}
}

void regF13::execRP()		
{	
}
void regF13::execWP()		
{
	int bit = SCL::register_SCL.readField(13,13);
	switch(read())
	{
	case WAIT_FOR_TRIGGER:	if(bit==1) write(OUTPUT_PULSE); break;
	case OUTPUT_PULSE:		write(WAIT_FOR_RESET); break;
	case WAIT_FOR_RESET:	if(bit==0) write(WAIT_FOR_TRIGGER); break;
	default: ;
	}
}

void regF10::execRP()		
{	
}
void regF10::execWP()		
{
	int bit = SCL::register_SCL.readField(10,10);
	switch(read())
	{
	case WAIT_FOR_TRIGGER:	if(bit==1) write(OUTPUT_PULSE); break;
	case OUTPUT_PULSE:		write(WAIT_FOR_RESET);
		CTR::pcUp[TIME1] = 1;
		CTR::pcUp[TIME3] = 1;
		CTR::pcUp[TIME4] = 1;
		break;
	case WAIT_FOR_RESET:	if(bit==0) write(WAIT_FOR_TRIGGER); break;
	default: ;
	}
}

unsigned SCL::F17x()
{
	return register_F17.readField(1,1);
}

unsigned SCL::F13x()
{
	return register_F13.readField(1,1);
}

unsigned SCL::F10x()
{
	return register_F10.readField(1,1);
}

void regSCL::execRP()		
{	
}
void regSCL::execWP()		
{
	//write((read() + 1) % outmask());
	write((read() + 1));
}
