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
	#define		PORT4		4


//salidas
	#define		LEDXpresso	PORT0,22

//SENSOR:
	#define		DHT_DATA	PORT2,10


//Defines generales:
	#define		OFF			0
	#define		ON			1

// dirección en FIODIR
	#define 	ENTRADA			0
	#define 	SALIDA			1

// Timer 0
	#define		TIMER0 		( ( __RW uint32_t  * ) 0x40004000UL )
	#define		T0IR		TIMER0[0]
	#define		T0TCR		TIMER0[1]
	#define		T0TC		TIMER0[2]
	#define		T0PR		TIMER0[3]
	#define		T0PC		TIMER0[4]
	#define		T0MCR		TIMER0[5]
	#define		T0MR0		TIMER0[6]
	#define		T0MR1		TIMER0[7]
	#define		T0MR2		TIMER0[8]
	#define		T0MR3		TIMER0[9]
	#define		T0CCR		TIMER0[10]
	#define		T0CR0		TIMER0[11]
	#define		T0CR1		TIMER0[12]

	#define		T0EMR	( *( __RW uint32_t  * ) 0x4000403CUL )

	#define		T0CTCR	(* ( __RW uint32_t  * ) 0x40004070UL )

	// Timer 1
	#define		TIMER1 		( ( __RW uint32_t  * ) 0x40008000UL )
	#define		T1IR		TIMER1[0]
	#define		T1TCR		TIMER1[1]
	#define		T1TC		TIMER1[2]
	#define		T1PR		TIMER1[3]
	#define		T1PC		TIMER1[4]
	#define		T1MCR		TIMER1[5]
	#define		T1MR0		TIMER1[6]
	#define		T1MR1		TIMER1[7]
	#define		T1MR2		TIMER1[8]
	#define		T1MR3		TIMER1[9]
	#define		T1CCR		TIMER1[10]
	#define		T1CR0		TIMER1[11]
	#define		T1CR1		TIMER1[12]

	#define		T1EMR	(* ( __RW uint32_t  * ) 0x4000803CUL )

	#define		T1CTCR	( *( __RW uint32_t  * ) 0x40008070UL )

	#define		__R					volatile const
	#define		__W					volatile
	#define		__RW				volatile

	typedef 	unsigned int 		uint32_t;
	typedef 	unsigned short 		uint16_t;
	typedef 	unsigned char 		uint8_t;

	void kitInic(void);
	void InicGPIOs ( void );
	void  InicTimers(void);


#endif /* PLACA1_DEF_H_ */
