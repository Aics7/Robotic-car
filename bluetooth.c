#include <mkl25z4.h>
#include <string.h>
#include "queues.h"
#include "bluetooth.h"

#define RX	1		//PTA1
#define TX	2		//PTA2
#define OSR 15		//over sample rate (like a pre-scaler)
#define BAUD_RATE  	9600	//my communication rate on BT
#define SYS_CLOCK	20971520u	//

//car directions
volatile int forward = 0, reverse = 0, left = 0, right = 0, stop = 1, fast = 0,slow = 0, accelerate = 0,automatic = 0;

#define MASK(x) (1UL << (x))

volatile char rxChar;

Q_T TxQ, RxQ;

void init_pins(void){
	//Clock gate port A
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1);
	PORTA->PCR[RX] &= ~PORT_PCR_MUX_MASK;  	//clear mux
	PORTA->PCR[RX] |=  PORT_PCR_MUX(2); 	//set for UART0 RX
	PORTA->PCR[TX] &= ~PORT_PCR_MUX_MASK;	//clear
	PORTA->PCR[TX] |=  PORT_PCR_MUX(2); 	//set for UART0 TX
}

void Init_UART(void){
	//select clock for uart0 (disabled by default), MCGFLLCLK/system clk as UART0 clock 
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	// clock gate UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//clock gate UART0
	//compute set baud rate (SBR), choosing baud rate of 9600 for BT
	uint8_t sbr = (uint16_t)((SYS_CLOCK)/((OSR+1) *BAUD_RATE ));	//default OSR is 15, 	sbr=136.5 if SYS_CLOCK =20971520u
	//UART0->BDH |=((sbr>>8) & 0x1F);	//generic. set only bottom 5 bits
	UART0->BDH =0;			//0x0 for this calculation, other fields are default 0. 
	UART0->BDL=sbr;			//0x88 for this calculation
	// Rx Interrupt enabled, Tx & RX enable
	UART0->C2  |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;
	//note: default is 8N1 if uart0->C1=0

	NVIC_SetPriority(UART0_IRQn, 3);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}

void Init_UART_fuller(void){
	//select clock for uart0 (disabled by default), MCGFLLCLK as UART0 clock (system??)
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	// clock gate UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;		//clock gate UART0
	//set baud rate (SBR), choosing baud rate of 9600
	uint8_t sbr = (uint16_t)((SYS_CLOCK)/((OSR+1) *BAUD_RATE ));	//default OSR is 15, sbr=136.5 if SYS_CLOCK =20971520u

	//UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK; //disable TX and Rx before configuring
	//configure 8bits, no parity  **(1 stop bit) //default settings
	//UART0->C1  = 0x00;		//8 bits, no parity
	//UART0->BDH &= ~UART0_BDH_SBNS_MASK;		// 1Stop Bit Number Select (alt is 2)
	//set baud rate
	UART0->BDH=(sbr>>8);	//0;
	UART0->BDL=sbr;			//0x88 for this calculation


	// Configure Tx/Rx Interrupts
	//UART0->C2  &= ~UART_C2_TIE_MASK;  // Tx Interrupt disabled, can also do |=UART_C2_TIE(0);
	//UART0->C2  &= ~UART_C2_TCIE_MASK; // Tx Complete Interrupt disabled, can also do |=UART_C2_TCIE(0)

	// Rx Interrupt enabled, Tx & RX enable
	UART0->C2  |= UART_C2_RIE_MASK | UART_C2_TE_MASK | UART_C2_RE_MASK;


	NVIC_SetPriority(UART0_IRQn, 3);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}


void UART0_IRQHandler(void) {
	uint8_t ch;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK |
		UART_S1_FE_MASK | UART_S1_PF_MASK)) {
			// clear the error flags
			UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
			// read the data register to clear RDRF
			ch = UART0->D;
	}
	if (UART0->S1 & UART0_S1_RDRF_MASK) {
		// received a character
		ch = UART0->D;
		rxChar=ch;		//to enable me take some action
		if (!Q_Full(&RxQ)) {
			Q_Enqueue(&RxQ, ch);
		} else {
			// error - queue full, discard character
		}
	}
	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
			(UART0->S1 & UART0_S1_TDRE_MASK) ) { // tx buffer empty
		// can send another character
		if (!Q_Empty(&TxQ)) {
			UART0->D = Q_Dequeue(&TxQ);
		} else {
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}
}



void Send_String(uint8_t * str) {
	// enqueue string
	while (*str != '\0') { // copy characters up to null terminator
		while (Q_Full(&TxQ))
			; // wait for space to open up
		Q_Enqueue(&TxQ, *str);
		str++;
	}
	// start transmitter if it isn't already running
	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->D = Q_Dequeue(&TxQ);
		UART0->C2 |= UART0_C2_TIE(1);
	}
}

void doTxRx_task(){
	uint8_t buffer[80], c, * bp;
	if (Q_Size(&RxQ) == 0)
		return;	//nothing received. Exit
	//lightLED();
	//echo character sent:
	c = Q_Dequeue(&RxQ);
	sprintf((char *) buffer, "Command %c", c);
	// decode instruction and queue string
	stop = forward = reverse = right = left = fast = slow = accelerate = automatic = 1;
	if(strcmp(buffer,"Command 0")==0) 
	{//stop
		stop = 1;
		forward = 0;
		reverse = 0;
		right = 0;
		left = 0;
		fast = 0;
		slow = 0;
		accelerate = 0;
		automatic = 0;
	}
	
	else if(strcmp(buffer,"Command 1")==0)
	{//forward
		forward = 1;
		stop = 0;
		reverse = 0;
		right = 0;
		left = 0;
		automatic = 0;
	}
	
	else if(strcmp(buffer,"Command 2")==0)
	{//reverse
		reverse = 1;
		stop = 0;
		forward = 0;
		right = 0;
		left = 0;
		automatic = 0;
	}
	
	else if(strcmp(buffer,"Command 3")==0)
	{//right
		right = 1;
		stop = 0;
		forward = 0;
		reverse = 0;
		left = 0;
		fast = 0;
		slow = 0;
		accelerate = 0;
		automatic = 0;
	}
	
	else if(strcmp(buffer,"Command 4")==0)
	{//left
		left = 1;
		stop = 0;
		forward = 0;
		reverse = 0;
		right = 0;
		fast = 0;
		slow = 0;
		accelerate = 0;
		automatic = 0;
	}
	
	else if(strcmp(buffer,"Command 5")==0)
	{//fast
		fast = 1;
		stop = 0;
		right = 0;
		left = 0;
		slow = 0;
		accelerate = 0;
		automatic = 0;
		
		reverse = 0;
		forward = 0;
	}
	
	else if(strcmp(buffer,"Command 6")==0)
	{//slow
		slow = 1;
		stop = 0;
		right = 0;
		left = 0;
		fast = 0;
		slow = 0;
		accelerate = 0;
		automatic = 0;
		
		reverse = 0;
		forward = 0;
	}
	
	else if(strcmp(buffer,"Command 7")==0)
	{//accelerate
		accelerate = 1;
		stop = 0;
		right = 0;
		left = 0;
		fast = 0;
		slow = 0;
		automatic = 0;
		
		reverse = 0;
		forward = 0;
	}
	
	else if(strcmp(buffer,"Command 8")==0)
	{//automatic
		automatic = 1;
		stop = 0;
		forward = 0;
		reverse = 0;
		right = 0;
		left = 0;
		fast = 0;
		slow = 0;
		accelerate = 0;
	}
	//queues data
	bp = buffer;
	while (*bp != '\0') {	//enqueue full string
		while (Q_Full(&TxQ)) ; // wait if buffer full
		Q_Enqueue(&TxQ, *bp);
		bp++;
	}
	// start transmitter if it isn't already running
	if (!(UART0->C2 & UART0_C2_TIE_MASK)) {
		UART0->C2 |= UART0_C2_TIE(1);
	}

}

void bluetooth_init()
{
	Q_Init(&TxQ);
	Q_Init(&RxQ);
}
char get_bluetooth_message()
{
	return rxChar;
}


int get_forward()
{
	return forward;
}
int get_reverse()
{
	return reverse;
}
int get_left()
{
	return left;
}
int get_right()
{
	return right;
}
int get_stop()
{
	return stop;
}
int get_fast()
{
	return fast;
}
int get_slow()
{
	return slow;
}
int get_accelerate()
{
	return accelerate;
}
int get_automatic()
{
	return automatic;
}


void set_forward()
{
	forward = 1;
}
void set_reverse()
{
	reverse = 1;
}
void set_left()
{
	left = 1;
}
void set_right()
{
	right = 1;
}
void set_stop()
{
	stop= 1;
	forward = 0;
	reverse = 0;
	right = 0;
	left = 0;
	fast = 0;
	slow = 0;
	accelerate = 0;
	automatic = 0;
}
void set_fast()
{
	fast = 1;
}
void set_slow()
{
	slow = 1;
}
void set_accelerate()
{
	accelerate = 1;
}
void set_automatic()
{
	automatic = 1;
}

