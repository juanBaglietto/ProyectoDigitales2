
#include "Placa1_def.h"


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

void TIMER3_IRQHandler(void) //cada 10 micro segundos
{

	if (Chip_TIMER_MatchPending(LPC_TIMER3, 1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER3, 1);
		xSemaphoreGive(sem_mach1_timer3);

	}

}

void TIMER2_IRQHandler(void) {
	if (Chip_TIMER_MatchPending(LPC_TIMER2, 1)) {
		Chip_TIMER_ClearMatch(LPC_TIMER2, 1);
		xSemaphoreGive(sem_timeOUT);
	}
}

static void Datos_motor()
{
	uint16_t rpm=0;
	uint16_t dato_rs485 [4];


	while(1)
	{
		while(xQueueReceive(motor,&rpm,portMAX_DELAY))
		{
			dato_rs485[0]=master_id;
			dato_rs485[1]=esclavo_1;
			dato_rs485[2]= rps_id;
			dato_rs485[3]=rpm;
			xQueueSendToBack(set_rpm,&rpm,0);

			xQueueSendToBack(tx_uart,&dato_rs485,0);
			//xQueueReceive(tx_uart,&aux,0);
			xSemaphoreGive(Sem_env);
		}
	}
}

static void Datos_bomba(void * pvParameters)
{

	uint16_t dato_rs485 [4];
	uint16_t nivel=0;


	while(1){

		while(xQueueReceive(bomba,&nivel,portMAX_DELAY))
		{

			dato_rs485[0]=master_id;
			dato_rs485[1]=esclavo_2;
			dato_rs485[2]=nievel_id;
			dato_rs485[3]=nivel;
			xQueueSendToBack(set_nivel,&nivel,0);

			xQueueSendToBack(tx_uart,&dato_rs485,0);
			xSemaphoreGive(Sem_env);
		}
	}



}

static void Sensor() {

	uint8_t fin_lec;

	while (1) {
//		if (fin_lec == 1) {
//			vTaskDelay(10000 / portTICK_RATE_MS);
//		}
//		NVIC_DisableIRQ(USB_IRQn);
		fin_lec = DHT_Mde();
	}

}
static void Enviar_rs485(void * pvParameters)
{
	uint16_t  envio[4];

	while(1)
	{

		xSemaphoreTake(Sem_env,portMAX_DELAY);

		Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,1);
		//copio los valores de la cola a enviar en la trama
		xQueueReceive(tx_uart,&envio,0);

		Chip_UART_SendRB(LPC_UART1, &txring, (const uint16_t *)&envio, sizeof(envio));
		vTaskDelay(15/portTICK_RATE_MS);
		Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
		xSemaphoreGive(Sem_rec);
	}

}

static void Recibir_rs485(void * pvParameters)
{
	uint16_t recibo[5];
	uint16_t rpm_usb;
	uint16_t nivel_usb;
	int Cont_datos;

	while (1)
	{

		recibo[0]=0;
		recibo[1]=0;
		recibo[2]=0;
		recibo[3]=0;
		recibo[4]=0;
		Cont_datos=0;

		xSemaphoreTake(Sem_rec,portMAX_DELAY);
		Cont_datos=Chip_UART_ReadRB(LPC_UART1, &rxring,&recibo,sizeof(recibo));

		if(Cont_datos <=10 &&Cont_datos >=9)
		{

			if(recibo[0]== esclavo_1 && recibo[1]== master_id && recibo[2]== rps_id)
			{
				xQueueReceive(set_rpm,&rpm_usb,0);
				if(recibo[3]==rpm_usb)
				{

					xQueueSendToBack(actual_rpm,&recibo[4],0);
					xSemaphoreTake(Sem_env,0);


				}
			}
			else if(recibo[0]== esclavo_2 && recibo[1]== master_id && recibo[2]== nievel_id)
			{
				xQueueReceive(set_nivel,&nivel_usb,0);
				if(recibo[3]==nivel_usb)
				{
					xQueueSendToBack(actual_nivel,&recibo[4],0);
					xSemaphoreTake(Sem_env,0);

				}
			}
			else
			{
				xSemaphoreGive(Sem_rec);
			}

		}
		else
		{
			xSemaphoreGive(Sem_rec);
		}

	}
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
void task_inic()
{
	xTaskCreate(Enviar_rs485, (signed char *) "Envio datos rs-485 ",
			configMINIMAL_STACK_SIZE, NULL, 1,
			(xTaskHandle *) NULL);

	xTaskCreate(Recibir_rs485, (signed char *) "Recibo datos rs-485 ",
			configMINIMAL_STACK_SIZE, NULL, 1,
			(xTaskHandle *) NULL);

	xTaskCreate(Datos_bomba, (signed char *) "cargos datos",
			configMINIMAL_STACK_SIZE, NULL, 1,
			(xTaskHandle *) NULL);
	xTaskCreate(Datos_motor, (signed char *) "cargos datos",
			configMINIMAL_STACK_SIZE, NULL, 1,
			(xTaskHandle *) NULL);
	xTaskCreate(Sensor, (signed char * ) "Lec_sensor ",
			configMINIMAL_STACK_SIZE, NULL, 1, (xTaskHandle *) NULL);


	tx_uart=xQueueCreate(1, sizeof( uint16_t  )*4);
	rx_uart=xQueueCreate(5, sizeof( uint16_t ));
	bomba=xQueueCreate(1, sizeof( uint8_t ));
	motor=xQueueCreate(1, sizeof( uint8_t ));
	set_rpm=xQueueCreate(1, sizeof( uint16_t ));
	set_nivel=xQueueCreate(1, sizeof( uint16_t ));
	actual_rpm=xQueueCreate(1, sizeof( uint16_t ));
	actual_nivel=xQueueCreate(1, sizeof( uint16_t ));
	actual_temp=xQueueCreate(1, sizeof( uint16_t ));
	actual_hum=xQueueCreate(1, sizeof( uint16_t ));

	//semaforos sensor
	vSemaphoreCreateBinary(sem_mach1_timer3);
	vSemaphoreCreateBinary(sem_timeOUT);
	xSemaphoreTake(sem_mach1_timer3, 0);
	xSemaphoreTake(sem_timeOUT, 0);

	//semaforos 485
	vSemaphoreCreateBinary(Sem_env);
	vSemaphoreCreateBinary(Sem_rec);
	xSemaphoreTake(Sem_env,0);
	xSemaphoreTake(Sem_rec,0);
}

int main(void)
{


	/* Initialize board and chip */
	SystemCoreClockUpdate();
	usb_init();
	kitInic();
	task_inic();

	vTaskStartScheduler();
	return 0;
}
