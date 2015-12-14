/*
 * Placa1_def.h
 *
 *  Created on: 2/10/2015
 *      Author: Juani
 */

#ifndef PLACA1_DEF_H_
#define PLACA1_DEF_H_

#include "DHT.h"


//puertos
	#define		PORT0		0
	#define		PORT1		1
	#define		PORT2		2
	#define		PORT3		3

	#define 	MODO0		0
	#define 	MODO1		1
	#define 	MODO2		2
	#define 	MODO3		3

	#define 	FUNC0		0
	#define 	FUNC1		1
	#define 	FUNC2		2
	#define 	FUNC3		3


//salidas
	#define		LEDXpresso	PORT0,22

//SENSOR:
	#define		DHT_DATA	PORT0,23


//Defines generales:
	#define		OFF			0
	#define		ON			1

// dirección en FIODIR
	#define 	ENTRADA			0
	#define 	SALIDA			1


	#define		__R					volatile const
	#define		__W					volatile
	#define		__RW				volatile

	void kitInic(void);
	void InicGPIOs ( void );
	void  InicTimers(void);


#endif /* PLACA1_DEF_H_ */
