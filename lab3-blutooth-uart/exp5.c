/* Receive characters from phone using UART1 and send it to PC using UART0 */
#include "TM4C123GH6PM.h"
char UART1Rx(void);
void UART0Tx(char const c);

int main(void)
{
	SYSCTL->RCGCUART |= 0x3; /* provide clock to UART0, UART1 */
	SYSCTL->RCGCGPIO |= 1; /* enable clock to PORTA */
	SYSCTL->RCGCGPIO |= 0x22; /* enable clock to PORTF & B */ //0010 0000
	
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
	UART1->CTL = 0; /* disable UART0 */
	UART1->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART1->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART1->CC = 0; /* use system clock */
	UART1->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART1->IM |= 0x0010; /* enable RX interrupt */
	UART1->CTL = 0x301; /* enable UART0, TXE, RXE */
	
/* UART1 TX0 and RX0 use PB1 and PB0. Set them up. */
	GPIOB->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOB->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOB->PCTL = 0x11; /* configure PA0 and PA1 for UART */
 
/* configure Port F pins 3,2,1 to control the LEDs */
	GPIOF->DIR = 0x0E; /* configure Port F to control the LEDs */
	GPIOF->DEN = 0x0E;
	GPIOF->DATA = 0; /* turn LEDs off */
	
	/* enable interrupt in NVIC and set priority to 3 */
 NVIC->IP[6] = 3 << 5; /* set interrupt no 5 priority to 3 */
 NVIC->ISER[0] |= 0x00000040; /* enable IRQ5 for UART0 */ //0100 0000
 __enable_irq(); /* global enable IRQs */

 while(1){};



 return 0;
}
void UART1_Handler(void)
{
volatile int readback;
 char c;
 if (UART1->MIS & 0x0010) /* if a receive interrupt has occurred */
 {
 c = UART1->DR; /* read the received data */
	 UART0Tx(c);
 UART1->ICR = 0x0010; /* clear Rx interrupt flag */
 readback = UART1->ICR; /* a read to force clearing of interrupt flag */
 }
 else
 {
 /* should not get here. But if it does, */
 UART1->ICR = UART1->MIS; /* clear all interrupt flags */
 readback = UART1->ICR; /* a read to force clearing of interrupt flag */
 }
}


void UART0Tx(char const c)
{
	while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
	UART0->DR = c; // Write byte
		if(c == 'r'){
			GPIOF->DATA = 2;
		}
		if(c == 'g'){
			GPIOF->DATA = 8;
		}
		if(c == 'b'){
			GPIOF->DATA = 4;
		}
}
