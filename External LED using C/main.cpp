#include "mbed.h"

AnalogOut myExtLED(p18);
//DigitalIn myIntSwitch(p23);
//DigitalIn myExtSwitch(p33);

int main()
{	
	
	while(1)
	{ 
		double k;
		for(k=0.5;k<=1;k+=0.01)
		{
			myExtLED=k;
			wait(0.2);
		}
		for(k=1;k>=0.5;k-=0.01)
		{
			myExtLED=k;
			wait(0.2);
		}
	} 
}
