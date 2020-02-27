#include <EFM8LB1.h>
#include <stdio.h>
#include <math.h>
#include "lcd.h"

// Values of resistors in slide 6
#define Ra 2000
#define Rb 2000

unsigned char overflow_count;

int getsn (char * buff, int len)
{
	int j;
	char c;
	
	for(j=0; j<(len-1); j++)
	{
		c=getchar();
		if ( (c=='\n') || (c=='\r') )
		{
			buff[j]=0;
			return j;
		}
		else
		{
			buff[j]=c;
		}
	}
	buff[j]=0;
	return len;
}

void TIMER0_Init(void)
{
	TMOD&=0b_1111_0000; // Set the bits of Timer/Counter 0 to zero
	TMOD|=0b_0000_0101; // Timer/Counter 0 used as a 16-bit counter
	TR0=0; // Stop Timer/Counter 0
}

const char* SIprefix(int exp)
{
	switch(exp)
	{
		case 3:
			return "m";
		case 6:
			return "u";
		case 9:
			return "n";
		case 12:
			return "p";
		default:
			return "?";
	}
}

void main (void)
{
	unsigned long F;
	float C; // Capacitance
	int exp=0;
	char Fstr[17];
	char Cstr[17];

	TIMER0_Init();
	waitms(500);
	printf("\x1b[2J"); // Clear screen using ANSI escape sequence.
	printf("\x1b[;f"); // Reset cursor position
	printf ("EFM8 Frequency measurement using Timer/Counter 0.\n"
	        "File: %s\n"
	        "Compiled: %s, %s\n\n",
	        __FILE__, __DATE__, __TIME__);

	// Configure the LCD
	LCD_4BIT();

	while(1)
	{
		exp=0;
		TL0=0;
		TH0=0;
		overflow_count=0;
		TF0=0;
		TR0=1; // Start Timer/Counter 0
		waitms(200);
		TR0=0; // Stop Timer/Counter 0

		F=overflow_count*0x10000L+TH0*0x100L+TL0;
		// Solve for capacitance based on frequency using formula in slide 6
		C = 1.44/(5*(Ra+2*Rb)*F);

		// convert to SI notation - find prefix value
		while(C<1)
		{
			exp+=3;
			C*=1000;
		}

		// print frequency and calculated capacitance to PuTTY
		printf("\rf=%luHz, c=%f%sF", F, C, SIprefix(exp));
		printf("\x1b[0K"); // ANSI: Clear from cursor to end of line.

		// print frequency and calculated capacitance to LCD
		sprintf(Fstr, "%luHz", F);
		sprintf(Cstr, "%f%sF", C, SIprefix(exp));
		LCDprint(Fstr, 1, 1);
		LCDprint(Cstr, 2, 1);
	}
}
