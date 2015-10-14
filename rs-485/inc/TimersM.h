/*
 * TimersM.h
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */
#include "Placa1_def.h"

	#define			TIMERSM			7

	#define			LIMPIObANDERAM	EventosM &= ~i

	// --- Eventos
	#define			EVENTOM1		0
	#define			EVENTOM2		1
	#define			EVENTOM3		2
	#define			EVENTOM4		3
	#define			EVENTOM5		4
	#define			EVENTOM6		5
	#define			EVENTOM7		6

	// --- Tiempos

	#define			TM1			2500 	//25 miliseg
	#define			TM2			3 	//30 microseg
	#define			TM3			2 	//20 microseg
	#define			TM4			50000 	//0,5 seg
	#define			TM5			4	//40 microseg
	#define			TM6			500	//5mS
	#define			TM7			500000 //5s


	#define 	VencioESPERA25ms	(EventosM & 0x01)
	#define 	VencioESPERA30us 	(EventosM & (0x01<<1))
	#define 	VencioESPERA20us 	(EventosM & (0x01<<2))
	#define 	VENCIOTimeout 		(EventosM & (0x01<<3))
	#define 	VencioESPERA40us 	(EventosM & (0x01<<4))
	#define 	VencioTimer485	 	(EventosM & (0x01<<5))
	#define 	VencioTimeOUT485 	(EventosM & (0x01<<6))

	void TmrStartM(uint16_t ev,uint32_t t);
	void AnalizarTimerM(void);
	void TmrStopM (uint8_t ev);
	void ApagoTimers(void);
	void ArrancoTimers(void);



