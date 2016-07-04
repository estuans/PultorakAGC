# PultorakAGC
Pultorak Block I Apollo Guidance Computer Assembler and Simulator

This project is an effort to make the code cross-platform and compile easily on "modern" environments.
See the README.TXT for the original, unfettered verbiage.

The "AGC1 simulator archive" contains 15 versions of Pultorak's C++ AGC block 1 simulator.
Version 15 is the latest.

All C++ software was compiled with g++.

  SIMULATOR VERSIONS:
    1.0 - initial version.
    1.1 - fixed minor bugs; passed automated test and checkout programs:
            teco1.asm, teco2.asm, and teco3.asm to test basic instructions,
            extended instructions, and editing registers.
    1.2 - decomposed architecture into subsystems; fixed minor bug in DSKY
            keyboard logic (not tested in current teco*.asm suite).
            Implemented scaler pulses F17, F13, F10. Tied scaler output to
            involuntary counters and interrupts. Implemented counter overflow
            logic and tied it to interrupts and other counters. Added simple
            set/clear breakpoint. Fixed a bug in bank addressing.
    1.3 - fixed bugs in the DSKY. Added 14-bit effective address (CADR) to the
            simulator display output. Inhibited interrupts when the operator
            single-steps the AGC.
    1.4 - performance enhancements. Recoded the control pulse execution code
            for better simulator performance. Also changed the main loop so it
            polls the keyboard and system clock less often for better performance.
    1.5 - reversed the addresses of TIME1 and TIME2 so TIME2 occurs first.
            This is the way its done in Block II so that a common routine (READLO)
            can be used to read the double word for AGC time.
    1.6 - added indicators for 'CHECK FAIL' and 'KEY RELS'. Mapped them to OUT1,
            bits 5 and 7. Added a function to display the current location in
            the source code list file using the current CADR.
    1.7 - increased length of 'examine' function display. Any changes in DSKY now
            force the simulator to update the display immediately. Added a 'watch'
            function that looks for changes in a memory location and halts the
            AGC. Added the 'UPTL', 'COMP', and "PROG ALM" lights to the DSKY.
    1.8 - started reorganizing the simulator in preparation for H/W logic design.
            Eliminated slow (1Hz) clock capability. Removed BUS REQUEST feature.
            Eliminated SWRST switch.
    1.9 - eliminated the inclusive 'OR' of the output for all registers onto the
            R/W bus. The real AGC OR'ed all register output onto the bus; normally
            only one register was enabled at a time, but for some functions several
            were simultaneously enabled to take advantage of the 'OR' function (i.e.:
            for the MASK instruction). The updated logic will use tristate outputs
            to the bus except for the few places where the 'OR' function is actually
            needed. Moved the parity bit out of the G register into a 1-bit G15
            register. This was done for convenience because the parity bit in G
            is set independently from the rest of the register.
    1.10 - moved the G15 parity register from MBF to the PAR subsystem. Merged SBFWG
            and SBEWG pulses into a single SBWG pulse. Deleted the CLG pulse for MBF
            (not needed). Separated the ALU read pulses from all others so they can
            be executed last to implement the ALU inclusive OR functions. Implemented
            separate read and write busses, linked through the ALU. Implemented test
            parity (TP) signal in PAR; added parity alarm (PALM) FF to latch PARITY
            ALARM indicator in PAR.
    1.11 - consolidated address testing signals and moved them to ADR. Moved memory
            read/write functions from MBF to MEM. Merged EMM and FMM subsystems into
            MEM. Fixed a bad logic bug in writeMemory() that was causing the load of
            the fixed memory to overwrite array boundaries and clobber the CPM table.
            Added a memory bus (MEM_DATA_BUS, MEM_PARITY_BUS).
    1.12 - reduced the number of involuntary counters (CTR) from 20 to 8. Eliminated
            the SHINC subsequence. Changed the (CTR) sequence and priority registers into
            a single synchronization register clocked by WPCTR. Eliminated the fifth
            interrupt (UPRUPT; INT). Eliminated (OUT) the signal to read from output
            register 0 (the DSKY register), since it was not used and did not provide
            any useful function, anyway. Deleted register OUT0 (OUT) which shadowed
            the addressed DSKY register and did not provide any useful function.
            Eliminated the unused logic that sets the parity bit in OUT2 for downlink
            telemetry.
    1.13 - reorganized the CPM control pulses into CPM-A, CPM-B, and CPM-C groups.
            Added the SDV1, SMP1, and SRSM3 control pulses to CPM-A to indicate when
            those subsequences are active; these signals are input to CPM-C. Moved the
            ISD function into CPM-A. Fixed a minor bug causing subsequence RSM3 to be
            displayed as RSM0. Added GENRST to clear most registers during STBY.
    1.14 - Moved CLISQ to TP1 to fix a problem in the hardware AGC. CLISQ was clearing
            SNI on CLK2 at TP12, but the TPG was advancing on CLK1 which occurs after
            CLK2, so the TPG state machine was not seeing SNI and was not moving to
            the correct state. In this software simulation, everything advances on
            the same pulse, so it wasn't a problem to clear SNI on TP12. Added a
            switch to enable/disable the scaler.
    1.15 - Reenabled interrupts during stepping (by removing MON::RUN) signals from
            CPM-A and CPM-C logic). Interrupts can be prevented by disabling the scaler.
            Fixed a problem with INHINT1; it is supposed to prevent an interrupt
            between instructions if there's an overflow. It was supposed to be cleared
            on TP12 after SNI (after a new instruction), but was being cleared on TP12
            after every subsequence.


========================================================================================

The "AGC1 assembler archive" contains 6 versions of the Pultorak C++ AGC block 1 assembler.
Version 6 is the latest, of course. The assembler produces object code readable by the
simulator.


  ASSEMBLER VERSIONS:
	1.0 - First version of the AGC assembler.
	1.1 - Added ability to handle simple arithmetic expressions for the operand.
	1.2 - Changed label fields to 14 char. Printed symbol table in 3 columns.
                Corrected wrong implementation of OVSK.
	1.3 - Added support for bank addressing.
	1.4 - Added capability to inline code with nested includes.
	1.5 - Added CADR, ADRES assembler directives. Swapped addresses for
                TIME1 and TIME2 for compatibility with Block II PINBALL routines.
	1.6 - Fixed the parser so it doesn't mangle comments anymore. Added
                FCADR and ECADR to the assembler directives. Added a check for
                assembled code that overwrites already used locations.
                Fixed 'agcMemSize' so it includes the last word of fixed memory.
