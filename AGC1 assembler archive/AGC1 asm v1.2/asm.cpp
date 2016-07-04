/*
 ****************************************************************
 *
 * Cross Assembler for Block I Apollo Guidance Computer (AGC4)
 *
 * 10/20/01
 *
 *****************************************************************

  Versions:
	1.0	-	First version of the AGC assembler.
	1.1 -	Added ability to handle simple arithmetic expressions for the operand.
	1.2 -	Changed label fields to 14 char. Printed symbol table in 3 columns.
			Corrected wrong implementation of OVSK.

  Sources:
	TBD.

Operation:
	The assembler reads an AGC source code file (having a .asm extension).
	It generates an assembly listing text file (.lst extension) and an
	object code text file (.obj extension). The object code file is readable
	by the AGC simulator.

Syntax:
	Source code files are text files containing multiple lines of source code.
	Each line is terminated by a newline character. Source code files can be
	produced by any editor, as long as it doesn't insert any hidden characters
	or formatting information.

	Each line of assembly code consists of one of the following:
		a) a blank line;
		b) a comment (comments must begin with a semicolon (;));
		c) or a line of assembler code.

	The assembler ignores blank lines and anything that occurs after a semicolon
	on any given line.

	A line of assembler code consists of the following components:
		1) a label (this is optional);
		2) an op code or assembler directive;
		3) an operand (also optional);
		4) a comment (optional; comments must start with a semicolon)

	The components, if present, must appear in the order given. Each component is
	separated from the next by one or more white spaces or tabs. The only constraint
	is that the label, if present, must start in the 1st column of the line. If no
	label is present, the op code or assembler directive must not start in the 1st
	column, but may appear in any subsequent column.

	The operand may consist of one of the following:
		a) an asterisk (the assembler substitutes in the current value of the
			location counter;
		b) a symbolic name (the assemble substitutes in the value during the
			second pass of the assembly;
		c) a numeric constant. Octal contants must be preceeded by a '%';
			hexadecimal constants are preceeded by a '$'; anything else
			is treated as a decimal.
		d) an expression consisting of one or more of the above, separated by
		    the operators: +, -, @ (for multiplication), or / (for division).
			Unary plus or minus is also allowed.
			examples: 
				*+2			means location counter plus 2
				LABEL+%10/2
				-5

Errata:
	The assembler ignores the last line of the source (.asm) file. If the last
	line of your source file contains code, you must add an additional blank
	line to the end of your source file to ensure that your last line of code
	is assembled.

	The symbol table should be sorted before the second pass. The linear
	search through the symbol table should be replaced by a more efficient
	method.

	The assembler directives and syntax don't match those of the original block 
	I AGC.

	The assembler can't detect invalid (undefined) op codes or assembler
	directives. Currently, it silently skips over them.

	A macro definition capability would be handy.

	There's no way to include or link multiple multiple source code files or
	multiple object files into a single file. However, this can be done
	manually in the AGC simulator by loading two or more separate object files.

	The assembler does not check for ORG directives that overwrite previously
	assembled code.

	The assembler listing strips whitespace out of comments, which destroys any
	fancy formatted comments.
*/

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream.h>
#include <stdio.h>

unsigned pass = 0;
unsigned locCntr = 0;
unsigned errorCount = 0;

FILE* fp = 0;		// input
FILE* fpList = 0;	// output (assembly listing)
FILE* fpObj = 0;	// output (object code)


struct ocode
{
	char* name;
	unsigned code;
	unsigned len; // words
};

ocode allcodes[] = 
{ 
		// Block I op codes.
	{ "TC",		0000000, 1 }, 
	{ "CCS",	0010000, 1 }, 
	{ "INDEX",  0020000, 1 }, 
	{ "XCH",	0030000, 1 }, 
	{ "CS",		0040000, 1 }, 
	{ "TS",		0050000, 1 }, 
	{ "AD",		0060000, 1 }, 
	{ "MASK",	0070000, 1 }, 
	{ "MP",		0040000, 1 }, 
	{ "DV",		0050000, 1 }, 
	{ "SU",		0060000, 1 },

		// Implied address codes (R393: 3-12)
	{ "RESUME",	0020025, 1 },	// INDEX 25
	{ "EXTEND",	0025777, 1 },	// INDEX 5777
	{ "INHINT",	0020017, 1 },	// INDEX 17
	{ "RELINT",	0020016, 1 },	// INDEX 16
	{ "XAQ",	0000000, 1 },	// TC A
	{ "RETURN",	0000001, 1 },	// TC Q
	{ "NOOP",	0030000, 1 },	// XCH A
	{ "COM",	0040000, 1 },	// CS A
	{ "TCAA",	0050002, 1 },	// TS Z
	{ "OVSK",	0050000, 1 },	// TS A
	{ "DOUBLE",	0060000, 1 },	// AD A
	{ "SQUARE",	0040000, 1 },	// MP A

		// For "clarity" (R393: 3-12)
	{ "TCR",	0000000, 1 },	// same as TC; subroutine call with return
	{ "CAF",	0030000, 1 },	// same as XCH; address to fixed acts like Clear and Add
	{ "OVIND",	0050000, 1 },	// same as TS

		// Assembler directives
	{ "DS", 0, 1 },		// define storage; reserves 1 word of memory
	{ "ORG", 0, 0 },	// origin; sets the location counter to the operand value
	{ "EQU", 0, 0 },	// equate; assigns a value to a label
	{ "", 0, 99 }		// end-of-list flag
};

void parse(char* buf, char* labl, char* opcd, char* opnd, char* cmnt)
{
		// strip off newline char.
	buf[strlen(buf) - 1] = '\0';

		// replace any horizontal tabs with spaces
	for(unsigned i=0; i<strlen(buf); i++) 
	{
		if(buf[i] == '\t') buf[i] = ' ';
	}

	strcpy(labl,""); 
	strcpy(opcd,""); 
	strcpy(opnd,""); 
	strcpy(cmnt,"");

	char* sp = buf;
	char* s = 0;

	enum {_labl=0, _opcd, _opnd, _cmnt } mode = _labl;


	if(buf[0] == ' ') mode = _opcd;
	do
	{
		s = strtok(sp, " "); sp = 0;
		if(s)
		{
			if(s[0] == ';') mode = _cmnt;
			switch(mode)
			{
			case _labl: strcat(labl, s); mode = _opcd; break;
			case _opcd: strcat(opcd, s); mode = _opnd; break;
			case _opnd: strcat(opnd, s); mode = _cmnt; break;
			case _cmnt: strcat(cmnt, s); strcat(cmnt, " "); break;
			}
		}
	} while(s);
}

struct symbl
{
	char name[20];
	unsigned val;
};


symbl symTab[5000];
unsigned nSym = 0;

	// Pre-defined symbols corresponding to architectural
	// conventions in the block I AGC4. 
symbl constSymTab[] = 
{
	{ "A",			00	},
	{ "Q",			01	},
	{ "Z",			02	},
	{ "LP",			03	},
	{ "IN0",		04	},
	{ "IN1",		05	},
	{ "IN2",		06	},
	{ "IN3",		07	},
	{ "OUT0",		010	},
	{ "OUT1",		011	},
	{ "OUT2",		012	},
	{ "OUT3",		013	},
	{ "OUT4",		014	},
	{ "BANK",		015	},
	{ "CYR",		020	},
	{ "SR",			021	},
	{ "CYL",		022	},
	{ "SL",			023	},
	{ "ZRUPT",		024	},
	{ "BRUPT",		025	},
	{ "ARUPT",		026	},
	{ "QRUPT",		027	},
	{ "OVCTR",		034 },
	{ "GOPROG",		02000	},	
	{ "T3RUPT",		02004	},
	{ "ERRUPT",		02010	},
	{ "DSRUPT",		02014	},
	{ "KEYRUPT",	02020	},
	{ "UPRUPT",		02024	},
	{ "EXTENDER",	05777	},
	{ "",			0	},
};

void add(char* labl, unsigned value)
{
		// Check whether symbol is already defined.
	unsigned i;
	for(i=0; i<nSym; i++)
	{
		if(strcmp(symTab[i].name, labl)==0)
		{
			fprintf(fpList,"*** ERROR: %s redefined.\n", symTab[i].name);
			errorCount++;
			return;
		}
	}

		// Add new symbol to symbol table
	strcpy(symTab[nSym].name, labl);
	symTab[nSym].val = value;
	nSym++;
}

// Return the int value of the operand string. The string is
// assumed to be a simple value (not an expression)
int _getopnd(char* opnd)
{
	if(strlen(opnd)==0)
		return 0;
	else if(opnd[0] == '$') // hex number
		return strtol(opnd+1, 0, 16);
	else if(opnd[0] == '%') // octal number
		return strtol(opnd+1, 0, 8);
	else if(isdigit(opnd[0])) // decimal number
		return strtol(opnd, 0, 10);
	else if(opnd[0] == '*')
		return locCntr;
	else // must be label; look up value
	{
		unsigned i;
		for(i=0; i<nSym; i++)
		{
			if(strcmp(symTab[i].name, opnd)==0)
				return symTab[i].val;
		}

		// Not there, so check whether symbol is an
		// assembler-defined constant. If so, copy it to
		// the user-defined symbols.
		for(i=0; strcmp(constSymTab[i].name,"") != 0; i++)
		{
			if(strcmp(constSymTab[i].name, opnd)==0)
			{
				strcpy(symTab[nSym].name, opnd);
				symTab[nSym].val = constSymTab[i].val;
				nSym++;
				return constSymTab[i].val;
			}
		}

		if(pass == 1)
		{
			fprintf(fpList,"*** ERROR: %s undefined.\n", opnd);
			errorCount++;
		}
	}
	return 0;
}

// returns pointer to new position in istr
char* getToken(char* istr, char* ostr)
{
	*ostr = '\0';

		// bump past any whitespace
	while(*istr == ' ') istr++;

	if(*istr == '\0') return istr;

	bool keepGoing = true;
	do
	{
		*ostr = *istr;
		if(*ostr == '+' || *ostr == '-' || *ostr == '@' || *ostr == '/')
			keepGoing = false;
		ostr++; istr++;
	}
	while(keepGoing && *istr != '\0' && *istr != '+' && *istr != '-'
		&& *istr != '@' && *istr != '/');

	*ostr = '\0';
	return istr;
}

int _eval(char* sp, int tot)
{
	if(*sp == '\0') return tot;

	char op[20];
	sp = getToken(sp, op);

	char vstr[20];
	int val = 0;
	sp = getToken(sp,vstr);
	if(*vstr =='-') // unary minus
	{
		sp = getToken(sp, vstr);
		val = -(_getopnd(vstr));
	}
	else
		val = _getopnd(vstr);

	switch(*op)
	{
	case '+': tot += val; break;
	case '-': tot -= val; break;
	case '@': tot *= val; break;
	case '/': tot /= val; break;
	}

	return _eval(sp,tot);
}

int eval(char* sp)
{
	char op[20];
	getToken(sp, op);
	char sp1[80];
	if(*op != '+' && *op != '-')
		strcpy(sp1,"+");
	else
		strcpy(sp1,"");

	strcat(sp1, sp);
	return _eval(sp1, 0);
}

// Return the value of the operand string. The string may
// be a simple token or an expression consisting of multiple
// tokens and operators. Evaluation occurs from left to right;
// no parenthesis allowed. Unary minus is allowed and correctly
// evaluated. Valid operators are +, -, @, and /. The @ operator
// is multiplication (the traditional * operator already is used
// to refer to the location counter.
unsigned getopnd(char* opnd)
{
	//return _getopnd(opnd); // the old call did not allow for expressions

	unsigned retval = 0;
	if(strcmp(opnd,"-0") == 0 || strcmp(opnd,"-%0") == 0 || strcmp(opnd,"-$0") == 0)
		retval = 077777;	// -0
	else
	{		// return the int value of the operand
		int opndVal = eval(opnd);

			// now, convert the number into 16-bit signed AGC format
		if(opndVal < 0)
		{
				// convert negative values into AGC 16-bit 1's C form.
			opndVal = 077777 + opndVal;
			if(opndVal < 0)
			{
				fprintf(fpList,"*** ERROR: %s underflowed.\n", opnd);
				errorCount++;
				opndVal = 0;
			}
		}
		else if(opndVal > 077777)
		{
			fprintf(fpList,"*** ERROR: %s overflowed.\n", opnd);
			errorCount++;
			opndVal = 0;
		}
		retval = (unsigned) opndVal;
	}
	return retval;
}

unsigned getopcode(char* opcd)
{
	for(int j=0; allcodes[j].len != 99; j++)
	{
		if(strcmp(allcodes[j].name, opcd) == 0)
		{
			return allcodes[j].code;
		}
	}
	fprintf(fpList,"*** ERROR: %s undefined.\n", opcd);
	errorCount++;

	return 0;
}

unsigned getoplen(char* opcd)
{
	for(int j=0; allcodes[j].len != 99; j++)
	{
		if(strcmp(allcodes[j].name, opcd) == 0)
		{
			return allcodes[j].len;
		}
	}
	return 0;
}

void updateLocCntr(char* opcd, char* opnd)
{
	unsigned size = 0;
	for(int i=0; allcodes[i].len != 99; i++)
	{
		if(strcmp(allcodes[i].name, opcd) == 0)
		{
			size = allcodes[i].len; break;
		}
	}
	locCntr += size;

	if(strcmp(opcd,"ORG") == 0)
	{
		locCntr = getopnd(opnd);
	}
}

unsigned genOddParity(unsigned r)
{
	//check the lower 15 bits of 'r' and return the odd parity
	unsigned evenParity =
		(1&(r>>0))  ^ (1&(r>>1))  ^ (1&(r>>2))  ^ (1&(r>>3))  ^
		(1&(r>>4))  ^ (1&(r>>5))  ^ (1&(r>>6))  ^ (1&(r>>7))  ^
		(1&(r>>8))  ^ (1&(r>>9))  ^ (1&(r>>10)) ^ (1&(r>>11)) ^
		(1&(r>>12)) ^ (1&(r>>13)) ^ (1&(r>>14));
	return ~evenParity & 1; // odd parity
}

void main(int argc, char* argv[])
{
	cout << "AGC Block I assembler" << endl;

		// The assembler reads an assembly source code file
		// with a .asm extension; i.e.: myProg.asm
		// It writes an assembly listing text file with
		// a .lst extension (myProg.lst) and an object code
		// text file with a .obj extension (myProg.obj)

#ifdef NOTDEF
	// use this to enter the source file using command line
	if(argc != 2)
	{
		cout << "*** ERROR: source file name not specified." << endl;
		exit(-1);
	}

	fp = fopen(argv[1], "r");
#endif

	char sourcefile[80];
	cout << "Enter source file: ";
	cin >> sourcefile;

		// Valid source files have a .asm extension; strip the
		// extension off so we can use the prefix for the list
		// and object files.
	char prefix[80];
	strcpy(prefix, sourcefile);

	char* p = prefix;
	while(*p != '\0') { p++; if(*p == '.') break; }
	if(strcmp(p,".asm") != 0)
	{
		cerr << "*** ERROR: Source file not *.asm" << endl;
		exit(-1);
	}
	*p = '\0';

		// Open the source code file.
	fp = fopen(sourcefile, "r");
	if(!fp)
	{
		perror("fopen failed for source file");
		exit(-1);
	}

		// Open a text file for the assembly listing. The filename
		// will have a .lst extension.
	char listfile[80];
	sprintf(listfile, "%s.lst", prefix);
	fpList = fopen(listfile, "w");
	if(!fpList)
	{
		perror("fopen failed for assembly list file");
		exit(-1);
	}

		// Open a text file for the object code. The filename
		// will have a .obj extension.
	char objfile[80];
	sprintf(objfile, "%s.obj", prefix);
	fpObj = fopen(objfile, "w");
	if(!fpObj)
	{
		perror("fopen failed for object file");
		exit(-1);
	}

	char buf[256];
	char labl[100];	// label
	char opcd[100];	// op code
	char opnd[100];	// operand
	char cmnt[100];	// comment

	fprintf(fpList,"Block I Apollo Guidance Computer (AGC4) assembler version 1.2\n\n");

	fprintf(fpList,"First pass: generate symbol table.\n");
	while(fgets(buf, 256, fp))
	{
		parse(buf, labl, opcd, opnd, cmnt);

		if(strlen(labl)>0)
		{
			if(strcmp(opcd,"EQU")==0)
				add(labl, getopnd(opnd));
			else
				add(labl, locCntr);
		}
		updateLocCntr(opcd, opnd);
	}

	rewind(fp);
	locCntr = 0;
	unsigned data;
	pass++;

	fprintf(fpList,"Second pass: generate object code.\n\n");
	while(fgets(buf,256,fp))
	{
		parse(buf, labl, opcd, opnd, cmnt);


		if(strcmp(opcd,"")==0)
			fprintf(fpList, "              %s\n", cmnt);
		else if(getoplen(opcd) == 0)
			fprintf(fpList, "              %-14s %-8s %-14s %s\n",
				labl, opcd, opnd, cmnt);
		else
		{
			data = getopcode(opcd) + getopnd(opnd);
			data |= genOddParity(data) << 15;
			fprintf(fpList, "%06o %06o %-14s %-8s %-14s %s\n",
				locCntr, data, labl, opcd, opnd, cmnt);
			fprintf(fpObj, "%06o %06o\n",
				locCntr, data);

		}

		updateLocCntr(opcd, opnd);
	}
	fclose(fp);
	fclose(fpObj);

	fprintf(fpList,"\nAssembly complete. Errors = %d\n", errorCount);

	fprintf(fpList,"\nSymbol table:\n");

	unsigned j=0;
	for(unsigned i=0; i<nSym; i++)
	{
		fprintf(fpList,"%-14s %06o   ", symTab[i].name, symTab[i].val);
		j = (j+1) % 3;
		if(j==0) fprintf(fpList,"\n");
	}
	fclose(fpList);
}
