/****************************************************************************
 *  MBF - MEMORY BUFFER REGISTER subsystem
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       9/22/01
 *  FILE:       MBF.cpp
 *
 *  NOTES: see header file.
 *    
 *****************************************************************************
 */
#include "MBF.h"
#include "SEQ.h"
#include "ADR.h"
#include "BUS.h"
#include "PAR.h"
#include "EMM.h"
#include "FMM.h"


regG MBF::register_G; // memory buffer register

unsigned MBF::conv_RG[] =
{	SG, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_WGn[] =
{	SG, BX, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_W20[] =
{	B1, BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned MBF::conv_W21[] =
{	SG, BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned MBF::conv_W22[] =
{	B14, BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };

unsigned MBF::conv_W23[] =
{	SG, BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };

unsigned MBF::conv_SBEWG[] =
{	SGM, PM, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_SBFWG[] =
{	SGM, PM, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_WE[] =
{	P, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };


void regG::execRP()		
{	if(SEQ::isAsserted(RG))		
		if(ADR::register_S.read() >= 020) 
			BUS::glbl_RWBUS|=shiftData(BUS::glbl_RWBUS, read(), MBF::conv_RG); 
}

void regG::execWP()		
{	
	if(SEQ::isAsserted(CLG))	write(0); 

	if(SEQ::isAsserted(RP2))	writeField(15, 15, PAR::register_P2.read());
	if(SEQ::isAsserted(GP))		writeField(15, 15, PAR::gen1_15Parity(PAR::register_P.read()));
	if(SEQ::isAsserted(WGn))	writeShift(BUS::glbl_RWBUS, MBF::conv_WGn);

							// This is only used in PINC, MINC, and SHINC. Does not clear G
							// register; writes (ORs) into G from RWBus and writes into parity
							// from 1-15 generator. All done in one operation, although I show
							// it in two steps here. The sequence calls CLG in a previous TP.
	if(SEQ::isAsserted(WGx))
	{
		write(read() | shiftData(read(), BUS::glbl_RWBUS, MBF::conv_WGn));
		writeField(15, 15, PAR::gen1_15Parity(PAR::register_P.read()));
	}

	if(SEQ::isAsserted(W20))	writeShift(BUS::glbl_RWBUS, MBF::conv_W20);
	if(SEQ::isAsserted(W21))	writeShift(BUS::glbl_RWBUS, MBF::conv_W21);
	if(SEQ::isAsserted(W22))	writeShift(BUS::glbl_RWBUS, MBF::conv_W22);
	if(SEQ::isAsserted(W23))	writeShift(BUS::glbl_RWBUS, MBF::conv_W23);
	if(SEQ::isAsserted(SBEWG))	writeShift(MBF::readMemory(), MBF::conv_SBEWG);
	if(SEQ::isAsserted(SBFWG))	writeShift(MBF::readMemory(), MBF::conv_SBFWG);
	if(SEQ::isAsserted(WE))		MBF::writeMemory(shiftData(0, MBF::register_G.read(), MBF::conv_WE));
}


unsigned MBF::getEffectiveAddress()
{
		// Return the 14-bit address selected by lower 10 bits of the S register (1K)
		// and the bank decoder (which selects the 1K bank)
	unsigned lowAddress = ADR::register_S.readField(10,1);

	if(ADR::bankDecoder() == 0)
		return lowAddress;

	unsigned highAddress = ADR::bankDecoder() << 10;
	return highAddress | lowAddress;
}

unsigned MBF::readMemory()
{
		// Return memory value addressed by lower 10 bits of the S register (1K) and the
		// bank decoder (which selects the 1K bank)
	unsigned lowAddress = ADR::register_S.readField(10,1);

	if(ADR::bankDecoder() == 0)
		return EMM::register_EMEM[lowAddress].read();

	unsigned highAddress = (ADR::bankDecoder()-1) << 10;
	return FMM::register_FMEM[highAddress | lowAddress].read();
}

void MBF::writeMemory(unsigned data)
{
		// Write into erasable memory addressed by lower 10 bits of the S register (1K) 
		// and the bank decoder (which selects the 1K bank)
	unsigned lowAddress = ADR::register_S.readField(10,1);

	if(ADR::bankDecoder() == 0)
	{
		EMM::register_EMEM[lowAddress].write(data);
		EMM::register_EMEM[lowAddress].clk(); // not a synchronous FF, so execute immediately
	}
}

unsigned MBF::readMemory(unsigned address)
{
		// Address is 14 bits. This function is used by the simulator for examining
		// memory; it is not part of the AGC design.
	unsigned lowAddress = address & 01777;
	unsigned bank = (address & 036000) >> 10;

	if(bank == 0)
		return EMM::register_EMEM[lowAddress].read();

	unsigned highAddress = (bank-1) << 10;
	return FMM::register_FMEM[highAddress | lowAddress].read();
}

void MBF::writeMemory(unsigned address, unsigned data)
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
		EMM::register_EMEM[lowAddress].write(data);
		EMM::register_EMEM[lowAddress].clk();  // not a synchronous FF, so execute immediately
	}

	unsigned highAddress = (bank-1) << 10;
	FMM::register_FMEM[highAddress | lowAddress].write(data);
	FMM::register_FMEM[highAddress | lowAddress].clk();  // not a synchronous FF, so execute immediately
}