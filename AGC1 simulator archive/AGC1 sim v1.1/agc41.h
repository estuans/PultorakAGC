// AGC header

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

char* instructionString[] =
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

//-------------------------------------------------------------
// ADDRESSES

enum specialRegister { // octal addresses of special registers
		// Flip-Flop registers
	A_ADDR		=00,
	Q_ADDR		=01,
	Z_ADDR		=02,
	LP_ADDR		=03,
	IN0_ADDR	=04,
	IN1_ADDR	=05,
	IN2_ADDR	=06,
	IN3_ADDR	=07,
	OUT0_ADDR	=010,
	OUT1_ADDR	=011,
	OUT2_ADDR	=012,
	OUT3_ADDR	=013,
	OUT4_ADDR	=014,
	BANK_ADDR	=015,

		// No bits in these registers
	RELINT_ADDR	=016,
	INHINT_ADDR	=017,

		// In eraseable memory
	CYR_ADDR	=020,
	SR_ADDR		=021,
	CYL_ADDR	=022,
	SL_ADDR		=023,
	ZRUPT_ADDR	=024,
	BRUPT_ADDR	=025,
	ARUPT_ADDR	=026,
	QRUPT_ADDR	=027,

		// 030 - 056: counter registers (p. 5-31)
		// MYSTERY: what are 0030 - 0033 ????????
	OVCTR_ADDR	=0034,

	TIME1_ADDR	=0035,
	TIME2_ADDR	=0036,
	TIME3_ADDR	=0037,
	TIME4_ADDR	=0040,

	UPLINK_ADDR	=0041,

	OUTCR1_ADDR	=0042,
	OUTCR2_ADDR	=0043,

	PIPAX_ADDR	=0044,
	PIPAY_ADDR	=0045,
	PIPAZ_ADDR	=0046,

	CDUX_ADDR	=0047,
	CDUY_ADDR	=0050,
	CDUZ_ADDR	=0051,

	OPTX_ADDR	=0052,
	OPTY_ADDR	=0053,

	TRKRX_ADDR	=0054,
	TRKRY_ADDR	=0055,
	TRKRZ_ADDR	=0056
};

const int GOPROG	=02000;	// bottom address of fixed memory

enum ruptAddress {
		// Addresses for service routines of vectored interrupts
	T3RUPT_ADDR		=02004,	// option 1: overflow of TIME 3
	ERRUPT_ADDR		=02010,	// option 2: error signal
	DSRUPT_ADDR		=02014,	// option 3: telemetry end pulse or TIME 4 overflow
	KEYRUPT_ADDR	=02020,	// option 4: activity from MARK, keyboard, or tape reader
	UPRUPT_ADDR		=02024,	// option 5: UPLINK overflow
};

enum keyInType {
	// DSKY keyboard input codes: Taken from E-1574, Appendix 1
	// These codes enter the computer through bits 1-5 of IN0.
	// The MSB is in bit 5; LSB in bit 1. Key entry generates KEYRUPT.
	KEYIN_NONE			=0,		// no key depressed**
	KEYIN_0				=020,
	KEYIN_1				=001,
	KEYIN_2				=002,
	KEYIN_3				=003,
	KEYIN_4				=004,
	KEYIN_5				=005,
	KEYIN_6				=006,
	KEYIN_7				=007,
	KEYIN_8				=010,
	KEYIN_9				=011,
	KEYIN_VERB			=021,
	KEYIN_ERROR_RESET	=022,
	KEYIN_KEY_RELEASE	=031,
	KEYIN_PLUS			=032,
	KEYIN_MINUS			=033,
	KEYIN_ENTER			=034,
	KEYIN_CLEAR			=036,
	KEYIN_NOUN			=037,
};

