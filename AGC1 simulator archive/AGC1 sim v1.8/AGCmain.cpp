/****************************************************************************
 *  AGC4 (Apollo Guidance Computer) BLOCK I Simulator
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       06/01/02
 *  FILE:       AGCmain.cpp
 *
 *  VERSIONS:
 *    1.0 - initial version.
 *    1.1 - fixed minor bugs; passed automated test and checkout programs:
 *            teco1.asm, teco2.asm, and teco3.asm to test basic instructions,
 *            extended instructions, and editing registers.
 *    1.2 - decomposed architecture into subsystems; fixed minor bug in DSKY
 *            keyboard logic (not tested in current teco*.asm suite).
 *            Implemented scaler pulses F17, F13, F10. Tied scaler output to
 *            involuntary counters and interrupts. Implemented counter overflow
 *            logic and tied it to interrupts and other counters. Added simple
 *            set/clear breakpoint. Fixed a bug in bank addressing.
 *    1.3 - fixed bugs in the DSKY. Added 14-bit effective address (CADR) to the
 *            simulator display output. Inhibited interrupts when the operator
 *            single-steps the AGC.
 *    1.4 - performance enhancements. Recoded the control pulse execution code 
 *            for better simulator performance. Also changed the main loop so it 
 *            polls the keyboard and system clock less often for better performance.
 *    1.5 - reversed the addresses of TIME1 and TIME2 so TIME2 occurs first.
 *            This is the way its done in Block II so that a common routine (READLO)
 *            can be used to read the double word for AGC time.
 *    1.6 - added indicators for 'CHECK FAIL' and 'KEY RELS'. Mapped them to OUT1,
 *            bits 5 and 7. Added a function to display the current location in
 *            the source code list file using the current CADR.
 *    1.7 - increased length of 'examine' function display. Any changes in DSKY now
 *            force the simulator to update the display immediately. Added a 'watch'
 *            function that looks for changes in a memory location and halts the
 *            AGC. Added the 'UPTL', 'COMP', and "PROG ALM" lights to the DSKY.
 *    1.8 - started reorganizing the simulator in preparation for H/W logic design.
 *            Eliminated slow (1Hz) clock capability. Removed BUS REQUEST feature.
 *            Eliminated SWRST switch.
 *
 *  SOURCES:
 *  Mostly based on information from "Logical Description for the Apollo Guidance
 *  Computer (AGC4)", Albert Hopkins, Ramon Alonso, and Hugh Blair-Smith, R-393,
 *  MIT Instrumentation Laboratory, 1963.
 *
 *  PORTABILITY:
 *  Compiled with Microsoft Visual C++ 6.0 standard edition. Should be fairly
 *  portable, except for some Microsoft-specific I/O and timer calls in this file.
 *
 *  NOTE: set tabs to 4 spaces to keep columns formatted correctly.
 *
 *****************************************************************************
 */

#include <conio.h>

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "reg.h"

#include "TPG.h"
#include "MON.h"
#include "SCL.h"
#include "SEQ.h"
#include "INP.h"
#include "OUT.h"
#include "BUS.h"
#include "DSP.h"
#include "ADR.h"
#include "PAR.h"
#include "MBF.h"
#include "EMM.h"
#include "FMM.h"
#include "CTR.h"
#include "INT.h"
#include "KBD.h"
#include "CRG.h"
#include "ALU.h"
#include "CPM.h"
#include "ISD.h"
#include "CLK.h"


extern bool dskyChanged;

//-----------------------------------------------------------------------
// CONTROL LOGIC

void genAGCStates()
{
		// 1) Decode the current instruction subsequence (glbl_subseq).
	SEQ::glbl_subseq = ISD::instructionSubsequenceDecoder();

		// 2) Build a list of control pulses for this state.
	CPM::controlPulseMatrix();

		// 3) Execute the control pulses for this state. In the real AGC, these occur
		// simultaneously. Since we can't achieve that here, we break it down into the
		// following steps:
		// Most operations involve data transfers--usually reading data from
		// a register onto a bus and then writing that data into another register. To
		// approximate this, we first iterate through all registers to perform
		// the 'read' operation--this transfers data from register to bus.
		// Then we again iterate through the registers to do 'write' operations,
		// which move data from the bus back into the register.

	BUS::glbl_RWBUS = 0; // clear bus; necessary because words are logical OR'ed onto the bus.

		// Now start executing the pulses: 

	// First, read register outputs onto the bus or anywhere else.
	int i;
	for(i=0; i<MAXPULSES && SEQ::glbl_cp[i] != NO_PULSE; i++)
	{
		CLK::doexecR(SEQ::glbl_cp[i]);
	}

		// Now, write the bus and any other signals into the register inputs.

	for(i=0; i<MAXPULSES && SEQ::glbl_cp[i] != NO_PULSE; i++)
	{
		CLK::doexecW(SEQ::glbl_cp[i]);
	}

		// Always execute these pulses.
	SCL::doexecWP_SCL();
	SCL::doexecWP_F17();
	SCL::doexecWP_F13();
	SCL::doexecWP_F10();
	TPG::doexecWP_TPG();
}

//-----------------------------------------------------------------------
// SIMULATION LOGIC

	// contains prefix for source filename; i.e.: the portion
	// of the filename before .obj or .lst
char filename[80];


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

bool breakpointEnab = false;
unsigned breakpoint = 0;
void toggleBreakpoint()
{
	if(!breakpointEnab)
	{
		char b[80];
		strcpy(b, getCommand("Set breakpoint: -- enter 14-bit CADR (octal): "));
		cout << endl;

		breakpoint = strtol(b,0,8);
		breakpointEnab = true;
	}
	else
	{
		cout << "Clearing breakpoint." << endl;
		breakpointEnab = false;
	}
}

bool watchEnab = false;
unsigned watchAddr = 0;
unsigned oldWatchValue = 0;
void toggleWatch()
{
	if(!watchEnab)
	{
		char b[80];
		strcpy(b, getCommand("Set watch: -- enter 14-bit CADR (octal): "));
		cout << endl;

		watchAddr = strtol(b,0,8);
		watchEnab = true;
		oldWatchValue = MBF::readMemory(watchAddr);

		char buf[100];
		sprintf(buf, "%06o:  %06o", watchAddr, oldWatchValue);
		cout << buf << endl;
	}
	else
	{
		cout << "Clearing watch." << endl;
		watchEnab = false;
	}
}

void incrCntr()
{
	char cntrname[80];
	strcpy(cntrname, getCommand("Increment counter: -- enter pcell (0-19): "));
	cout << endl;

	int pc = atoi(cntrname);
	CTR::pcUp[pc] = 1;
}

void decrCntr()
{
	char cntrname[80];
	strcpy(cntrname, getCommand("Decrement counter: -- enter pcell (0-19): "));
	cout << endl;

	int pc = atoi(cntrname);
	CTR::pcDn[pc] = 1;
}

void interrupt()
{
	char iname[80];
	strcpy(iname, getCommand("Interrupt: -- enter priority (1-5): "));
	cout << endl;

	int i = atoi(iname) - 1;
	INT::rupt[i] = 1;
}

	// Load AGC memory from the specified file.
void loadMemory()
{
	strcpy(filename, getCommand("Load Memory -- enter filename: "));
	cout << endl;

		// Add the .obj extension.
	char fname[80];
	strcpy(fname, filename);
	strcat(fname, ".obj");


	FILE* fp = fopen(fname, "r");
	if(!fp)
	{
		perror("fopen failed:");
		cout << "*** ERROR: Can't load memory for file: " << fname << endl;
		return;
	}
	unsigned addr;
	unsigned data;
	while(fscanf(fp, "%o %o", &addr, &data) != EOF)
	{
		MBF::writeMemory(addr, data);
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
		sprintf(buf, "%06o %06o\n", addr, MBF::readMemory(addr));
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
	for(unsigned i=address; i<address+23; i++)
	{
		sprintf(buf, "%06o:  %06o", i, MBF::readMemory(i));
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

void updateAGCDisplay()
{
	static bool displayTimeout = false;
	static int clockCounter = 0;

	if(checkElapsedTime(2)) displayTimeout = true;
	if(MON::FCLK)
	{
		if(MON::RUN)
		{
				// update every 2 seconds at the start of a new instruction
			if(displayTimeout || dskyChanged)
			{
				clockCounter++;
				if(
					(TPG::register_SG.read() == TP12 && SEQ::register_SNI.read() == 1) || 
					(TPG::register_SG.read() == STBY) ||
					clockCounter > 500 ||
					dskyChanged)
				{
					MON::displayAGC();
					displayTimeout = false;
					clockCounter = 0;
					dskyChanged = false;
				}
			}
		}
		else
		{
			static bool displayOnce = false;
			if(TPG::register_SG.read() == WAIT)
			{
				if(displayOnce == false)
				{
					MON::displayAGC();
					displayOnce = true;
					clockCounter = 0;
				}
			}
			else
			{
				displayOnce = false;
			}
		}
	}
	else
		MON::displayAGC(); // When the clock is manual or slow, always update.
}

void showMenu()
{
	cout << "AGC4 EMULATOR MENU:" << endl;
	cout << " 'r' = RUN:  toggle RUN/HALT switch upward to the RUN position." << endl;
}


const int startCol	= 0;	// columns are numbered 0-n
const int colLen	= 5;	// number of chars in column

const int maxLines	= 23;	// # of total lines to display
const int noffset	= 10;	// # of lines prior to, and including, selected line

const int maxLineLen = 79;

void showSourceCode()
{
			// Add the .lst extension.
	char fname[80];
	strcpy(fname, filename);
	strcat(fname, ".lst");

		// Open the file containing the source code listing.
	FILE* fp = fopen(fname, "r");
	if(!fp)
	{
		perror("fopen failed:");
		cout << "*** ERROR: Can't load source list file: " << fname << endl;
		return;
	}
	cout << endl;

		// Get the address of the source code line to display.
		// The address we want is the current effective address is the
		// S and bank registers.
	char CADR[colLen+1];
    sprintf(CADR, "%05o", MBF::getEffectiveAddress());

	int op = 0; // offset index
	long foffset[noffset];
	for(int i=0; i<noffset; i++) foffset[i]=0;

	bool foundit = false;
	int lineCount = 0;

	char s[256];
	char valString[20];
	char out[256];

	while(!feof(fp))
	{
		if(!foundit)
		{
			foffset[op] = ftell(fp);
			op = (op + 1) % noffset;
		}

			// Read a line of the source code list file.
		if(fgets(s, 256, fp))
		{
				// Get the address (CADR) from the line.
			strncpy(valString, s+startCol, colLen);
			valString[colLen]='\0';

				// 'foundit' is true after we have found the desired line.
			if(foundit)
			{
				if(strcmp(valString,CADR) == 0)
					cout << ">";
				else
					cout << " ";

					// truncate line so it fits in 80 col display
				strncpy(out, s, maxLineLen);
				out[maxLineLen] = '\0';
				cout << out;

				lineCount++;
				if(lineCount >= maxLines)
					break;
			}
			else
			{
				if(strcmp(valString, CADR) == 0)
				{
						// Reposition the file pointer back several lines so
						// we can see the code that preceeds the desired line, too.
					foundit = true;
					fseek(fp, foffset[op], 0);
				}
			}
		}
	}
	fclose(fp);
}



void main(int argc, char* argv[])
{
	bool singleClock = false;

	genAGCStates();
	MON::displayAGC();

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
			if(MON::FCLK || singleClock)
			{
					// This is a performance enhancement. If the AGC is running,
					// don't check the keyboard or simulator display every simulation
					// cycle, because that slows the simulator down too much.
				int genStateCntr = 100;
				do {
				    CLK::clkAGC(); 
					singleClock = false;

				    genAGCStates();
				    genStateCntr--;

						// This needs more work. It doesn't always stop at the
						// right location and sometimes stops at the instruction
						// afterwards, too.
					if(breakpointEnab && breakpoint == MBF::getEffectiveAddress())
					{
						MON::RUN = 0;
					}

						// Halt right after the instruction that changes a watched
						// memory location.
					if(watchEnab)
					{
						unsigned newWatchValue = MBF::readMemory(watchAddr);
						if(newWatchValue != oldWatchValue)
						{
							MON::RUN = 0;
						}
						oldWatchValue = newWatchValue;
					}


				} while (MON::FCLK && MON::RUN && genStateCntr > 0);

				updateAGCDisplay();

			}
			// for convenience, clear the single step switch on TP1; in the
			// hardware AGC, this happens when the switch is released
			if(MON::STEP && TPG::register_SG.read() == TP1) MON::STEP = 0;
		}
        char key = _getch();

	    // Keyboard controls for front-panel:
		switch(key)
		{
			// AGC controls
            // simulator controls

		case 'q': cout << "QUIT..." << endl; exit(0);
		case 'm': showMenu(); break;

		case 'd': 
			genAGCStates(); 
			MON::displayAGC(); 
			break; // update display

		case 'l': loadMemory(); break;
		case 'e': examineMemory(); break;

		case 'f':
			showSourceCode();
			break;

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
			//SCL::F17 = (SCL::F17 + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;

		case 'x':
			//SCL::F13 = (SCL::F13 + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;

		case 'r': 
			MON::RUN = (MON::RUN + 1) % 2; 
			genAGCStates();
			if(!MON::FCLK) MON::displayAGC();
			break;	

		case 's': 
			MON::STEP = (MON::STEP + 1) % 2; 
			genAGCStates();
			if(!MON::FCLK) MON::displayAGC();
			break;	

		case 'a': 
			MON::SA = (MON::SA + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;
			
		case 'n': 
			MON::INST = (MON::INST + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;	

		case 'p': 
			MON::PURST = (MON::PURST + 1) % 2;
			genAGCStates();
			MON::displayAGC();
			break;	

		case 'b':
			toggleBreakpoint();
			break;

		case 'y':
			toggleWatch();
			break;

		// DSKY:
		case '0': KBD::keypress(KEYIN_0); break;
		case '1': KBD::keypress(KEYIN_1); break;
		case '2': KBD::keypress(KEYIN_2); break;
		case '3': KBD::keypress(KEYIN_3); break;
		case '4': KBD::keypress(KEYIN_4); break;
		case '5': KBD::keypress(KEYIN_5); break;
		case '6': KBD::keypress(KEYIN_6); break;
		case '7': KBD::keypress(KEYIN_7); break;
		case '8': KBD::keypress(KEYIN_8); break;
		case '9': KBD::keypress(KEYIN_9); break;
		case '+': KBD::keypress(KEYIN_PLUS); break;
		case '-': KBD::keypress(KEYIN_MINUS); break;
		case '.': KBD::keypress(KEYIN_CLEAR); break;
		case '/': KBD::keypress(KEYIN_VERB); break;
		case '*': KBD::keypress(KEYIN_NOUN); break;
		case 'g': KBD::keypress(KEYIN_KEY_RELEASE); break;
		case 'h': KBD::keypress(KEYIN_ERROR_RESET); break;
		case 'j': KBD::keypress(KEYIN_ENTER); break;

		case '\0': // must be a function key
			key = _getch();
			switch(key)
			{
			case 0x3b: // F1: single clock pulse (when system clock off)
				singleClock = true; break;
		    case 0x3c: // F2: manual clock (FCLK=0)
				MON::FCLK = 0; genAGCStates(); MON::displayAGC(); break;
		    case 0x3e: // F4: fast clock (FCLK=1)
				MON::FCLK = 1; genAGCStates(); MON::displayAGC(); break;
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


change shiftData function to member function of reg.
on clk member function, AGC stops working if you clear masterVal. WHY?????


Issue: check all the FFs and registers that need to be cleared during GENRST.

Issue: Added 'D0' to the shift command to make MP work; this affected the WLP pulse
by changing the BX declaration for bit 14 to D0. Should I change the BX'es for all
others to D0 as well?

On kbd command polling, consider adding genAGCStates and displayAGC for counter inputs
and interrupts, too. Need to check this out, so we don't break it.

Change TP pulses to tpType (??)

Convert interrupt and counter cells to register objects so they can reset on GENRST
during WP.

Fix the breakpoint code. It doesn't work reliably.
*/