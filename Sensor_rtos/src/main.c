/*
===============================================================================
 Name        : Sensor_rtos.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "DHT.h"


#include <cr_section_macros.h>



void TIMER3_IRQHandler(void)//cada 10 micro segundos
{
	static uint32_t tiempo1=0;
	static uint32_t tiempo2=0;
	static uint32_t	diferencia_LOW=0;
	static uint32_t	diferencia_HIGH=0;
	static uint32_t tiempo_HIGH=0;
	static uint32_t tiempo_LOW=0;
	static uint32_t timerFreq;
	static timerFreq = Chip_Clock_GetSystemClockRate();

	if(Chip_TIMER_CapturePending(LPC_TIMER3,0)) //entra si es acendente
	{
		if(DHT_is==true)
		{
			tiempo1 = Chip_TIMER_ReadCapture(LPC_TIMER3,0 );

			if(tiempo1>tiempo2 )
			{
				diferencia_HIGH=tiempo1-tiempo2;
				tiempo_HIGH=(diferencia_HIGH)*(1/(timerFreq));
				tiempo2=0;

				if(tiempo_HIGH>(70*1000000) && tiempo_HIGH<(90*1000000))
				{
					xSemaphoreGive(sem_cap_desc);
				}
			}
			Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER3,0);//habilito la detenccion del flanco descendente
						Chip_TIMER_CaptureRisingEdgeDisable(LPC_TIMER3,0);//desabilito la deteccion del flanco  ascendente

		}
		else//entra si es descendente
		{
			tiempo2 = Chip_TIMER_ReadCapture(LPC_TIMER3,0 );
			if(tiempo2 > tiempo1)
			{
				diferencia_LOW=tiempo2-tiempo1;
				tiempo_LOW=(diferencia_LOW)*(1/(timerFreq));
			}


			Chip_TIMER_CaptureFallingEdgeDisable(LPC_TIMER3,0);
			Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER3,0);
			Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);
		}
		Chip_TIMER_ClearCapture(LPC_TIMER3,0);
	}

	if(Chip_TIMER_MatchPending(LPC_TIMER3,1))
	{
		Chip_TIMER_ClearMatch(LPC_TIMER3, 1);
		DHT_SETHIGH;
		xSemaphoreGive(sem_mach1);
	}


}



static void Sensor()
{
	uint8_t lectura=0;
	while (1){

				//lectura=DHT_Mde();
	}

}

int main(void) {


	SystemCoreClockUpdate();


	xTaskCreate(Sensor, (signed char *) "Lec_sensor ",
						configMINIMAL_STACK_SIZE, NULL, 1,
						(xTaskHandle *) NULL);

	vSemaphoreCreateBinary(sem_mach1);
	vSemaphoreCreateBinary(sem_cap_desc);

	xSemaphoreTake(sem_mach1,0);
	xSemaphoreTake(sem_cap_desc,0);

	kitInic();
	vTaskStartScheduler();


    return 0 ;
}
