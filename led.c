#include "mkl25z4.h"
#include "led.h"
#include "millis.h"
#include "bluetooth.h"

#define MASK(x)	(1UL << x)
#define ON_BOARD_RED (18) //PTB 18
#define ON_BOARD_GREEN (19) //PTB 19
#define ON_BOARD_BLUE (1) //PTD 1
#define TAIL_RED (8) //PTB8
#define TAIL_WHITE (9) //PTB9
#define BUZZER (10) //PTB10

static unsigned long led_current_time =0u;
static unsigned long rbbbr_last_run=0u;
static unsigned long buzzer_last_run=0u;
static unsigned long beep_twice_last_run=0u;
static unsigned long point2_sec=200; //200ms
static unsigned long point5_sec=500; //200ms

static int cur_stop = 0;
static int prev_stop = 0;
	
/*Initializes pots and pins to be used on board*/
void initialize_leds(){
	configure_systick();
	//clock gating to port B & D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK; 

	//set up pins as GPIO
	PORTB->PCR[ON_BOARD_RED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[ON_BOARD_RED] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTB->PCR[ON_BOARD_GREEN] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[ON_BOARD_GREEN] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTD->PCR[ON_BOARD_BLUE] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[ON_BOARD_BLUE] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTB->PCR[TAIL_RED] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[TAIL_RED] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTB->PCR[TAIL_WHITE] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[TAIL_WHITE] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PORTB->PCR[BUZZER] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTB->PCR[BUZZER] |= PORT_PCR_MUX(1);	//setup to be GPIO


	//set up outputs
	PTB->PDDR |= MASK(ON_BOARD_RED);
	PTB->PDDR |= MASK(ON_BOARD_GREEN);
	PTD->PDDR |= MASK(ON_BOARD_BLUE);
	PTB->PDDR |= MASK(TAIL_RED);
	PTB->PDDR |= MASK(TAIL_WHITE);
	PTB->PDDR |= MASK(BUZZER);

	//put the LEDs in a known state (off for active low config)
	PTB->PSOR =MASK(ON_BOARD_RED);	//turn off ON_BOARD_RED
	PTB->PSOR =MASK(ON_BOARD_GREEN);	//turn off ON_BOARD_GREEN
	PTD->PSOR =MASK(ON_BOARD_BLUE);	//turn off ON_BOARD_BLUE
}


/*flash pattern RBBBR*/
void RBBBR()
{
	led_current_time = millis();
	if(led_current_time - rbbbr_last_run < point2_sec)
	{
		PTB->PCOR=MASK(ON_BOARD_RED); 	//on
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 2*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 3*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PCOR=MASK(ON_BOARD_BLUE); 	//on
	}
	else if(led_current_time - rbbbr_last_run < 4*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 5*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PCOR=MASK(ON_BOARD_BLUE); 	//on
	}
	else if(led_current_time - rbbbr_last_run < 6*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 7*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PCOR=MASK(ON_BOARD_BLUE); 	//on
	}
	else if(led_current_time - rbbbr_last_run < 8*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 9*point2_sec)
	{
		PTB->PCOR=MASK(ON_BOARD_RED); 	//on
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else if(led_current_time - rbbbr_last_run < 10*point2_sec)
	{
		PTB->PSOR=MASK(ON_BOARD_RED); 	//off
		PTB->PSOR=MASK(ON_BOARD_GREEN); 	//off
		PTD->PSOR=MASK(ON_BOARD_BLUE); 	//off
	}
	else
	{
		rbbbr_last_run = led_current_time;
	}
}

void  on_tail_red()
{
	PTB->PSOR=MASK(TAIL_RED); 	//on
	PTB->PCOR=MASK(TAIL_WHITE); 	//off
}
void  on_tail_white()
{
	PTB->PSOR=MASK(TAIL_WHITE); 	//on
	PTB->PCOR=MASK(TAIL_RED); 	//off
}
void  off_tail_light()
{
	PTB->PCOR=MASK(TAIL_WHITE); 	//off
	PTB->PCOR=MASK(TAIL_RED); 	//off
}

void reverse_beep()
{
	led_current_time = millis();
	if(led_current_time - buzzer_last_run < point5_sec)
	{
		PTB->PSOR=MASK(BUZZER); 	//on
	}
	else if(led_current_time - buzzer_last_run < 2*point5_sec)
	{
		PTB->PCOR=MASK(BUZZER); 	//off
	}
	else
	{
		buzzer_last_run = led_current_time;
	}
}

void off_buzzer()
{
	PTB->PCOR=MASK(BUZZER); 	//off
}
void beep_twice()
{
	cur_stop = get_stop();
	enum states{S1,S2,S3,S4,S5,S6};
	static enum states s = S1;
	volatile int change_state = 0;
	
	led_current_time = millis();
	if(led_current_time - beep_twice_last_run >= point2_sec)
	{
		change_state = 1;
		beep_twice_last_run = led_current_time;
	}
	
	if(prev_stop != cur_stop)
	{
		switch(s)
		{
			case S1:
				PTB->PSOR=MASK(BUZZER); 	//on
				if(change_state)
				{
					s = S2;
					change_state = 0;
				}
				break;
			case S2:
				PTB->PCOR=MASK(BUZZER); 	//off
				if(change_state)
				{
					s = S3;
					change_state = 0;
				}
				break;
				case S3:
				PTB->PSOR=MASK(BUZZER); 	//on
				if(change_state)
				{
					s = S4;
					change_state = 0;
				}
				break;
			case S4:
				PTB->PCOR=MASK(BUZZER); 	//off
				if(change_state)
				{
					s = S5;
					change_state = 0;
				}
				break;
			case S5:
				PTB->PSOR=MASK(BUZZER); 	//on
				if(change_state)
				{
					s = S6;
					change_state = 0;
				}
				break;
			case S6:
				PTB->PCOR=MASK(BUZZER); 	//off
				//if(change_state)
				//{
				//	s = S1;
				//	change_state = 0;
				//	prev_stop = cur_stop;
				//}
				//break;
			
				s = S1;
				change_state = 0;
				prev_stop = cur_stop;
			default:
				s=S1;
				break;
		}
	}
}

