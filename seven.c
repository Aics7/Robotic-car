#include "mkl25z4.h"
#include "seven.h"
#include "millis.h"
#define MASK(x)	(1UL << x)

//defining pins to map with ports
#define a (7) //PTC7
#define b (0) //PTC0
#define c (3) //PTC3
#define d (4) //PTC4
#define e (5) //PTC5
#define f (6) //PTC6
#define g (10) //PTC10

#define D1 (11) //PTC11
#define D2 (12) //PTC12
#define D3 (13) //PTC13
#define D4 (16) //PTC16

static unsigned long seven_current_time = 0u;
static unsigned long seven_last_run = 0u;
static unsigned long point001_sec=1; //2ms

/*Initializes pots and pins to be used on board*/
void initialize_seven_segment(){
	//clock gating to port C
	SIM->SCGC5 |=SIM_SCGC5_PORTC_MASK ;

	configure_systick();
	
	//set up pins as GPIO
	PORTC->PCR[a] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[a] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[b] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[b] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[c] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[c] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[d] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[d] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[e] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[e] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[f] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[f] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[g] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[g] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[D1] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[D1] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[D2] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[D2] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[D3] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[D3] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTC->PCR[D4] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTC->PCR[D4] |= PORT_PCR_MUX(1);	//setup to be GPIO

	//set up outputs
	PTC->PDDR |= MASK(a);
	PTC->PDDR |= MASK(b);
	PTC->PDDR |= MASK(c);
	PTC->PDDR |= MASK(d);
	PTC->PDDR |= MASK(e);
	PTC->PDDR |= MASK(f);
	PTC->PDDR |= MASK(g);
	PTC->PDDR |= MASK(D1);
	PTC->PDDR |= MASK(D2);
	PTC->PDDR |= MASK(D3);
	PTC->PDDR |= MASK(D4);
	
	//makes digits high
	PTC->PSOR=MASK(D1);
	PTC->PSOR=MASK(D2);
	PTC->PSOR=MASK(D3);
	PTC->PSOR=MASK(D4);
}

void display_digits(int number)
{
	enum states{S1,S2,S3,S4};
	static enum states s = S1;
	volatile int change_state = 0;
	
	seven_current_time = millis();
	if(seven_current_time - seven_last_run >= point001_sec)
	{
		change_state = 1;
		seven_last_run = seven_current_time;
	}
	
	switch(s)
	{
		case S1:
			PTC->PCOR=MASK(D1);
			display(number/1000);
			PTC->PSOR=MASK(D1);
			if(change_state)
			{
				s=S2;
				change_state = 0;
			}
			break;
		case S2:
			PTC->PCOR=MASK(D2);
			display((number%1000)/100);
			PTC->PSOR=MASK(D2);
			if(change_state)
			{
				s=S3;
				change_state = 0;
			}
			break;
		case S3:
			PTC->PCOR=MASK(D3);
			display((number%100)/10);
			PTC->PSOR=MASK(D3);
			if(change_state)
			{
				s=S4;
				change_state = 0;
			}
			break;
		case S4:
			PTC->PCOR=MASK(D4);
			display(number%10);
			PTC->PSOR=MASK(D4);
			if(change_state)
			{
				s=S1;
				change_state = 0;
			}
			break;
		default:
			s=S1;
			break;
	}
	
	/*
	seven_current_time = millis();
	if(seven_current_time - seven_last_run < point002_sec)
	{
	PTC->PCOR=MASK(D1);
	display(number/1000);
	PTC->PSOR=MASK(D1);
	}
	else if(seven_current_time - seven_last_run < 2*point002_sec)
	{
	PTC->PCOR=MASK(D2);
	display((number%1000)/100);
	PTC->PSOR=MASK(D2);
	}
	else if(seven_current_time - seven_last_run < 3*point002_sec)
	{
	PTC->PCOR=MASK(D3);
	display((number%100)/10);
	PTC->PSOR=MASK(D3);
	}
	else if(seven_current_time - seven_last_run < 4*point002_sec)
	{
	PTC->PCOR=MASK(D4);
	display(number%10);
	PTC->PSOR=MASK(D4);
	}
	else
	{
		seven_last_run = seven_current_time;
	}
	*/
}

void display(int x)
{
	switch(x)
	{
		case 0: 
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PSOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PCOR=MASK(g);
			break;
		case 1: 
			PTC->PCOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PCOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PCOR=MASK(f);
			PTC->PCOR=MASK(g); 
			break;
		case 2: 
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PCOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PSOR=MASK(e);
			PTC->PCOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		case 3:
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PCOR=MASK(f);
			PTC->PSOR=MASK(g); 
			break;
		case 4:
			PTC->PCOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PCOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		case 5:
			PTC->PSOR=MASK(a);
			PTC->PCOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		case 6:
			PTC->PSOR=MASK(a);
			PTC->PCOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PSOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		case 7:
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PCOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PCOR=MASK(f);
			PTC->PCOR=MASK(g);
			break;
		case 8:
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PSOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		case 9:
			PTC->PSOR=MASK(a);
			PTC->PSOR=MASK(b);
			PTC->PSOR=MASK(c);
			PTC->PSOR=MASK(d);
			PTC->PCOR=MASK(e);
			PTC->PSOR=MASK(f);
			PTC->PSOR=MASK(g);
			break;
		default:
			break;
	}
}

