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
		Chip_TIMER_Init(LPC_TIMER2);

	//tomo la frecuencia del clok
		timerFreq = Chip_Clock_GetSystemClockRate();


	//confinfiguro el timer 2 y el match 1

		Chip_TIMER_Reset(LPC_TIMER2);
		Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
		Chip_TIMER_SetMatch(LPC_TIMER2, 1, 250);
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);

		Chip_TIMER_Enable(LPC_TIMER2);

		//habilito interrupciones

		NVIC_ClearPendingIRQ(TIMER2_IRQn);
		NVIC_EnableIRQ(TIMER2_IRQn);


}
