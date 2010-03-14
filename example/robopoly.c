/***************************************************************************************
 *
 * Libraire C de Robopoly v1.0
 * Fichier: robopoly.c
 * Date: 13.08.2008 
 * Auteur(s): Christophe Winter, Thierry Barras
 * 
 * Cette librairie a été crée par le club de robotique de l'EPFL Robopoly. 
 * Cette librairie est distribuée gratuitement aux membres de Robopoly et les sources 
 * appartiennent à Robopoly.
 *
 ***************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay_basic.h>
#include "robopoly.h"
#include <util/setbaud.h>


void digitalWrite(unsigned char port, unsigned char bit, unsigned char value)
{
	
	if(bit == 0xFF)
	{
		switch(port)
		{
		case 'A':
			DDRA = 0xFF;
			PORTA = value;
			return;
		case 'B':
			DDRB = 0xFF;
			PORTB = value;
			return;
		case 'C':
			DDRC = 0xFF;
			PORTC = value;
			return;
		case 'D':
			DDRD = 0xFF;
			PORTD = value;
			return;
		}
	}
	else
	{
		value = value << bit;
		switch(port)
		{
		case 'A':
			DDRA |= (1<<bit);
			PORTA = (PORTA & (~(1<<bit))) + value;
			return;
		case 'B':
			DDRB |= (1<<bit);
			PORTB = (PORTB & (~(1<<bit))) + value;
			return;
		case 'C':
			DDRC |= (1<<bit);
			PORTC = (PORTC & (~(1<<bit))) + value;
			return;
		case 'D':
			DDRD |= (1<<bit);
			PORTD = (PORTD & (~(1<<bit))) + value;
			return;
		}
	}
}


unsigned char digitalRead (unsigned char port, unsigned char bit)
{
	if(bit == 0xFF)
	{
		switch(port)
		{
		case 'A':
			DDRA = 0;
			return PINA;
		case 'B':
			DDRB = 0;
			return PINB;
		case 'C':
			DDRC = 0;
			return PINC;
		case 'D':
			DDRD = 0;
			return PIND;
		}
	}
	else
	{
		switch(port)
		{
			case 'A':
				DDRA &= ~(1<<bit);
				return (PINA >> bit) & 1;
			case 'B':
				DDRA &= ~(1<<bit);
				return (PINB >> bit) & 1;
			case 'C':
				DDRA &= ~(1<<bit);
				return (PINC >> bit) & 1;
			case 'D':
				DDRA &= ~(1<<bit);
				return (PIND >> bit) & 1;
		}
	}
	return 0;
}


unsigned char analogReadPortA(unsigned char bit)
{
	unsigned char result;
	DDRA &= ~(1<<bit); 
	ADCSRA = 0x87; 
	ADMUX =0x20+bit; 
	ADCSRA |= (1<<ADSC); 
	while((ADCSRA & (1<<ADSC))>>ADSC);
	result = ADCH; 
	ADCSRA = 0;
	return result;
}

void waitms(unsigned int iter)
{
	for(;iter; iter--)
	{
		_delay_loop_2(2000);
	}
}

void waitus(unsigned char iter)
{
	iter = iter >> 2;
	for(; iter; iter--)
	{
		_delay_loop_1(3);
	}
}


// fonction bloquante tant que buffer d'envoi n'est pas prêt pour recevoir le nouveau byte
static unsigned char uartStatus = 1;
void uartSendByte(unsigned char a)
{
	if(uartStatus)
	{
		UBRRH = UBRRH_VALUE;
		UBRRL = UBRRL_VALUE;
		#if USE_2X
		UCSRA |= (1 << U2X);
		#else
		UCSRA &= ~(1 << U2X);
		#endif
		UCSRB |= 0x18;
		uartStatus = 0;
	}

	while((UCSRA & (1<<UDRE))==0);
	UDR = a;
}

void uartSendString(const char *text)
{
	unsigned int i;
	for(i=0; text[i] != '\0'; i++)
	{
		uartSendByte((unsigned char)text[i]);
	}
}

// Fonction bloquante tant qu'aucun valeur reçue sur le bus
unsigned char uartGetByte(void)
{
	if(uartStatus)
	{
		UBRRH = UBRRH_VALUE;
		UBRRL = UBRRL_VALUE;
		#if USE_2X
		UCSRA |= (1 << U2X);
		#else
		UCSRA &= ~(1 << U2X);
		#endif
		UCSRB |= 0x18;
		uartStatus = 0;
	}

	while((UCSRA & (1<<RXC)) == 0);
	return UDR;
}



void setupMotorPWM(int vLeft, int vRight)
{
	DDRD |= 0xF0;	// PD4-7 as output
	TCCR1A = 0x03;	// PWM, phase correct, 10bits
	TCCR1B = 0x01;

	if(vLeft==0)
	{
		TCCR1A &= 0x3F;
		PORTD &= ~(1<<PD5);
	}
	else
	{
		TCCR1A |= 0x80;
		if(vLeft < 0)
		{
			vLeft = -vLeft;
			PORTD |= (1<<PD7);
		}
		else
		{
			PORTD &= ~(1<<PD7);
		}

		vLeft = (10*vLeft); // 1023*vLeft/100
	}

	if(vRight==0)
	{
		TCCR1A &= 0xCF;
		PORTD &= ~(1<<PD4);
	}
	else
	{
		TCCR1A |= 0x20;
		if(vRight < 0)
		{
			vRight = -vRight;
			PORTD |= (1<<PD6);
		}
		else
		{
			PORTD &= ~(1<<PD6);
		}

		vRight = (10*vRight); // 1023*vLeft/100
	}

	OCR1AH = (vLeft & 0xFF00)>>8;
	OCR1AL = vLeft & 0xFF;
	OCR1BH = (vRight & 0xFF00)>>8;
	OCR1BL = vRight & 0xFF;
}



#ifndef	noagenda

void(*callbackFct[8])(void);
static volatile unsigned char callbackStatus = 0;
static volatile unsigned int timeInterval[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned char numberRepetition[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned char numberRepeted[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned long nextExecutionTime[8] = {0,0,0,0,0,0,0,0};
static volatile unsigned long time = 0;

// max absolute time: 8589934sec
// time resolution ~2msec
char addNewCallback(void (* newcallbackaddr)(void), unsigned int duration, unsigned char executionNumber)
{
	unsigned char i;

	if(callbackStatus == 0)
	{
		//start agenda (again) !
		TCCR0 = 3;	// normal timer, fclk/64
		OCR0 = 250;
		TIMSK |= (1<<OCIE0);//(1<<TOIE0);
		TIFR &= ~((1<<OCF0)+(1<<TOV0));
		sei();
	}

	for(i=0; i<8; i++)
	{
		if((callbackStatus & (1<<i)) != 0)
		{
			continue;
		}

		callbackStatus |= (1<<i);
		callbackFct[i] = newcallbackaddr;
		timeInterval[i] = duration;
		numberRepetition[i] = executionNumber;
		numberRepeted[i] = 0;
		nextExecutionTime[i] = time + duration;
		return i;	
	}

	return -1;
}

void stopCallback(char callbackNumber)
{
	callbackStatus &= ~(1<<callbackNumber);
	if(callbackStatus == 0)
	{
		TIMSK &= ~(1<<TOIE0);
		TCCR0=0;
	}
}

ISR(TIMER0_COMP_vect) //interruption bloquante !!
{
	unsigned char i;
	TCNT0 = 0;
	time++;
	for(i=0;i<8;i++)
	{
		if((callbackStatus & (1<<i)) != 0)
		{
			if(nextExecutionTime[i] <= time)
			{
				if((numberRepeted[i] < numberRepetition[i]) || (numberRepetition[i]==0))
				{
					(* callbackFct[i])();
					nextExecutionTime[i] = time + timeInterval[i];
						
					if((numberRepeted[i] >= (numberRepetition[i]-1)) && (numberRepetition[i] != 0))
					{
						callbackStatus &= ~(1<<i);
					}
					else
					{	
						numberRepeted[i]++;
					}
					break;
				}
			}
		}
	}
}

#endif





#ifndef noservo
//FONCTION POUR LES SERVOS
static volatile unsigned char ServoStatus = 0;
char angle_servos[10]= {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
unsigned char num_servo;

//La fonction set_servos permet de controller 10 servomoteur placés physiquement sur les lignes definies dans robopoly.h

void set_servo(unsigned char num_servo, char angle_servo)

{
	if (ServoStatus==0)  	
	{
		ServoStatus	= 1;	// initialisation uniquement lors du premier appel de la fct set_servo
		TCCR2 		= 5;	// normal timer, fclk/128 => resolution 16us
		OCR2 		= 250;  // a regler pour chaque servo entre 125 (1ms) et 250 (2ms). 125 correspondant à 0% et 250 à 100%
		TCNT2		= 100;	// pour arriver à 2ms on compte de 100 à 255 = 155 cycles à 16us = 2.48ms
		TIFR 		&= ~((1<<OCF2)+(1<<TOV2));	//MAZ des flags
		TIMSK 		|=  (1<<OCIE2)+(1<<TOIE2); //activation des interrupts COMP et OVF du timer 2
		sei(); 				//Active les interruptions globales
	}

	if ((angle_servo<101)&&(angle_servo>=0))
	{
		angle_servos[num_servo] = (angle_servo)+150; 	
		// OCR2-TCNT2 = nbre de cycle avec la ligne du servo à 1 (logique)
		// ex:  angle = 0,   COMPARE après  50cycles à 16us = 0.8ms
		// ex:  angle = 100, COMPARE après 150cycles à 16us = 2.4ms

		switch (num_servo)		
		{
			case 0:		SERVO_0_DIR = 1; break;
			case 1:	 	SERVO_1_DIR = 1; break;
			case 2:		SERVO_2_DIR = 1; break;
			case 3:	 	SERVO_3_DIR = 1; break;
			case 4:		SERVO_4_DIR = 1; break;
			case 5:	 	SERVO_5_DIR = 1; break;
			case 6:		SERVO_6_DIR = 1; break;
			case 7:	 	SERVO_7_DIR = 1; break;
			case 8:		SERVO_8_DIR = 1; break;
			case 9:	 	SERVO_9_DIR = 1; break;
		}
	}
}


ISR(TIMER2_COMP_vect) //interruption bloquante !!  Mise de zéro des lignes des servos jusqu'au COMP suivant
{
	if (angle_servos[num_servo]  != -1)
	{
		switch (num_servo)
		{
			case 0:		SERVO_0 = 0; break;
			case 1:	 	SERVO_1 = 0; break;
			case 2:		SERVO_2 = 0; break;
			case 3:	 	SERVO_3 = 0; break;
			case 4:		SERVO_4 = 0; break;
			case 5:	 	SERVO_5 = 0; break;
			case 6:		SERVO_6 = 0; break;
			case 7:	 	SERVO_7 = 0; break;
			case 8:		SERVO_8 = 0; break;
			case 9:	 	SERVO_9 = 0; break;
		}
	}
}


ISR(TIMER2_OVF_vect) //interruption bloquante !! Mise à un des lignes des servos jusqu'au OVF suivant
{
	TCNT2	= 100;	// pour arriver à 2.48ms on compte de 100 à 255 (=155 cycles de 16us)
	num_servo++;
	if (num_servo>=10) num_servo=0;

	
	//Reglage du temps que la ligne va rester à 1.
	OCR2 = angle_servos[num_servo];


	if (angle_servos[num_servo]  != -1)
	{
	switch (num_servo)
		{
			case 0:		SERVO_0 = 1; break;
			case 1:	 	SERVO_1 = 1; break;
			case 2:		SERVO_2 = 1; break;
			case 3:	 	SERVO_3 = 1; break;
			case 4:		SERVO_4 = 1; break;
			case 5:	 	SERVO_5 = 1; break;
			case 6:		SERVO_6 = 1; break;
			case 7:	 	SERVO_7 = 1; break;
			case 8:		SERVO_8 = 1; break;
			case 9:	 	SERVO_9 = 1; break;
		}
	}
}
#endif


