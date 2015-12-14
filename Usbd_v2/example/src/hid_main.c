/*
 * @brief HID generic example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "hid_generic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "Placa1_def.h"


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/


uint8_t en =0;

STATIC RINGBUFF_T txring;
STATIC RINGBUFF_T rxring;
static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];

//USB
static USBD_HANDLE_T g_hUsb;

extern const  USBD_HW_API_T hw_api;
extern const  USBD_CORE_API_T core_api;
extern const  USBD_HID_API_T hid_api;
/* Since this example only uses HID class link functions for that class only */
static const  USBD_API_T g_usbApi = {
	&hw_api,
	&core_api,
	0,
	0,
	&hid_api,
	0,
	0,
	0x02221101,
};

const  USBD_API_T *g_pUsbApi = &g_usbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize pin and clocks for USB port */
static void usb_pin_clk_init(void)
{
	/* enable USB PLL and clocks */
	Chip_USB_Init();
	/* enable USB 1 port on the board */

	Board_USBD_Init(1);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from USB
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

void usb_init()
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	usb_pin_clk_init();

		/* initialize call back structures */
		memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
		usb_param.usb_reg_base = LPC_USB_BASE + 0x200;
		usb_param.max_num_ep = 2;
		usb_param.mem_base = USB_STACK_MEM_BASE;
		usb_param.mem_size = USB_STACK_MEM_SIZE;

		/* Set the USB descriptors */
		desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
		desc.string_desc = (uint8_t *) USB_StringDescriptor;

		/* Note, to pass USBCV test full-speed only devices should have both
		 * descriptor arrays point to same location and device_qualifier set
		 * to 0.
		 */
		desc.high_speed_desc = USB_FsConfigDescriptor;
		desc.full_speed_desc = USB_FsConfigDescriptor;
		desc.device_qualifier = 0;

		/* USB Initialization */
		ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {

			ret = usb_hid_init(g_hUsb,
				(USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
				&usb_param.mem_base, &usb_param.mem_size);
			if (ret == LPC_OK) {
				/*  enable USB interrupts */
				NVIC_EnableIRQ(USB_IRQn);
				/* now connect */
				USBD_API->hw->Connect(g_hUsb, 1);
			}
		}
}



void UART1_IRQHandler(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_UART1, &rxring, &txring);
}




static void Cargo_datos(void * pvParameters)
{
	uint8_t i;
	uint16_t dato [4];

	dato[0]=master_id;
	dato[1]=esclavo_1;
	dato[2]= rps_id;
	dato[3]=40;

	while(1)
	{
		for(i=0;i<5;i++)
		{
			xQueueSendToBack(tx_uart,&dato[i],portMAX_DELAY);
		}

	}
}

static void Enviar_rs485(void * pvParameters)
{
	uint16_t  envio[4];
	uint8_t	cam=0;

	envio[0]=master_id;
	envio[1]=esclavo_2;
	envio[2]=nievel_id ;
	envio[3]=90;

		while(1)
		{

		//	xSemaphoreTake(Sem_env,portMAX_DELAY);
			if(en==0)
			{
					Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,1);
					en=1;
					//copio los valores de la cola a enviar en la trama
//					xQueueReceive(tx_uart,&(envio[0]),0);
//					xQueueReceive(tx_uart,&(envio[1]),0);
//					xQueueReceive(tx_uart,&(envio[2]),0);
//					xQueueReceive(tx_uart,&(envio[3]),0);
					if(cam==0)
					{
						envio[1]=esclavo_1;
						envio[2]=rps_id;
						cam=1;
					}
					else
					{
						envio[1]=esclavo_2;
						envio[2]=nievel_id;
						cam=0;
					}

					Chip_UART_SendRB(LPC_UART1, &txring, (const uint16_t *)&envio, sizeof(envio));
					vTaskDelay(15/portTICK_RATE_MS);
					Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
					xSemaphoreGive(Sem_rec);
			}



		}

}

static void Recibir_rs485(void * pvParameters)
{
	uint16_t recibo[5];
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
					if(recibo[3]==90)
					{
						//xQueueSendToBack(rx_uart,&recibo[4],0);
						xSemaphoreGive(Sem_env);
						en=0;

					}
				}
				else if(recibo[0]== esclavo_2 && recibo[1]== master_id && recibo[2]== nievel_id)
				{
					if(recibo[3]==90)
					{
						//xQueueSendToBack(rx_uart,&recibo[4],0);
						//xSemaphoreGive(Sem_env);
						en=0;
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
	bomba=xQueueCreate(20, sizeof( uint8_t ));
	motor=xQueueCreate(20, sizeof( uint8_t ));

	vSemaphoreCreateBinary(Sem_env);
	vSemaphoreCreateBinary(Sem_rec);
	xSemaphoreTake(Sem_env,0);
	xSemaphoreTake(Sem_rec,0);
}

void kitInic(void)
{
 InicGPIOs();
 InicUart1();


}
void InicGPIOs ( void )
{
	Chip_GPIO_SetDir(LPC_GPIO,PIN_TE_485,SALIDA); //control rs485
	Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,ENTRADA);// sensor como entrada
	Chip_GPIO_SetDir(LPC_GPIO,PIN_CONT_USB,SALIDA);//pin para habilitar el usb

	Chip_GPIO_WritePortBit(LPC_GPIO,PIN_TE_485,0);
	Chip_GPIO_WritePortBit(LPC_GPIO,PIN_CONT_USB,0);

	//Chip_GPIO_WriteDirBit(LPC_GPIO, 0, 22, true);
	//Chip_GPIO_WriteDirBit(LPC_GPIO,2,9,1);//pin para restencia de
	//Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);

	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);


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

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
void main(void)
{


	/* Initialize board and chip */
	SystemCoreClockUpdate();

	kitInic();
	task_inic();

	usb_init();

	vTaskStartScheduler();

}
