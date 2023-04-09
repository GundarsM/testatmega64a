/*
 * testatmega64a.c
 *
 * Created: 3/17/2023 1:38:50 PM
 * Author : GundarsMiezitis
 */ 


/*
atmega103 compatability must be turned off
FUSES must be set to:
	EXTENDED 0XFF
	HIGH 0X91
	LOW 0XEF
*/

#define F_CPU 16000000UL
#define BAUD 9600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
volatile uint16_t adcresult = 0;
char text[50] = "";

ISR(ADC_vect)
{
	adcresult = ADCL | (ADCH<<8);
	//char i = (char)(adcresult/127.0);
	PORTA = (0xff<<(char)(adcresult/127.0));	//turn on 8 LEDS
	OCR0 = adcresult/4;							//dim PWM LED
}	

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) )
	;
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void write (char text[]) {
	
	for (int i = 0; ; i++) {
		USART_Transmit(text[i]);
	if (! text[i + 1]) break;}
	USART_Transmit('\n');
	USART_Transmit('\r');
}



int main(void)
{
	/* LEDs */
	DDRA = 0xff;

	char data =0x01;
	
	/* ADC for one channel, continious */
	ADMUX |= (0 << REFS1) | (1 << REFS0);
	ADMUX |= (0 << ADLAR);
	ADMUX |= (0 << MUX2)|(1 << MUX1)|(1 << MUX0);

	ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0);
	ADCSRA |= (1 << ADATE);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADSC);	
	ADCSRB = 0;
	
	DDRD = 0b00001000;
	
	/* USART */
	uint16_t ubrr = 103; //F_CPU/16*BAUD-1
	
	 UBRR1H = (unsigned char)(ubrr>>8);
	 UBRR1L = (unsigned char)ubrr;
	 /*Enable receiver and transmitter */
	 UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	 /* Set frame format: 8data, 2stop bit */
	 UCSR1C = (1<<USBS1)|(3<<UCSZ10);	
	/* TIMER0 for PWM */
	DDRB=(1<<DDD4);
	TCCR0 = (1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS01);		
	sei();
	
	//USART_Transmit('d'); // test USART
	write("Begin"); 
	
	/* 7 segment LEDs */
	
	DDRC |= 0b11111000;
	DDRE |= 0b11111100;
	DDRD |= 0b11110000;
	DDRG |= 0b00000100;
	
	/*
	PORTC |= 0b10000000;//2D
	PORTC |= 0b01000000;//NESTRAADAA - 1DP
	PORTC |= 0b00100000;//1C
	PORTC |= 0b00010000;//NESTRAADAA - 1B
	PORTC |= 0b00001000;//1A
	PORTE |= 0b10000000;//2G
	PORTE |= 0b01000000;//2F
	PORTE |= 0b00100000;//1D
	PORTE |= 0b00010000;//NESTRAADAA - 1E
	PORTE |= 0b00001000;//NESTRAADAA - 1G
	PORTE |= 0b00000100;//NESTRAADAA - 1F
	PORTD |= 0b10000000;//NESTRAADAA - 2DP
	PORTD |= 0b01000000;//2C
	PORTD |= 0b00100000;//2B
	PORTD |= 0b00010000;//2A
	PORTG |= 0b00000100;//2E
	*/
	PORTC |= 0b00000000;
	PORTE |= 0b00000000;
	PORTD |= 0b00000000;
	PORTG |= 0b00000000;
	
    while (1) 
    {
		//PORTA = data; 
		//data = data <<1;
		//_delay_ms(500);
		//if(data==0) data = 0x01;
		//if(PINC&0x01) PORTA = 0x1;
		//else PORTA = 0;
		//PORTA = PINC;
		sprintf(text,"ADC:%d, LED:%d",adcresult,(int)(adcresult/127.0));
		write(text);
		//PORTA = 0X55;
    }
}

