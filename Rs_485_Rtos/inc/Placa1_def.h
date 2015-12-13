/*
 * Placa1_def.h
 *
 *  Created on: 2/10/2015
 *      Author: Juani
 */


#ifndef PLACA1_DEF_H_
#define PLACA1_DEF_H_
#include "chip.h"




//puertos
	#define		PORT0		0
	#define		PORT1		1
	#define		PORT2		2
	#define		PORT3		3
	#define		PORT4		4


//salidas
	#define		LEDXpresso	PORT0,22

//uart
	#define 	PIN_tx_rs485     PORT0,15
	#define 	PIN_rx_rs485     PORT0,16
	#define 	PIN_TE_485		 PORT0,22

//SENSOR:
	#define		DHT_DATA	PORT2,10

// comunic rs-485

#define master_id 		1769
#define esclavo_1 		425
#define rps_id 			13

#define esclavo_2 		333
#define nievel_id 		77


#define trama 5

//Defines generales:
	#define		OFF			0
	#define		ON			1
	#define 	MODO2     2
	#define 	FUNCION1  1

//buffer circulares para rs-485



#define UART_SRB_SIZE 32	/* Send */
#define UART_RRB_SIZE 32	/* Receive */

// dirección en FIODIR
	#define 	ENTRADA			0
	#define 	SALIDA			1



	#define		__R					volatile const
	#define		__W					volatile
	#define		__RW				volatile

	typedef 	unsigned int 		uint32_t;
	typedef 	unsigned short 		uint16_t;
	typedef 	unsigned char 		uint8_t;

	void kitInic(void);
	void InicGPIOs ( void );
	void InicTimers(void);
	void InicUart1(void);

	STATIC RINGBUFF_T txring;
	STATIC RINGBUFF_T rxring;
	static uint8_t rxbuff[UART_RRB_SIZE], txbuff[UART_SRB_SIZE];


#endif /* PLACA1_DEF_H_ */
