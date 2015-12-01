/*
 * inic_placa1.c
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */

#include "Placa1_def.h"
#include "TimersM.h"
#include <chip.h>



void kitInic(void)
{
 InicGPIOs();
 //InicTimers();
 InicUart1();


}
void InicGPIOs ( void )
{
	Chip_GPIO_SetDir(LPC_GPIO,LEDXpresso,SALIDA);
	Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);
}

//void InicTimers(void){
//
//	uint32_t timerFreq;
//
//	//inicializo los timers para la maquina de estado
//		Chip_TIMER_Init(LPC_TIMER2);
//
//	//tomo la frecuencia del clok
//		timerFreq = Chip_Clock_GetSystemClockRate();
//
//
//	//confinfiguro el timer 2 y el match 1
//
//		Chip_TIMER_Reset(LPC_TIMER2);
//		Chip_TIMER_MatchEnableInt(LPC_TIMER2, 1);
//		Chip_TIMER_SetMatch(LPC_TIMER2, 1, 250);
//		Chip_TIMER_ResetOnMatchEnable(LPC_TIMER2, 1);
//
//		Chip_TIMER_Enable(LPC_TIMER2);
//
//		//habilito interrupciones
//
//		NVIC_ClearPendingIRQ(TIMER2_IRQn);
//		NVIC_EnableIRQ(TIMER2_IRQn);
//
//
//}

void InicUart1(void){



	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 9600);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));

	Chip_UART_TXEnable(LPC_UART1);

	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	Chip_IOCON_PinMux(LPC_IOCON,PIN_tx_rs485,MODO2,FUNCION1);

	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

		/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, 1);
	NVIC_EnableIRQ(UART1_IRQn);



}
