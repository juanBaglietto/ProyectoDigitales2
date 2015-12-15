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
	 InicUart1();

}
void InicGPIOs ( void )
{
	Chip_GPIO_SetDir(LPC_GPIO,PIN_TE_485,SALIDA); //control rs485

	Chip_GPIO_SetDir(LPC_GPIO,PIN_CONT_USB,SALIDA);//pin para habilitar el usb

	Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
	Chip_GPIO_WritePortBit(LPC_GPIO,PIN_CONT_USB,0);

	Chip_IOCON_PinMux(LPC_IOCON,DHT_DATA,MODO0,FUNCION0);

	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 22, true);
	//Chip_GPIO_WriteDirBit(LPC_GPIO,2,9,1);//pin para restencia de
	//Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);

	//Chip_GPIO_Init(LPC_GPIO);
	//Chip_IOCON_Init(LPC_IOCON);


}

void InicTimers(void){

	uint32_t timerFreq;

	//inicializo los timers para la maquina de estado
		Chip_TIMER_Init(LPC_TIMER3);
		Chip_TIMER_Init(LPC_TIMER2);//lo utilizo para el TIMEOUT

	//tomo la frecuencia del clok
		timerFreq = Chip_Clock_GetSystemClockRate();

	//confinfiguro el timer 2 y timer 3

		Chip_TIMER_Reset(LPC_TIMER3);
		Chip_TIMER_Reset(LPC_TIMER2);
		Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER3,SYSCTL_CLKDIV_1); //No divido el clock general

		Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);

		Chip_TIMER_Enable(LPC_TIMER3);
		Chip_TIMER_Enable(LPC_TIMER2);

		//habilito interrupciones

		NVIC_ClearPendingIRQ(TIMER3_IRQn);
		NVIC_EnableIRQ(TIMER3_IRQn);

		NVIC_ClearPendingIRQ(TIMER2_IRQn);
		NVIC_EnableIRQ(TIMER2_IRQn);


}


