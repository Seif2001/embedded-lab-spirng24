#include "TM4C123GH6PM.h"
void delayMs(int n);
unsigned int color = 0;
unsigned int flag = 1;
float duty = 0.5;
unsigned int colorArr[] = {0x08, 0x06}; // 0000 1000
int freq = 30;
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
	
	/* configure PORTF4, 0 for falling edge trigger interrupt */
 GPIOF->IS &= ~0x11; /* make bit 4, 0 edge sensitive */ 
 GPIOF->IBE &= ~0x11; /* trigger is controlled by IEV */ 
 GPIOF->IEV &= ~0x11; /* falling edge trigger */  //
 GPIOF->ICR |= 0x11; /* clear any prior interrupt */ 
 GPIOF->IM |= 0x11; /* unmask interrupt for PF4,PF0 */ 
 /* enable interrupt in NVIC and set priority to 3 */
	


 
	
 NVIC->IP[30] &=  ~0xff; /*no need to shift because its 0 set interrupt priority to 0 */
 NVIC->ISER[0] |= 0x40000000; /* enable IRQ30 (D30 of ISER[0]) */ //0100
 
 	 /* Configure SysTick */
 SysTick->LOAD = 15999*duty; /* reload with number of clocks per second */
 SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */
 
 __enable_irq(); /* global enable IRQs */

 /* toggle the green/violet LED colors continuously */
 while(1)
 {

 }
}
/* SW1 is connected to PF4 pin, SW2 is connected to PF0. */
/* Both of them trigger PORTF interrupt */
void GPIOF_Handler(void)
{
	if(GPIOF->MIS == 0x01){
		if(duty<0.9){
			duty+=0.05;

		}
	}
	if(GPIOF->MIS == 0x10){ 
		if(duty>0.1){
			duty-=0.05;
		}
	}


 volatile int readback;
 GPIOF->ICR |= 0x11; /* clear the interrupt flag before return */
 readback = GPIOF->ICR; /* a read to force clearing of interrupt flag */
}


void SysTick_Handler(void)
{
	SysTick->LOAD = flag? 15999*(1.0-duty) : 15999*(duty); /* reload with number of clocks per second */
	GPIOD->DATA = flag ?0x40 : 0x00; 
	flag = !flag;
}
