/*
===============================================================================
 Name        : Temp-Hum.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"
#include "DHT.h"

//#include <board.h>


//#include <cr_section_macros.h>

__RW uint32_t TmrRunM[8];	//para maq de timers
__RW uint8_t EventosM;

__RW uint8_t Humestring[7]; 	//vectores globales que tienen las medidas de la temp y humedad
__RW uint8_t Tempstring[8];

__RW uint8_t flagLeerMensajeNuevo=0;

void TIMER2_IRQHandler(void)//cada 10 micro segundos
{
	if (Chip_TIMER_MatchPending(LPC_TIMER2, 1)) {
			Chip_TIMER_ClearMatch(LPC_TIMER2, 1);
			AnalizarTimerM();

		}

}

int main(void) {


    SystemCoreClockUpdate();
    kitInic();

    uint32_t delay;
    uint8_t lectura,cantlecturas=0;

       for(delay=0;delay<0x00ffffff;delay++);



    while(1) {

    	//ApagoTimers();//apago timers para no molestar en lectura de DHT
    	lec_pin_DHT();
    	lectura=DHT_Mde();

    }
    return 0 ;
}
