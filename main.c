#include <EFM8LB1.h>
#include <stdio.h>
#include "lcd.h"

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

void main (void)
{
	unsigned long F;
	char Fstr[17];
	TIMER0_Init();
	waitms(500);
	printf("\x1b[23");
	printf ("EFM8 Frequency measurement using Timer/Counter 0.\n"
	        "File: %s\n"
	        "Compiled: %s, %s\n\n",
	        __FILE__, __DATE__, __TIME__);

	// Configure the LCD
	LCD_4BIT();

	LCDprint("Frequency (Hz):", 1, 1);
	while(1)
	{
		TL0=0;
		TH0=0;
		overflow_count=0;
		TF0=0;
		TR0=1; // Start Timer/Counter 0
		waitms(1000);
		TR0=0; // Stop Timer/Counter 0
		F=overflow_count*0x10000L+TH0*0x100L+TL0;
		printf("\rf=%luHz", F);
		printf("\x1b[0K"); // ANSI: Clear from cursor to end of line.
		sprintf(Fstr, "%lu", F);
		LCDprint(Fstr, 2, 1);
	}
}
