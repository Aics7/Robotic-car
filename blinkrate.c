
#include <MKL25Z4.h>
#include <stdio.h>
#include "millis.h"

#define MASK(x)	(1UL << x)
#define PTE20 (20)
void init_ADC(void) ;
float measure_POT(void);

static unsigned long cur_time =0u;
static unsigned long l_run=0u;


char bufff[20];

void do_blink_rate (void) {
	cur_time = millis();
	int ctr=0;
	init_ADC();
	float val=measure_POT();
		//sprintf(bufff,"Voltage= %5.2", val);
	int num = (int) 330/val;
	
	if(cur_time - l_run > num)
	{
		PTA->PTOR=MASK(13); 	
		l_run = cur_time;
	}
}

void init_ADC(void) {

	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Select analog for pin
	PORTE->PCR[PTE20] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[PTE20] |= PORT_PCR_MUX(0);

	// use low power and long sample time improves accuracy (& uses less power)
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK ;
	//16 bit single-ended conversion, when DIFF=0
	ADC0->CFG1 |= ADC_CFG1_MODE(3);
	// Default settings: Software trigger, voltage references VREFH and VREFL
	ADC0->SC2 = ADC_SC2_REFSEL(0);

	configure_systick();
	//clock gating to port A
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	//set up pins as GPIO
	PORTA->PCR[13] &= ~PORT_PCR_MUX_MASK;	//Clear mux
	PORTA->PCR[13] |= PORT_PCR_MUX(1);	//setup to be GPIO

	//set up outputs
	PTA->PDDR |= MASK(13);

}



//consider continuous conversion, enable hardware average, take avg of 32 samples ((2^2)^N, N=3
//ADC0->SC3 |=ADC_SC3_ADCO_MASK | ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);

float measure_POT(void){
	float val;
	unsigned int sample =0;

	ADC0->SC1[0] = 0x00; // start conversion on channel 0, (the software trigger & chan selection)

	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	sample = ADC0->R[0];
	val = (float)3.3*sample/0xffff;		//recall 0xffff maps to max of 3.3v
	return val;
}



