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

	// The actual bit 15 of register_G is not used.
regG MBF::register_G;     // memory buffer register (except bit 15: parity)

unsigned MBF::conv_RG[] =
{	SG,  SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_SBWG[] =
{	SGM, BX, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };

unsigned MBF::conv_WE[] =
{	BX,  SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1 };


unsigned MBF::conv_W20[] =
{	B1,  BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };

unsigned MBF::conv_W21[] =
{	SG,  BX, SG, B14, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2 };


unsigned MBF::conv_W22[] =
{	B14, BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };

unsigned MBF::conv_W23[] =
{	SG,  BX, B13, B12, B11, B10, B9, B8, B7, B6, B5, B4, B3, B2, B1, SG };





void MBF::execRP_RG()
{
		// Shouldn't this be the effective address and not just the S register??? *********************
	if(ADR::register_S.read() >= 020)
		BUS::glbl_READ_BUS = register_G.shiftData(BUS::glbl_READ_BUS, register_G.read(), MBF::conv_RG); 
}







void MBF::execWP_WGn()
{
	register_G.write(BUS::glbl_WRITE_BUS);
}

void MBF::execWP_WGx()
{
		// This is only used in PINC, MINC, and SHINC. Does not clear G
		// register; writes (ORs) into G from RWBus and writes into parity
		// from 1-15 generator. All done in one operation, although I show
		// it in two steps here. The sequence calls CLG in a previous TP.
	register_G.write(BUS::glbl_WRITE_BUS);
}

void MBF::execWP_W20()
{
	register_G.writeShift(BUS::glbl_WRITE_BUS, MBF::conv_W20);
}

void MBF::execWP_W21()
{
	register_G.writeShift(BUS::glbl_WRITE_BUS, MBF::conv_W21);
}

void MBF::execWP_W22()
{
	register_G.writeShift(BUS::glbl_WRITE_BUS, MBF::conv_W22);
}

void MBF::execWP_W23()
{
	register_G.writeShift(BUS::glbl_WRITE_BUS, MBF::conv_W23);
}


void MBF::execWP_SBWG()
{
	register_G.writeShift(MBF::readMemory(), MBF::conv_SBWG);
}


void MBF::execWP_WE()
{
		// Write G into memory; shift the sign to bit 15 and get the parity (bit 16) from G15
	MBF::writeMemory(register_G.shiftData(0, MBF::register_G.read(), MBF::conv_WE) | (PAR::register_G15.read() << 15));
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