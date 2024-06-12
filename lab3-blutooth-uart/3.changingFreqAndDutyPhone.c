/* Receive characters from phone using UART1 and send it to PC using UART0 */
#include "TM4C123GH6PM.h"
#include <stdio.h>
#include <string.h>
char UART1Rx(void);
void UART0Tx(char const c);
char buffer[10];
int ind = 0;
int flag2 = 1;
int flag = 0;
volatile int freq = 30;
volatile float duty = 0.5;
int main(void)
{
	SYSCTL->RCGCUART |= 0x3; /* provide clock to UART0, UART1 */
	//SYSCTL->RCGCUART |= 0xff; /* provide clock to UART0, UART1 */

	SYSCTL->RCGCUART |= 0x20; /* provide clock to UART5 */ 
	SYSCTL->RCGCGPIO |= 1; /* enable clock to PORTA */
	SYSCTL->RCGCGPIO |= 0x22; /* enable clock to PORTF & B */ //0010 0000
	SYSCTL->RCGCGPIO |= 0x10;
	SYSCTL->RCGCGPIO |= 0x28;

/* UART0 initialization */
	UART0->CTL = 0; /* disable UART0 */
	UART0->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART0->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART0->CC = 0; /* use system clock */
	UART0->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART0->CTL = 0x301; /* enable UART0, TXE, RXE */

/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */
	
/* UART1 initialization, enabling RX interrupt */
	UART5->CTL = 0; /* disable UART0 */
	UART5->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART5->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART5->CC = 0; /* use system clock */
	UART5->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART5->IM |= 0x0010; /* enable RX interrupt */
	UART5->CTL = 0x301; /* enable UART0, TXE, RXE */
	
/* UART1 TX0 and RX0 use PB1 and PB0. Set them up. */
	GPIOE->DEN = 0x30; /* Make PA0 and PA1 as digital */ 
	GPIOE->AFSEL = 0x30; /* Use PA0,PA1 alternate function */
	GPIOE->PCTL = 0x00110000; /* configure PA0 and PA1 for UART */
 
/* configure Port F pins 3,2,1 to control the LEDs */
	GPIOD->DIR = 0x40;
	GPIOD->DEN = 0x40;
	GPIOF->DIR = 0x0E; /* configure Port F to control the LEDs */
	GPIOF->DEN = 0x0E;
	GPIOF->DATA = 0; /* turn LEDs off */
	
	/* enable interrupt in NVIC and set priority to 3 */
 NVIC->IP[6] = 3 << 5; /* set interrupt no 5 priority to 3 */
  NVIC->IP[61] = 3 << 5; /* set interrupt no 5 priority to 3 */
 NVIC->ISER[1] |= 0x20000000; /* enable IRQ5 for UART0 */ //0100 0000
  NVIC->ISER[0] |= 0x00000040; /* enable IRQ5 for UART0 */ //0100 0000
 
 
 //SysTick->LOAD = (15999/(freq))* duty; /* reload with number of clocks per second */
 SysTick->LOAD = 16000;
  SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */

	GPIOD->DATA =  0x00; 
 
 __enable_irq(); /* global enable IRQs */

 while(1){
	if(!flag2){
		if(strcmp("freq+1", buffer)){
			freq+=1;
			GPIOF->DATA = 2;
			flag2 =1;
		}
		if(strcmp("freq+5", buffer)){
			freq+=5;
			GPIOF->DATA = 2;
			flag2 =1;
		}
		
		if(strcmp("freq-1", buffer)){
			if(freq>2){
				freq-=1;
				GPIOF->DATA = 2;
				flag2 =1;
			}
		}
		if(strcmp("freq-5", buffer)){
			if(freq>=10){
				freq-=5;
				GPIOF->DATA = 2;
				flag2 =1;
			}
		}
		
		if(strcmp("duty+10", buffer)){
			if(duty<0.8){
			duty+=0.1;
			GPIOF->DATA = 2;
			flag2 =1;
			}
		}
		if(strcmp("duty+5", buffer)){
			if(duty<0.9){
			duty+=0.05;
			GPIOF->DATA = 2;
			flag2 =1;
			}
		}
		
		if(strcmp("duty-10", buffer)){
			if(duty>0.2){
				duty-=0.1;
				GPIOF->DATA = 2;
				flag2 =1;
			}
		}
		if(strcmp("duty-5", buffer)){
			if(duty>0.2){
				duty-=0.05;
				GPIOF->DATA = 2;
				flag2 =1;
			}
		}
		for (int i = 0; i < sizeof(buffer); i++) { 
  buffer[i] = '\0'; // set each element to null 
			ind = 0;
} 
	}
	
 };



 return 0;
}
void UART5_Handler(void)
{
	flag2 = 1;
	volatile int readback;
	char c;
	if (UART5->MIS & 0x0010) /* if a receive interrupt has occurred */
	{
		c = UART5->DR; /* read the received data */
		if(c!='!'){
			buffer[ind] = c;
			ind++;
		}
		else{
			ind = 0;
			flag2 = !flag2;
		}
		UART0Tx(c);
		UART5->ICR = 0x0010; /* clear Rx interrupt flag */
		readback = UART5->ICR; /* a read to force clearing of interrupt flag */
	}
	else
	{
		/* should not get here. But if it does, */
		UART5->ICR = UART5->MIS; /* clear all interrupt flags */
		readback = UART5->ICR; /* a read to force clearing of interrupt flag */
	}
}


void UART0Tx(char const c)
{
	while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
	UART0->DR = c; // Write byte
}


void SysTick_Handler(void)
{
	if(flag)
	{
		SysTick->LOAD = ((16000*duty/freq))-1; /* reload with number of clocks per second */
		GPIOD->DATA = 0x40; 
		flag = 0;
	}
	else
	{
		SysTick->LOAD = ((16000*(1-duty)/freq))-1; /* reload with number of clocks per second */
		GPIOD->DATA = 0x00; 
		flag = 1;
	}
		
	//flag = !flag;
}
