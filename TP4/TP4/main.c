
#include <avr/io.h>
#define F_CPU 16000000UL
#define B9600 103
#include <util/delay.h>
#include <stdint.h>
#include "serialPort.h"


volatile uint8_t color='R';
volatile uint8_t aux;

int main(void)
{
	//CONFIGURACION DEL ADC3
	ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<REFS0); // Seteo ADC3 y FUENTE = VCC 
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // Habilito ADC, y prescaler=128 con 16Mhz para 125Khz
	DDRC&=~(1<<PORTC3);
	DIDR0= 0x01;
	//CONFIGURACION DEL TIMER1
	TCCR1A |= (1 << WGM10 | 1 << COM1A0 | 1 << COM1A1 | 1<< COM1B0 | 1<<COM1B1); // habilito el timer modo PWM fase correcta 8 bits y el modo invertido del pwm
	TCCR1B |= (1 << CS12); // prescaler 128
	OCR1A = 0;
	OCR1B = 0;
	DDRB |= (1 << PORTB1 | 1 << PORTB5 | 1 << PORTB2);
	
	//CONFIGURACION DEL UART
	SerialPort_Init(B9600);
	SerialPort_RX_Enable();
	SerialPort_TX_Enable();
	SerialPort_RX_Interrupt_Enable();
	sei();
	//VARIABLES
	SerialPort_Send_String("Bienvenido. Presione R/G/B para modificar el color\r\n");
	uint8_t topRed = 0;
	uint16_t valorPot = 0;
	ADCSRA|=(1<<ADSC);  // inicializo conversion
    while (1) 
    {	
		if(topRed > TCNT1)				
			PORTB &= ~(1<< PORTB5);
		else
			PORTB |= (1<<PORTB5);
		//LEO VALOR DEL POTE
		if(ADCSRA&(1<<ADIF)){ // si termino la conversion
			valorPot=(ADC/4);
			ADCSRA|=(1<<ADIF);
			ADCSRA|=(1<<ADSC);
		}
		switch(color){
			case 'R':
				topRed=valorPot;
				//SETEO EN TOPRED
				break;
			case 'B':
				OCR1A=valorPot;
				//LO SETEO EN OCR1B
				break;
			case 'G':
				//SETEO EN OCR1A
				OCR1B=valorPot;
				break;	
		}
		_delay_us(16);		
    }
}

ISR(USART_RX_vect){
	aux = UDR0;
	if(aux == 'R' || aux == 'G' || aux == 'B')
		color=aux;	
}
