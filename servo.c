#include <MKL25Z4.h>
#include "servo.h"
#define SERVO		(3)		//PTD3
#define MASK(X)  (1<<X)
static int pulse_width = 0xF6; //1.5ms (forward by default)

void init_servo_timer()
{
	//Clock gate
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;	//*******TPM0 channel 0
	//Select clock source in SIM_SOPT
	//SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);	//1- MCGPLLCLK/2, or MCGFLLCLK 01 (ext?), 2-ext OSCERCLK, 3-internal MCGIRCLK
	//Configure registers
	TPM0->MOD= 0xCCD; //20ms

	//working with TPM0_C0SC
	//output compare + edge aligned PWM MSBA: 10, ELSBA:10
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1);
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;  //clear spurious interrupts
	TPM0->CONTROLS[0].CnV = 0xF6; //pulse_width;	//pulse width defined
	TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK  ;
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}

void TPM0_IRQHandler()
{
	//optionally check the flag
	if (TPM0->STATUS & TPM_STATUS_CH0F_MASK)
	{
		TPM0->CONTROLS[0].CnSC |=TPM_CnSC_CHF_MASK;//clear flag
	}
	PTD->PTOR |= MASK(SERVO ) ; 
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear the interrupt
}

void init_servo_pin()
{
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[SERVO] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTD->PCR[SERVO] |= PORT_PCR_MUX(1);	//setup to be GPIO
	PTD->PDDR |= MASK(SERVO ) ;
	PTD->PCOR |= MASK(SERVO ) ; //set to 0
}

void turn_servo_left()
{
	pulse_width = 0xA4; //1ms
}
void turn_servo_forward()
{
	pulse_width = 0xF6; //1.5ms
}
void turn_servo_right()
{
	pulse_width = 0x148; //2ms
}
