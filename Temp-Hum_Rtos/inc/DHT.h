/*
 * DHT.h
 *
 *  Created on: 2/10/2015
 *      Author: Juani
 */

#ifndef DHT_H_
#define DHT_H_

#include "Placa1_def.h"
#include "TimersM.h"
#include "chip.h"


#define DHT_SALIDA	Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,SALIDA)//SetDIR (DHT_DATA , SALIDA)
#define	DHT_ENTRADA	Chip_GPIO_SetDir(LPC_GPIO,DHT_DATA,ENTRADA)//SetDIR (DHT_DATA , ENTRADA)
#define	DHT_SETLOW	Chip_GPIO_WritePortBit(LPC_GPIO,DHT_DATA,0)//SetPIN(DHT_DATA,0)
#define	DHT_SETHIGH	Chip_GPIO_WritePortBit(LPC_GPIO,DHT_DATA,1)//SetPIN(DHT_DATA,1)
#define	DHT_is		Chip_GPIO_GetPinState(LPC_GPIO,DHT_DATA)//GetPIN(DHT_DATA,1)
//#define	DHT_isLOW	//GetPIN(DHT_DATA,0)

#define N_useg_match2 (1)

uint8_t DHT_isLOW;
uint8_t DHT_isHIGH;



int DHT_Mde(void);
void lec_pin_DHT(void);

#endif /* DHT_H_ */
