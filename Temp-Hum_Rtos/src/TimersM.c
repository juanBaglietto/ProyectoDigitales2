/*
 * Timers.c

 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */

#include "DHT.h"
extern volatile uint32_t TmrRunM[];
extern volatile uint8_t EventosM;

void TmrStartM(uint16_t ev,uint32_t t){		//void TmrStart(EVENTO ev,TIEMPO t)
	if(t){
		TmrRunM[ev] = t;
		EventosM &= ~((uint16_t)0x01 << ev);
	}
	else {
		TmrRunM[ev]=0;
		EventosM &= ~((uint16_t)0x01 << ev);
	}
}


void AnalizarTimerM(void) {
	uint8_t i;

	for (i=0; i<TIMERSM; i++)
		if (TmrRunM[ i ])
   		{
   			TmrRunM[i]--;
	   		if( !TmrRunM[i] ){
	   			EventosM |= ((uint8_t) 0x01 << i) ;
	   		}
		}
}


void TmrStopM (uint8_t ev) {
	TmrRunM[ev] = 0;
  	EventosM &= ~((uint8_t) 0x01 << ev);

}

void ApagoTimers(void){
	//T1TCR = 0x00000002;
	//T0TCR = 0x00000002;
}

void ArrancoTimers(void){
	//T1TCR = 0x00000001;
	//T0TCR = 0x00000001;
}


