/*
 * Mde_DHT.c
 *
 *  Created on: 3/10/2015
 *      Author: Juani
 */
#include "DHT.h"

extern uint8_t TmrRunM[];
extern uint8_t EventosM;
extern uint8_t Humestring[7];
extern uint8_t Tempstring[8];
extern uint8_t flagLeerMensajeNuevo;
extern uint8_t DHT_isLOW;
extern uint8_t DHT_isHIGH;


//Funcion con maquina de estados
int DHT_Mde(void){
	static uint8_t estado=0;
	static uint8_t byte[5];
	static uint8_t i=0,j=7;//i recorre los bytes y j los bits de cada byte
	uint8_t neg=0;
	uint16_t checkmio=0;
	uint16_t hume=0,temp=0,check;
	uint16_t ent_temp,dec_temp,ent_hume,dec_hume; //partes enteras y partes decimales



	switch(estado){
		case 0:
			DHT_SALIDA; //LO HAGO SALIDA
			DHT_SETLOW;
			TmrStartM(EVENTOM1,TM1);//ARRANCO TIMER QUE ESPERA 25ms
			estado=1;
			break;
		case 1:
			if(VencioESPERA25ms){
 				Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,0);
				DHT_SETHIGH; //PONGO UN UNO A LA SALIDA
				TmrStartM(EVENTOM2,TM2);//ARRANCO TIMER QUE espera 30us
				estado=2;
			}
			break;
		case 2:
			if(VencioESPERA30us){
				DHT_ENTRADA;//LO HAGO ENTRADA
				TmrStartM(EVENTOM3,TM3);//ARRANCO TIMER QUE espera 20us
				estado=3;
			}
			break;
		case 3:
			if(VencioESPERA20us){
				TmrStartM(EVENTOM4,TM4);//ARRANCA MI TIMEOUT de 0,5s
				estado=4;
			}
			break;
		case 4:
			//Primero espero un uno...
			if(DHT_isHIGH)
			{
				estado=5;
				TmrStopM(EVENTOM4);//paro el timer 0,5s
				TmrStartM(EVENTOM4,TM4);//lo reinicio a 0,5s
			}//termina el 0 de 80us
			if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
			{
				estado=0;
				flagLeerMensajeNuevo=0;//termino lectura
				return 1; //DEVUELVO ERROR
			}
			break;
		case 5:
			if(DHT_isLOW) //espero un 0
			{
				estado=7; //tengo q ir esperar el 1
				TmrStopM(EVENTOM4);//paro el timer 0,5s

			}//termina el 1 de 80us
			if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
			{
				estado=0;
				flagLeerMensajeNuevo=0;//termino lectura
				return 1; //DEVUELVO ERROR
			}
			break;
		case 6:	//espera un 0
			if(DHT_isLOW)
			{
				estado=7;
			}
			if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
			{
				estado=0;
				flagLeerMensajeNuevo=0;//termino lectura
				return 1; //DEVUELVO ERROR
			}
			break;
		case 7: //espero un 1
			if(DHT_isHIGH)
			{
				estado=8;
				TmrStartM(EVENTOM5,TM5);//ARRANCA TIMER QUE espera 40us
			} //llego el 1 de transmision
			if(VENCIOTimeout)//sigo esperando o ya paso el tiempo?
			{
				estado=0;
				flagLeerMensajeNuevo=0;//termino lectura
				return 1; //DEVUELVO ERROR
			}
			break;
		case 8:
			if(VencioESPERA40us){
				if(DHT_isHIGH)
				{
					byte[i]|=(uint8_t)0x01<<j;//pongo un uno en la posicion
					estado=6; //espero q se haga cero
					TmrStartM(EVENTOM4,TM4);//lo reinicio a 0,5s
				}
				else{ //la lectura fue un 0, espero al proximo 1
					estado=7;
					TmrStartM(EVENTOM4,TM4);//lo reinicio a 0,5s
				}
				j--;
				if(j>7) //era 0 y ahora hizo overflow
				{
					i++;
					j=7;
					if(i==5){
						estado=9; //termino la lectura de los 5 bytes
					}
				}
			}
			break;
		case 9:
			checkmio=byte[0]+byte[1]+byte[2]+byte[3];
			checkmio&=0x00ff;
			//anido los datos
			hume=byte[0];
			hume=hume<<8;
			hume|=byte[1];
			temp=byte[2];
			temp=temp<<8;
			temp|=byte[3];
			check=byte[4];
			//checkeo si lo que me llego es correcto
			if(checkmio!=check)
			{
				estado=0;
				flagLeerMensajeNuevo=0;//termino lectura
				return 1; //suma incorrecta

			}
			//datos correctamente recibidos
			//estan en hume y temp
			// checkeo si la temperatura es negativa
			if(temp & 0x8000)//si esto es verdad es negativa
			{
				neg=1;//activo un flag
				temp=temp & 0x7FFF;//le saco el uno del bit mas significativo que indica negatividad
			}
			//decodifico la información
			dec_temp=temp%10;
			ent_temp=temp/10;
			dec_hume=hume%10;
			ent_hume=hume/10;
			Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);//prueba
			//La coloco en el buffer que manipularán otras funciones:
			Humestring[6] = '\0';//para que puea ser manipulado como string
			Humestring[5] = 'H';
			Humestring[4] = ' ';
			Humestring[3] = '0'+dec_hume;
			Humestring[2] = ',';
			for (i=1;i>=0 && i<=1;i--)
			{
			   	Humestring[i]='0'+ent_hume%10;
			   	ent_hume=ent_hume/10;
			}

			Tempstring[7] = '\0';//para que puea ser manipulado como string
			Tempstring[6] = 'C';
			Tempstring[5] = ' ';
			Tempstring[4] = '0'+dec_temp;
			Tempstring[3] = ',';
			for (i=2;i>=1 && i<=2;i--)
			{
			   	Tempstring[i]='0'+ent_temp%10;
			   	ent_temp=ent_temp/10;
			}
			if(neg==1)//si es negativo le agrego un menos
			{
				Tempstring[0]='-';
			}
			else{
				Tempstring[0]=' ';//si es positivo dejo un espacio
			}
			estado=0; //para la proxima vez que entre
			Chip_GPIO_WritePortBit(LPC_GPIO,LEDXpresso,1);//prueba
			i=0;
			j=7;
			byte[0]=byte[1]=byte[2]=byte[3]=byte[4]=0;
			flagLeerMensajeNuevo=0;//termino la lectura
			break;
	}
	return 0; //lectura correcta por el momento
}

void lec_pin_DHT(void){

	if(DHT_is==true){
		DHT_isHIGH=1;
		DHT_isLOW=0;
		}
	else if(DHT_is==false){
		DHT_isHIGH=0;
		DHT_isLOW=1;
	}
}
