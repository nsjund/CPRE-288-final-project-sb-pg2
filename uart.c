/*
*
*   uart.c
* Description: This is file is meant for those that would like a little
*              extra help with formatting their code, and followig the Datasheet.
*/

#include "uart.h"
#include "Timer.h"

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R  |= 0b00000010;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R  |= 0b00000010;      // enable clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R |= 0b11;      // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R  |= 0x11;       // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R   |= 0b00000011;        // enables pb0 and pb1
    GPIO_PORTB_DIR_R   |= 0b00000010;        // sets pb0 as output, pb1 as input

    //compute baud values [UART clock= 16 MHz] 
    int    fbrd;
    int    ibrd;
	
	// baud rate = 16 MHz / (16 * 115200)
	

    fbrd = 44; // page 903
    ibrd = 8;

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = (UART1_IBRD_R & 0xFFFF0000) | (ibrd & 0x0000FFFF);        // write integer portion of BRD to IBRD
    UART1_FBRD_R = (UART1_FBRD_R & 0xFFFF0000) | (fbrd & 0x0000FFFF);    // write fractional portion of BRD to FBRD
    UART1_LCRH_R = (UART1_LCRH_R & 0xFFFFFF60) | 0b01100000;        // write serial communication parameters (page 916) * 8bit and no parity
	// Double check since there might be 2 stop bits or 1?? idk lol
    UART1_CC_R   &= 0xFFFFFFF0;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x01;        // enable UART1

}

void uart_sendChar(char data)
{
	
	//wait here as long as the FIFO is full
   while(UART1_FR_R & 0b00100000) {
   }
   
   UART1_DR_R = data;
   
}

char uart_receive(void)
{
	char data = 0;
	
	// keep waiting as long as FIFO is empty
	
	while (UART1_FR_R & 0b00010000){
	}
	
	data = (char)(UART1_DR_R & 0xFF);
	
	return data;
 
}

void uart_sendStr(const char *data)
{
    while (*data != '\0'){
	uart_sendChar(*data);
	data++;
	}
	
}

// _PART3


void uart_interrupt_init()
{
	//turn off uart1 while we set it up
	UART1_CTL_R &= 0xFFFFFFFE;
	
	//clear interrupt flags by wrtiing a 1 to ICR to clear RX
	
	UART1_ICR_R = UART1_ICR_R | 0b00010000;
	
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0b000010000; //enable interrupt on receive - page 924
	
	//set priority of usart1 interrupt: example to 1. group 1 bits 21-23
	
	NVIC_PRI1_R |= 0x00200000;

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0b01000000; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104
    IntRegister(INT_UART1, UART1_Handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number
	
	IntMasterEnable();
	
	UART1_CTL_R = UART1_CTL_R | 0x1;
	
	// may need to change but i think this should work
}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrup Status

//STEP2:  Copy the data 

//STEP3:  Clear the interrup   

}

void UART1_Handler(void){
	
	if (UART1_MIS_R & 0x10) {
		
		
		// clear the receive byte interrupt by writing a 1 to ICR
		
	    recived = uart_receive();
	    //recived = (char)(UART1_DR_R);
		
		UART1_ICR_R = UART1_ICR_R | 0x10;
		
		//do something
		
		
	}
	//check if a transmit byte IRQ has occured
	
	else if (UART1_MIS_R & 0x20){
		
		
	
	// clear the transmit byte interrupt

		
		
	UART1_ICR_R = UART1_ICR_R | 0x20;
	
	
	}
		
		
	flag = 1;
	

}
