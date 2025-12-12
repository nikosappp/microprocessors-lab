
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/cpufunc.h>	
#include <stdio.h>

void write_2_nibbles(uint8_t data){
	
	uint8_t temp;
	
	temp = PIND & 0x0F;                // κρατά PD0–PD3
	PORTD = temp | (data & 0xF0);      // βάζει high nibble στα PD4–PD7
	PORTD |= (1 << PD3); 
	_NOP(); 
	_NOP();
	PORTD &= ~(1 << PD3);
	
	
	temp = PIND & 0x0F;
	PORTD = temp | ((data << 4) & 0xF0); // βάζει low nibble
	PORTD |= (1 << PD3); 
	_NOP(); 
	_NOP();
	PORTD &= ~(1 << PD3);
}

void lcd_data(uint8_t data){
	
	PORTD |= (1 << PD2);    // RS=1 so it gets data 
	write_2_nibbles(data);
	_delay_us(250);

}

void lcd_command(uint8_t command){
	
	PORTD &= ~(1 << PD2);    // RS=0 so it gets command
	write_2_nibbles(command);
	_delay_us(250);

}

void lcd_clear_display(){
	
	lcd_command(0x01);
	_delay_ms(5);
	
}

void lcd_init(){
	
	_delay_ms(200);
	
	// repeat 3 times the command that switches to 8 bit mode
	for (uint8_t i = 0; i < 3; i++) {
		PORTD = 0x30;
		PORTD |= (1 << PD3);   // E = 1
		_NOP();
		_NOP();
		PORTD &= ~(1 << PD3);  // E = 0
		_delay_us(250);
	}
	
	// switch to 4 bit mode
	PORTD = 0x20;
	PORTD |= (1 << PD3);
	_NOP();
	_NOP();
	PORTD &= ~(1 << PD3);
	_delay_us(250);
	
	lcd_command(0x28);  // 5x8 dots, 2 lines
	lcd_command(0x0C);  // display on, cursor off
	lcd_clear_display(); // 
	lcd_command(0x06);  // Increase address, no display shift
	                    // this means that the cursor ismoving not the screen
}


int main(){
	
	// ADMUX[0-3]=0011 because the input is adc3
	// ADLAR=0 to right-adjust the ADC result
	// REFSn[1:0]=01 to select Vref= 5V
	// ADEN=1 => ADC Enable
	// ADPS[2:0]=111 => Fadc=16MHz/128=125KHz

	ADMUX  = (1 << REFS0) | (1 << MUX1) | (1 << MUX0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// the control pins of the LCD will be connected to portD, so we must make it behave as output

	DDRD = 0b11111111;
	
	lcd_init();
	// _delay_ms(100);

	while(1){
		
	//convertion
	ADCSRA |= (1 << ADSC);          // start conversion
	while (ADCSRA & (1 << ADSC));   // wait until done
	
	uint16_t adc_value = ADC;       // read the result (all 10 bits)
	float voltage = (adc_value * 5.0) / 1024.0;  // get te volatge
	
	// lcd prints only ascii, so we convert the float "voltage" into char
	char buffer[8];
	snprintf(buffer, sizeof(buffer), "%.2f", voltage);
	
	lcd_clear_display();


	lcd_data('V');
	lcd_data('=');
	for (uint8_t i = 0; buffer[i] != '\0'; i++) {
		lcd_data(buffer[i]);
	}
	
	_delay_ms(995); 
	 // 955 because, clear_display has 5ms delay

	}
}