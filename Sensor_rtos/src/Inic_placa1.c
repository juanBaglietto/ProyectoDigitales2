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
}

void InicTimers(void){

	uint32_t timerFreq;

	//inicializo los timers para la maquina de estado
		Chip_TIMER_Init(LPC_TIMER3);

	//tomo la frecuencia del clok
		timerFreq = Chip_Clock_GetSystemClockRate();


	//confinfiguro el timer 2

		Chip_TIMER_Reset(LPC_TIMER3);
		Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER0,SYSCTL_CLKDIV_1); //No divido el clock general
		Chip_TIMER_MatchEnableInt(LPC_TIMER3, 1);
		Chip_TIMER_SetMatch(LPC_TIMER3, 1, 250);
		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);


		//Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,ENTRADA);
		Chip_IOCON_PinMux(LPC_IOCON,DHT_DATA,MODO3,FUNC3);//P0 pin 23  CAP3.0 j6-15



		//Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER3,0);
		//Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER3,0);
		Chip_TIMER_CaptureEnableInt(LPC_TIMER3,0);

		Chip_TIMER_Enable(LPC_TIMER3);

		//habilito interrupciones

		NVIC_ClearPendingIRQ(TIMER3_IRQn);
		NVIC_EnableIRQ(TIMER3_IRQn);


}
