#include <avr/io.h>
#include <util/delay.h>

#define PB1 1
#define PB4 4
#define PB5 5
#define PD0 0
#define PD1 1


uint8_t dc_table[] = {5, 20, 36, 51, 66, 82, 97, 112, 128, 143, 158, 174, 189, 204, 220, 235, 250};
	            // %: 2   8  14  20  26  32  38  44   50   56   62   68   74   80   86   92   98
#define dc_values 17

int main(){
	
	// set TMR1A in fast 8 bit mode with non-inverted output 
	
	// fpwm = fclk / N(1+255) where N is the prescaler
	// to achieve fpwm = 62.500 we do:
	// 62.500 = 16.000.000 / (N + 255) => N=1
	// so the working frequency of the TCNT1 is clk/1 (CS10=1)
	
	TCCR1A = (1 << WGM10) | (1 << COM1A1);
	TCCR1B = (1 << WGM12) | (1 << CS10);
	
	// ADMUX[0-3]=0000 because the input is adc0, ADLAR=1 to left-adjust the ADC result 
	// REFSn[1:0]=01 to select Vref= 5V
	
	// ADEN=1 => ADC Enable
	// ADPS[2:0]=111 => Fadc=16MHz/128=125KHz (i used the frequency of the example in theory)
	
	ADMUX  = (1 << REFS0) | (1 << ADLAR);         
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 

	// Ι/Ο 

	DDRD &= ~((1 << PD0) | (1 << PD1)); // set PD0, PD1 as input
	PORTD |= (1 << PD0) | (1 << PD1);   // enable pull up resistors
	
	DDRB |= (1 << PB1);                  // set PB1 as output
	
	DDRB &= ~((1 << PB4) | (1 << PB5));     // set PB4, PB5 as input
	PORTB |= (1 << PB4) | (1 << PB5);    // and enable pull up resistors
	
	// for mode1
	
	uint8_t dc_index = 8;           // start at 50% (9th element)
	OCR1A = dc_table[dc_index];
	uint8_t DC_VALUE = dc_table[dc_index];
	
	// for mode2
	
	uint8_t mode = 1;
	
	
	/*
	
	--- how it works ---
	
	MODE1: The buttons increase or decrease the duty cycle and the brightness follows 
	
	MODE2: At first the potentiometer increases the amplitude of the analog signal (DC voltage).
		   Then it passes this signal though the ADC, making it digital and puts the result om the OCR1A register that controls the duty cycle.
		   As the amplitude increases the number inside OCR1A increases as well, giving higher duty cycle and as a result brighter light 
	
	*/

	while(1){
		 
		// PD0 => mode 1
		if (!(PIND & (1 << PD0))){
			mode = 1;
			_delay_ms(80);

			// reset brightness at 50% every time we switch modes
			dc_index = 8;
			DC_VALUE = dc_table[dc_index];
			OCR1A = DC_VALUE;
		}

		// PD1 => mode 2
		if (!(PIND & (1 << PD1))){
			mode = 2;
			_delay_ms(80);
		}
		
		// mode 1: control with buttons
		
		if (mode == 1) {
			if (!(PINB & (1 << PB4))){          
				if (dc_index < dc_values - 1) {
					dc_index++;
					DC_VALUE = dc_table[dc_index];
					OCR1A = DC_VALUE;
				}
				while (!(PINB & (1 << PB4)));  // wait until release
				_delay_ms(50);
			}

			if (!(PINB & (1 << PB5))) {         
				if (dc_index > 0) {
					dc_index--;
					DC_VALUE = dc_table[dc_index];
					OCR1A = DC_VALUE;
				}
				while (!(PINB & (1 << PB5)));  // wait until release
				_delay_ms(50);
			}
		}
		
		// control with potentiometer
		
		else if (mode == 2) {
			ADCSRA |= (1 << ADSC);          // start conversion           
			while (ADCSRA & (1 << ADSC));   // wait until done
			OCR1A = ADCH;                   // use the 8 higher bits for PWM's duty cycle   
			_delay_ms(80);                       
		}
	}
}

	
