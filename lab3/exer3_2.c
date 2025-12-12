#define F_CPU 16000000UL
#include<avr/io.h>
#include<util/delay.h>

int table[17] = {5, 20, 36, 51, 66, 82, 97, 112, 128, 
				143, 158, 173, 189, 204, 219, 235, 250};

int increase(int *i){
	if (*i == 16) return table[*i];
	while(!(PINB & (1 << PB4))) _delay_ms(5);
	return table[++(*i)];
}

int decrease(int *i){
	if (*i == 0) return table[*i];
	while(!(PINB & (1 << PB5))) _delay_ms(5);
	return table[--(*i)];
}


int main(){
	DDRB = 0b00000010;
	PORTB = 0b00110000;

	TCCR1A = (1<<COM1A1)|(1<<WGM10);
	TCCR1B = (1<<WGM12)|(1<<CS10);
	
	int pwm = 8;
	int DC_VALUE = table[pwm];
	
	OCR1AH = 0;
	OCR1AL = DC_VALUE;
	
	// ------------------
	
	int current = 0;
	int oldest = 0;
	int sum = 0;
	int avg = -1;
	int adc[16];
	for(int i=0; i<16; i++) adc[i]=0;
	
	DDRD = 0xFF;
	
	ADMUX = (1<<REFS0)|(1<<MUX0);
	ADCSRA = (1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);

	while(1){
		if(!(PINB & (1 << PB4))){
			DC_VALUE = increase(&pwm);
			OCR1AL = DC_VALUE;
		}
		
		if(!(PINB & (1 << PB5))){
			DC_VALUE = decrease(&pwm);
			OCR1AL = DC_VALUE;
		}
		
		// read input
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1 << ADSC));
		current = ADC;
		
		// calculate average
		sum += current - adc[oldest];
		adc[oldest] = current;
		if(++oldest>15) oldest=0;
		avg = sum>>4; // avg=sum/16
		
		// print on PORTD
		if (avg<=200) PORTD = 1;
		else if (avg<=400) PORTD = 2;
		else if (avg<=600) PORTD = 4;
		else if (avg<=800) PORTD = 8;
		else PORTD = 16;
		
		_delay_ms(100);
	}
}