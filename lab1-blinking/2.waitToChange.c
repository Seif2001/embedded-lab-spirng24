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
	int curr = 0;
 // enable clock to GPIOF at clock gating control register
	SYSCTL->RCGCGPIO |= 0x20;
		GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR = 0b1;
 // enable the GPIO pins for the LED (PF3, 2 1) as output
	GPIOF->DIR = 0x0e;
 // enable the GPIO pins for digital function
	GPIOF->DEN = 0x1f;
	GPIOF->PUR = 0b10001;

	int useCurr = 0;
	int isOn = 0;
	
	while(1)
	{	
			int sw1 = 0;
			int sw2 = 0;
			for(int j = 0; j<2; j++){
				if((GPIOF->DATA & 0b10000) == 0){
					sw1 = 1;
					delayMs(500);
					changeColor(useCurr);
					delayMs(500);

					if(!isOn){
					GPIOF->DATA = 0;	
					}
				}
				else{
					sw1 = 0;
					break;
				}
			}
			for(int j = 0; j<2; j++){
				if((GPIOF->DATA & 0b00001) == 0){
					sw2 = 1;
					delayMs(500);
					changeColor(useCurr);
					delayMs(500);

					if(!isOn){
					GPIOF->DATA = 0;	
					}
				}
				else{
					sw2 = 0;
					break;
				}
			}
			if (sw1){
				useCurr = changeColor(useCurr);
			}
			else{
				changeColor(useCurr);
			}
		
			delayMs(500);

			if(!isOn){
					GPIOF->DATA = 0;	
			}
			delayMs(500);
					
			if (sw2){
				isOn = isOn?0:1;
			}
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
void delayMs(int n)
{
	for(int i =0; i< n; i++){
		SysTick->LOAD= 15999;
		SysTick->CTRL = 0x5;
		while((SysTick->CTRL & 0x10000) == 0) 
		{ }
		SysTick->CTRL = 0;
	}
}