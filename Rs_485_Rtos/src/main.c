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

xQueueHandle tx_uart;
xQueueHandle rx_uart;

xSemaphoreHandle Sem_env;
xSemaphoreHandle Sem_rec;

uint8_t en =0;

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
	Chip_GPIO_SetDir(LPC_GPIO,PIN_TE_485,SALIDA);
	Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
	//Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);
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

	Chip_IOCON_PinMux(LPC_IOCON,0,15,MODO2,FUNC1);
	Chip_IOCON_PinMux(LPC_IOCON,0,16,MODO2,FUNC1);

	 RingBuffer_Init(&rxring, rxbuff, 1, UART_RRB_SIZE);
	 RingBuffer_Init(&txring, txbuff, 1, UART_SRB_SIZE);

	 Chip_UART_SetupFIFOS(LPC_UART1, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
	 							UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

	Chip_UART_IntEnable(LPC_UART1, (UART_IER_RBRINT | UART_IER_RLSINT));

		/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(UART1_IRQn, 1);
	NVIC_EnableIRQ(UART1_IRQn);
}

static void Cargo_datos()
{
	unsigned char dato[]="Hola mundo";

	while(1)
	{
		xQueueSendToBack(tx_uart,&dato,portMAX_DELAY);
	}
}

static void Enviar_rs485 ()
{
	unsigned char envio[]="11 hola mundo";


		while(1)
		{
//			while(xQueueReceive(tx_uart,&envio,0))
//			{
				if(en==0){
					Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,1);
					en=1;

					Chip_UART_SendRB(LPC_UART1, &txring, (const char *)&envio, sizeof(envio) - 1);
					vTaskDelay(100/portTICK_RATE_MS);
					Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
					xSemaphoreGive(Sem_rec);
				}
//			}

		}

}

static void Recibir_rs485()
{
	unsigned char  recibo[20];
	uint8_t Cont_datos=0;

		while (1)
		{
			//xSemaphoreTake(Sem_rec,portMAX_DELAY);
			Cont_datos=Chip_UART_ReadRB(LPC_UART1, &rxring,&recibo,20);
			if(Cont_datos >= 2)
			{
				xQueueSendToBack(rx_uart,&recibo,0);
				//en=0;
			}
//			else
//			{
//				xSemaphoreGive(Sem_rec);
//			}

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

	xTaskCreate(Cargo_datos, (signed char *) "cargos datos",
						configMINIMAL_STACK_SIZE, NULL, 1,
						(xTaskHandle *) NULL);

	tx_uart=xQueueCreate(512, sizeof( uint16_t ));
	rx_uart=xQueueCreate(512, sizeof( uint16_t ));

	vSemaphoreCreateBinary(Sem_env);
	vSemaphoreCreateBinary(Sem_rec);
//	xSemaphoreTake(Sem_rec,0);
}

void main(void)
{
    SystemCoreClockUpdate();

    kitInic();
    task_inic();

    vTaskStartScheduler();

}

