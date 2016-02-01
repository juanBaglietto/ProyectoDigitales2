/*
 * Mde_DHT.c
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */
#include "DHT.h"

uint8_t Humestring[7];
uint8_t Tempstring[8];
extern uint8_t flagLeerMensajeNuevo;

extern uint8_t bit_zero;
extern uint8_t bit_one;

//Funcion con maquina de estados
int DHT_Mde(void) {
	static uint8_t estado = 0;
	static uint8_t byte[5];
	static uint8_t i = 0, j = 7; //i recorre los bytes y j los bits de cada byte
	uint8_t neg = 0;
	uint16_t checkmio = 0;
	uint16_t hume = 0, temp = 0, check;
	uint16_t ent_temp, dec_temp, ent_hume, dec_hume; //partes enteras y partes decimales





	switch (estado) {
	case 0:
		//DHT_SALIDA; //LO HAGO SALIDA
		Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,SALIDA);
		Chip_GPIO_SetDir(LPC_GPIO,0,24,SALIDA);
		//DHT_SETLOW;
		Chip_GPIO_WritePortBit(LPC_GPIO,DHT_DATA,0);
		Chip_GPIO_WritePortBit(LPC_GPIO,0,24,0);
		Chip_TIMER_Enable(LPC_TIMER0);// incio el mach de 25ms

		estado = 1;
		break;
	case 1:
		xSemaphoreTake(sem_mach1_timer0, portMAX_DELAY);
		DHT_SETHIGH;

		estado = 2;

		break;
	case 2:
		xSemaphoreTake(sem_mach2_timer0, portMAX_DELAY);
		 //LO HAGO ENTRADA
		Chip_IOCON_PinMux(LPC_IOCON,DHT_DATA,MODO0,FUNC3);
		DHT_ENTRADA;
		Chip_TIMER_Enable(LPC_TIMER3);
		Chip_TIMER_CaptureEnableInt(LPC_TIMER3,0);

		//Chip_TIMER_CaptureFallingEdgeEnable(LPC_TIMER3, 0); //HABILITO LA CAPTURA DEL FALNCO desndente
		Chip_TIMER_CaptureFallingEdgeDisable(LPC_TIMER3, 0);
		Chip_TIMER_CaptureRisingEdgeEnable(LPC_TIMER3, 0);

		estado = 3;

		break;

	case 3:
		xSemaphoreTake(sem_cap_high_80us, portMAX_DELAY);
		//se entro es por que pasaron los 80us en alto y me va a venir un o de trasmiscion

		//xSemaphoreTake(sem_cap_valor, portMAX_DELAY);
		estado = 4;

		break;

	case 4:
		xSemaphoreTake(sem_cap_valor, portMAX_DELAY);

		if (bit_one) {
			byte[i] |= (uint8_t) 0x01 << j; //pongo un uno en la posicion

		}
		if (bit_zero) { //la lectura fue un 0, espero al proximo 1

		}
		j--;
		if (j > 7) //era 0 y ahora hizo overflow
				{
			i++;
			j = 7;
			if (i == 5 ) {
				estado = 5; //termino la lectura de los 5 bytes
				Chip_TIMER_Disable(LPC_TIMER3);
				Chip_TIMER_CaptureDisableInt(LPC_TIMER3,0);

			}
		}
		break;
	case 5:
		checkmio = byte[0] + byte[1] + byte[2] + byte[3];
		checkmio &= 0x00ff;
		//anido los datos
		hume = byte[0];
		hume = hume << 8;
		hume |= byte[1];
		temp = byte[2];
		temp = temp << 8;
		temp |= byte[3];
		check = byte[4];
		//checkeo si lo que me llego es correcto
		if (checkmio != check) {
			estado = 0;
			flagLeerMensajeNuevo = 0; //termino lectura
			return 1; //suma incorrecta

		}
		//datos correctamente recibidos
		//estan en hume y temp
		// checkeo si la temperatura es negativa
		if (temp & 0x8000)			//si esto es verdad es negativa
				{
			neg = 1;			//activo un flag
			temp = temp & 0x7FFF;//le saco el uno del bit mas significativo que indica negatividad
		}
		//decodifico la información
		dec_temp = temp % 10;
		ent_temp = temp / 10;
		dec_hume = hume % 10;
		ent_hume = hume / 10;
		//La coloco en el buffer que manipularán otras funciones:
		Humestring[6] = '\0';		//para que puea ser manipulado como string
		Humestring[5] = 'H';
		Humestring[4] = ' ';
		Humestring[3] = '0' + dec_hume;
		Humestring[2] = ',';
		for (i = 1; i >= 0 && i <= 1; i--) {
			Humestring[i] = '0' + ent_hume % 10;
			ent_hume = ent_hume / 10;
		}

		Tempstring[7] = '\0';		//para que puea ser manipulado como string
		Tempstring[6] = 'C';
		Tempstring[5] = ' ';
		Tempstring[4] = '0' + dec_temp;
		Tempstring[3] = ',';
		for (i = 2; i >= 1 && i <= 2; i--) {
			Tempstring[i] = '0' + ent_temp % 10;
			ent_temp = ent_temp / 10;
		}
		if (neg == 1)			//si es negativo le agrego un menos
				{
			Tempstring[0] = '-';
		} else {
			Tempstring[0] = ' ';			//si es positivo dejo un espacio
		}
		estado = 0; //para la proxima vez que entre
		i = 0;
		j = 7;
		byte[0] = byte[1] = byte[2] = byte[3] = byte[4] = 0;
		flagLeerMensajeNuevo = 0; //termino la lectura

		break;

	}
	return 0; //lectura correcta por el momento
}

