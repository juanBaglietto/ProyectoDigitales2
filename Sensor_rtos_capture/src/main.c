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
uint8_t bit_zero=0;
uint8_t bit_one=0;
uint8_t flagLeerMensajeNuevo;

void TIMER0_IRQHandler(void) //cada 10 micro segundos
{
	if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
			Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
			xSemaphoreGive(sem_mach1_timer0);

		}
		if (Chip_TIMER_MatchPending(LPC_TIMER0, 2)) {
				Chip_TIMER_ClearMatch(LPC_TIMER0, 2);

				xSemaphoreGive(sem_mach2_timer0);
				Chip_TIMER_Reset(LPC_TIMER0);
				Chip_TIMER_Disable(LPC_TIMER0);

		}
}

void TIMER3_IRQHandler(void) //cada 10 micro segundos
{
	static uint32_t cuenta_actual = 0;
	static uint32_t cuenta_anterior = 0;
	static uint32_t diferencia_cuenta = 0;
	static uint32_t diferencia_HIGH = 0;
	static uint32_t tiempo_HIGH = 0;




	if (Chip_TIMER_CapturePending(LPC_TIMER3, 0)) {

		if (DHT_is == true) //entra si es acendente

		{

			cuenta_actual = Chip_TIMER_ReadCapture(LPC_TIMER3, 0);

			cuenta_anterior = cuenta_actual;
			Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER3, 0); //habilito la detenccion del flanco descendente
			Chip_TIMER_CaptureRisingEdgeDisable(LPC_TIMER3, 0); //desabilito la deteccion del flanco  ascendente

		}
		else //entra si es descendente
		{
			cuenta_actual = Chip_TIMER_ReadCapture(LPC_TIMER3, 0);

			if (cuenta_actual > cuenta_anterior)
			{
				diferencia_cuenta = cuenta_actual- cuenta_anterior;
				tiempo_HIGH = (diferencia_cuenta/30);

				if (tiempo_HIGH >75
						&& tiempo_HIGH < 85)
				{
					xSemaphoreGive(sem_cap_high_80us);
					Chip_TIMER_Reset(LPC_TIMER3);
					cuenta_actual=0;

				}
				if (tiempo_HIGH > (20)
						&& tiempo_HIGH < (40))
				{
					xSemaphoreGive(sem_cap_valor);
					Chip_TIMER_Reset(LPC_TIMER3);
					cuenta_actual=0;
					bit_zero = 1;
					bit_one = 0;

				}
				if (tiempo_HIGH > 65
						&& tiempo_HIGH < 75)
				{
					xSemaphoreGive(sem_cap_valor);
					Chip_TIMER_Reset(LPC_TIMER3);
					cuenta_actual=0;
					bit_zero = 0;
					bit_one = 1;

				}
			}
			cuenta_anterior = cuenta_actual;
			Chip_TIMER_CaptureFallingEdgeDisable(LPC_TIMER3, 0);
			Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER3, 0);

		}

		Chip_TIMER_ClearCapture(LPC_TIMER3, 0);
	}

}

static void Sensor() {
	uint8_t lectura = 0;
	while (1) {

		if(flagLeerMensajeNuevo==0){

			vTaskDelay(500 / portTICK_RATE_MS);
			flagLeerMensajeNuevo=1;
		}
		lectura = DHT_Mde();
	}

}

int main(void) {

	SystemCoreClockUpdate();

	vSemaphoreCreateBinary(sem_mach1_timer3);
	vSemaphoreCreateBinary(sem_mach0_timer3);
	vSemaphoreCreateBinary(sem_mach1_timer0);
	vSemaphoreCreateBinary(sem_mach2_timer0);
	vSemaphoreCreateBinary(sem_cap_high_80us);
	vSemaphoreCreateBinary(sem_cap_valor);

	xSemaphoreTake(sem_mach1_timer3, 0);
	xSemaphoreTake(sem_mach0_timer3, 0);
	xSemaphoreTake(sem_mach1_timer0, 0);
	xSemaphoreTake(sem_mach2_timer0, 0);
	xSemaphoreTake(sem_cap_high_80us, 0);
	xSemaphoreTake(sem_cap_valor, 0);

	kitInic();

	xTaskCreate(Sensor, (signed char * ) "Lec_sensor ",
			configMINIMAL_STACK_SIZE, NULL, 1, (xTaskHandle *) NULL);

	vTaskStartScheduler();

	return 0;
}
