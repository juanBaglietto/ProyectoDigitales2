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

static void Sensor() {
	uint8_t lectura = 0;
	while (1) {

		lectura = DHT_Mde();
	}

}

int main(void) {

	SystemCoreClockUpdate();

	vSemaphoreCreateBinary(sem_mach1_timer3);
	vSemaphoreCreateBinary(sem_timeOUT);
	xSemaphoreTake(sem_mach1_timer3, 0);
	xSemaphoreTake(sem_timeOUT, 0);

	xTaskCreate(Sensor, (signed char * ) "Lec_sensor ",
				configMINIMAL_STACK_SIZE, NULL, 1, (xTaskHandle *) NULL);

	kitInic();



	vTaskStartScheduler();

	return 0;
}
