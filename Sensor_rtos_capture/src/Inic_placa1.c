/*
 * inic_placa1.c
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */

#include "DHT.h"


void kitInic(void)
{
 InicGPIOs();
 InicTimers();


}
void InicGPIOs ( void )
{
	Chip_GPIO_SetDir(LPC_GPIO,LEDXpresso,SALIDA);
	Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);
	Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,ENTRADA);
}

void InicTimers(void){

	uint32_t timerFreq;

	//inicializo los timers para la maquina de estado
		Chip_TIMER_Init(LPC_TIMER3);
		Chip_TIMER_Init(LPC_TIMER0);

	//tomo la frecuencia del clok
		timerFreq = Chip_Clock_GetSystemClockRate();

	//confinfiguro el timer 2

		Chip_TIMER_Reset(LPC_TIMER3);

		Chip_TIMER_Reset(LPC_TIMER0);
		Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER3,SYSCTL_CLKDIV_4); //No divido el clock general
		Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER0,SYSCTL_CLKDIV_4);



		//configuracion de los march del timer 0
		//match 1 timer 0
		Chip_TIMER_SetMatch(LPC_TIMER0, 1, timerFreq / (40*4));// seteo el mach 1 con 25 ms
		Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);

		//match 2 timer 0
		Chip_TIMER_SetMatch(LPC_TIMER0, 2, (timerFreq / (33333*4))+(timerFreq / (40*4))); //seteo el mach 2 con 30us
		Chip_TIMER_MatchEnableInt(LPC_TIMER0, 2);

		Chip_TIMER_CaptureEnableInt(LPC_TIMER3,0);


		//habilito interrupciones

		NVIC_ClearPendingIRQ(TIMER3_IRQn);
		NVIC_EnableIRQ(TIMER3_IRQn);

		NVIC_ClearPendingIRQ(TIMER0_IRQn);
		NVIC_EnableIRQ(TIMER0_IRQn);


}
