/****************************************************************************
 *  AGC4 (Apollo Guidance Computer) BLOCK I Simulator
 *
 *  AUTHOR:     John Pultorak
 *  DATE:       02/23/02
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
 *
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
	for(unsigned i=address; i<address+6; i++)
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
	if(MON::CLKRATE == FCLK)
	{
		if(MON::RUN)
		{
				// update every 2 seconds at the start of a new instruction
			if(displayTimeout)
			{
				clockCounter++;
				if(
					(TPG::register_SG.read() == TP12 && SEQ::register_SNI.read() == 1) || 
					(TPG::register_SG.read() == STBY) ||
					clockCounter > 500)
				{
					MON::displayAGC();
					displayTimeout = false;
					clockCounter = 0;
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

void main(int argc, char* argv[])
{
	// TO TEST THE OVERFLOW HANDLING IN TIME1
	//MBF::writeMemory(035,037700); // ************ REMOVE *************
	
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
			if((MON::CLKRATE != MCLK) || singleClock)
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

				} while (MON::CLKRATE == FCLK && MON::RUN && genStateCntr > 0);

				updateAGCDisplay();


				if(MON::CLKRATE == SCLK)
					delay(1);
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

		case 'd': genAGCStates(); MON::displayAGC(); break; // update display

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
			if(MON::CLKRATE != FCLK) MON::displayAGC();
			break;	

		case 's': 
			MON::STEP = (MON::STEP + 1) % 2; 
			genAGCStates();
			if(MON::CLKRATE != FCLK) MON::displayAGC();
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

		case 'c': 
			MON::BREQ = (MON::BREQ + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;

		case 'p': 
			MON::PURST = (MON::PURST + 1) % 2;
			genAGCStates();
			MON::displayAGC();
			break;	

		case 'w': 
			MON::SWRST = (MON::SWRST + 1) % 2; 
			genAGCStates();
			MON::displayAGC();
			break;

		case 'b':
			toggleBreakpoint();
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
			case 0x3c: // F2: manual clocking (MCLK)
				MON::CLKRATE = MCLK; genAGCStates(); MON::displayAGC(); break;
			case 0x3d: // F3: slow clock (SCLK)
				MON::CLKRATE = SCLK; genAGCStates(); break;
		    case 0x3e: // F4: fast clock (FCLK)
				MON::CLKRATE = FCLK; genAGCStates(); MON::displayAGC(); break;
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
Implement BGRANT signal; (I think I removed this)

Implement alarms


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