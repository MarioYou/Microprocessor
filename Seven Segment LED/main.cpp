#include "mbed.h"

DigitalOut data (p30); //A and B inputs synchronized
DigitalOut clr  (p28); //Clear the display
DigitalOut clk  (p27); //Clock for controlling data

void loadSeq	(int seq[17][8]);
void loadBits	(int bits[8]);
void blink		(int bits[8]);

int main()
{
int sequence[17][8] = 
{{0,0,0,0,0,0,1,1}, //0
 {1,0,0,1,1,1,1,1}, //1
 {0,0,1,0,0,1,0,1}, //2
 {0,0,0,0,1,1,0,1}, //3
 {1,0,0,1,1,0,0,1}, //4
 {0,1,0,0,1,0,0,1}, //5
 {0,1,0,0,0,0,0,1}, //6
 {0,0,0,1,1,1,1,1}, //7
 {0,0,0,0,0,0,0,1}, //8
 {0,0,0,1,1,0,0,1}, //9
 {0,0,0,1,0,0,0,1}, //A
 {1,1,0,0,0,0,0,1}, //B
 {0,1,1,0,0,0,1,1}, //C
 {1,0,0,0,0,1,0,1}, //D
 {0,1,1,0,0,0,0,1}, //E
 {0,1,1,1,0,0,0,1}, //F
 {1,1,1,1,1,1,1,0}}; //.
clr = 1;
loadSeq(sequence);
}

void loadSeq(int seq[17][8])
{
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 16; j++)
		{
			for(int k = 0; k < 16; k++)
			{
				loadBits(seq[k]);
				loadBits(seq[j]);
				loadBits(seq[i]);
				wait(0.1);
				if(i == 16)
				{
					blink(seq[i]);
				}
			}
		}
	}
}
void loadBits(int bits[8])
{
	for(int i = 7; i >= 0; i--)
	{
		data = bits[i];
		clk = 1;
		clk = 0;
	}
}

void blink(int bits[8])
{
	int empty[8] = {1,1,1,1,1,1,1,1};
	while(1)
	{
		loadBits(empty);
		wait(0.5);
		loadBits(bits);
		wait(0.5);
	}
}

