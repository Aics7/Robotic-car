#include <MKL25Z4.h>
#include "millis.h"
static volatile uint32_t counter=0;

void configure_systick()
{
	//configuring SysTick
	SysTick->LOAD = (20971520u/1000u)-1 ;  //configure for every half sec restart.
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
}

/*returns time in ms*/
unsigned long  millis(){
	return (unsigned long)counter;
}

/*SysTick_Handler ---- increments counter by 1 after every ms*/
void SysTick_Handler(){
	counter++;
}