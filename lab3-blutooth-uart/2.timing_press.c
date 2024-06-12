/* Receive characters from phone using UART1 and send it to PC using UART0 */
#include "TM4C123GH6PM.h"
void UART0Tx(char const c);
int timer = 0;
int flag = 0;

int main(void)
{		
		SYSCTL->RCGCGPIO |= 0x20; /* enable clock to PORTF */ //0010 0000
		SYSCTL->RCGCUART |= 0x1; /* provide clock to UART0, UART1 */
		SYSCTL->RCGCGPIO |= 1;
		GPIOF->LOCK = 0x4C4F434B;
		GPIOF->CR = 0b1;
	
		
	 // enable the GPIO pins for the LED gpiof as input and gpiod7 as output
		GPIOF->DIR = 0x00;
	 // enable the GPIO pins for digital function and pullup resistance for pin f0 
		GPIOF->DEN = 0x01;
		GPIOF->PUR = 0x01;
		
		/* configure PORTF4, 0 for falling edge trigger interrupt */
	 GPIOF->IS &= ~0x01; /* make bit 4, 0 edge sensitive */ 
	 GPIOF->IBE |= 0x01; /* trigger is controlled by bofa */ 
	 GPIOF->ICR |= 0x01; /* clear any prior interrupt */ 
	 GPIOF->IM |= 0x01; /* unmask interrupt for PF4,PF0 */ 
	 /* enable interrupt in NVIC and set priority to 3 */
	
	
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
	 	SysTick->LOAD = 15999; /* reload with number of clocks per second */
		SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */
		
		/* enable interrupt in NVIC and set priority to 3 */
		
		NVIC->IP[6] = 3 << 5; /* set interrupt no 5 priority to 3 */
		NVIC->IP[30] = 3 << 5; 
		NVIC->ISER[0] |= 0x40000040; /* enable IRQ5 for UART0 */ //0100 0000
		__enable_irq(); /* global enable IRQs */

	 while(1){};



	 return 0;
}

void UART0Tx(char c)
{
	while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
	UART0->DR = c; // Write byte
}


void GPIOF_Handler(void)
{    
	if(GPIOF->MIS & 0x01){
		if(!flag){
			timer = 0;
		}
	
	else{
			char timer_str[10];
			sprintf(timer_str,"%d \r\n", timer);
			for(int i = 0; timer_str[i]!='\0'; i++){
				UART0Tx(timer_str[i]);
			}
	}
			flag=!flag;

}
	

 volatile int readback;
 GPIOF->ICR |= 0x11; /* clear the interrupt flag before return */
 readback = GPIOF->ICR; /* a read to force clearing of interrupt flag */
}


void SysTick_Handler(void)
{
	SysTick->LOAD = 15999; /* reload with number of clocks per second */
	timer++;
}