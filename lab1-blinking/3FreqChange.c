/* This program blinks the red LED on the
* TI Tiva LaunchPad. The connections are:
* PF1 - red LED
* PF2 - blue LED
* PF3 - green LED
* They are high active (a '1' turns on the LED).
*/
#include "TM4C123GH6PM.h"
void delayMs(int n);
int changeColor(int color);
int main(void)
{
 // enable clock to GPIOF at clock gating control register
	SYSCTL->RCGCGPIO |= 0x28;
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR = 0b1;
 // enable the GPIO pins for the LED gpiof as input and gpiod7 as output
	GPIOF->DIR = 0x0;
	GPIOD->DIR = 0x40;
 // enable the GPIO pins for digital function and pullup resistance for pin f0 and pinf4
	GPIOF->DEN = 0x11;
	GPIOF->PUR = 0b10001;
	
	GPIOD->DEN = 0x40;

	int freq = 30;
	float duty = 0.5;
	int flag = 0;
	int flag2 = 0;
	while(1)
	{	
			
			if((GPIOF->DATA & 0b10000) == 0){				//sw1
				if(!flag){
					flag = 1;
					freq+=1;
				}
			}
			else{
				flag = 0;
			}
			
			if((GPIOF->DATA & 0b00001) == 0){ //sw2
					if(freq>1 && !flag2){
						flag2 = 1;
						freq-=1;
						
					}
			}
			else{
				flag2 = 0;
			}
			GPIOD->DATA=0;
			delayMs(freq*(duty));
			GPIOD->DATA=0x40;
			delayMs(freq*(1-duty));
			
		
			
	}
}

//GPIOlock GPIOcommitcontrol

int changeColor(int color){
	if (color == 0){
		GPIOF->DATA = 0x08;
		return 1;
	}
	else if (color == 1){
		GPIOF->DATA = 0x02;
		return 2;
	}
	else{
		GPIOF->DATA = 0x04;
		return 0;
	}
}

// delay in milliseconds (16 MHz CPU clock)
void delayMs(int freq)
{
		SysTick->LOAD= 15999/(unsigned int)freq;
		SysTick->CTRL = 0x5;
		while((SysTick->CTRL & 0x10000) == 0) 
		{ }
		SysTick->CTRL = 0;
}