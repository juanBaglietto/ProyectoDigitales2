/*
===============================================================================
 Name        : Rs_485_Rtos.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/



#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "Placa1_def.h"

xQueueHandle rx_uart;
xSemaphoreHandle Sem_env;
xSemaphoreHandle Sem_rec;

STATIC RINGBUFF_T txring;
STATIC RINGBUFF_T rxring;
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


void UART1_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_UART1, &rxring, &txring);
}


void kitInic(void)
{
 InicGPIOs();
 InicUart1();


}
void InicGPIOs ( void )
{
	Chip_GPIO_SetDir(LPC_GPIO,LEDXpresso,SALIDA);
	Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);
	Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,ENTRADA);
}


void InicUart1(void){



	Chip_UART_Init(LPC_UART1);
	Chip_UART_SetBaud(LPC_UART1, 9600);
	Chip_UART_ConfigData(LPC_UART1, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));

	Chip_UART_TXEnable(LPC_UART1);

	//Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	Chip_GPIO_SetDir(LPC_GPIO,0,15,SALIDA);
	Chip_GPIO_SetDir(LPC_GPIO,0,16,ENTRADA);

	Chip_IOCON_PinMux(LPC_IOCON,0,15,MODO2,FUNCION1);
	Chip_IOCON_PinMux(LPC_IOCON,0,16,MODO2,FUNCION1);

	 RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	 RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	 Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
	 							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

		/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, 1);
	NVIC_EnableIRQ(UART1_IRQn);
}

static void Enviar_rs485 ()
{
	uint8_t envio=10;

		while(1)
		{
			//Tamanio_cola=uxQueueMessagesWaiting(xQueue1);
			//if(uxQueueMessagesWaiting(xQueue1)>= 128)
			//{

				//while(xQueueReceive(rx_uart,&envio,0)){
					xSemaphoreTake(Sem_env,0);
					Chip_UART_SendRB(LPC_UART1, &txring, (const uint8_t *)&envio, 1);
					xSemaphoreGive(Sem_env);

				//}
			//}
		}

}

static void Recibir_rs485()
{
	uint8_t recibo;

		while (1)
		{
			xSemaphoreTake(Sem_env,0);
			Chip_UART_ReadRB(LPC_UART1, &rxring, (const uint8_t *)&recibo,1);
			xSemaphoreGive(Sem_env);
		}
}

void task_inic()
{
	xTaskCreate(Enviar_rs485, (signed char *) "Envio datos rs-485 ",
					configMINIMAL_STACK_SIZE, NULL, 1,
					(xTaskHandle *) NULL);

	xTaskCreate(Recibir_rs485, (signed char *) "Recibo datos rs-485 ",
					configMINIMAL_STACK_SIZE, NULL, 1,
					(xTaskHandle *) NULL);


	rx_uart=xQueueCreate(512, sizeof( uint16_t ));

	vSemaphoreCreateBinary(Sem_env);
	vSemaphoreCreateBinary(Sem_rec);
}

void main(void)
{
    SystemCoreClockUpdate();

    kitInic();
    task_inic();

    vTaskStartScheduler();

}

