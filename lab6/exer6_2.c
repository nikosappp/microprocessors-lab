#define F_CPU 16000000UL

#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define PCA9555_0_ADDRESS 0x40
#define TWI_READ    1
#define TWI_WRITE   0
#define SCL_CLOCK  100000L

#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2

volatile uint8_t state = 0;

#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3

typedef enum {
	REG_INPUT_0 = 0,
	REG_INPUT_1 = 1,
	REG_OUTPUT_0 = 2,
	REG_OUTPUT_1 = 3,
	REG_POLARITY_INV_0 = 4,
	REG_POLARITY_INV_1 = 5,
	REG_CONFIGURATION_0 = 6,
	REG_CONFIGURATION_1 = 7
} PCA9555_REGISTERS;

//----------- Master Transmitter/Receiver -------------------
#define TW_START 0x08
#define TW_REP_START 0x10

//---------------- Master Transmitter ----------------------
#define TW_MT_SLA_ACK 0x18
#define TW_MT_SLA_NACK 0x20
#define TW_MT_DATA_ACK 0x28

//---------------- Master Receiver ----------------
#define TW_MR_SLA_ACK 0x40
#define TW_MR_SLA_NACK 0x48
#define TW_MR_DATA_NACK 0x58

#define TW_STATUS_MASK  0b11111000
#define TW_STATUS (TWSR0 & TW_STATUS_MASK)

//initialize TWI clock
void twi_init(void)
{
	TWSR0 = 0;
	TWBR0 = TWBR0_VALUE;
}

// Read one byte from the twi device (request more data from device)
unsigned char twi_readAck(void)
{
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR0 & (1<<TWINT)));
	return TWDR0;
}

//Read one byte from the twi device, read is followed by a stop condition
unsigned char twi_readNak(void)
{
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR0 & (1<<TWINT)));
	return TWDR0;
}

// Issues a start condition and sends address and transfer direction.
// return 0 = device accessible, 1= failed to access device
unsigned char twi_start(unsigned char address){
	
	uint8_t   twi_status;
	// send START condition
	TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait until transmission completed
	while(!(TWCR0 & (1<<TWINT)));
	
	// check value of TWI Status Register.
	twi_status = TW_STATUS & 0xF8;
	if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) return 1;
	
	// send device address
	TWDR0 = address;
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	
	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR0 & (1<<TWINT)));
	
	// check value of TWI Status Register.
	twi_status = TW_STATUS & 0xF8;
	if ( (twi_status != TW_MT_SLA_ACK) && (twi_status != TW_MR_SLA_ACK) )
	{
		return 1;
	}
	
	return 0;
	
}

// Send start condition, address, transfer direction.
// Use ack polling to wait until device is ready
void twi_start_wait(unsigned char address)
{
	uint8_t   twi_status;
	
	while ( 1 )
	{
		// send START condition
		TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
		
		// wait until transmission completed
		while(!(TWCR0 & (1<<TWINT)));
		
		// check value of TWI Status Register.
		twi_status = TW_STATUS & 0xF8;
		if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) continue;
		
		// send device address
		TWDR0 = address;
		TWCR0 = (1<<TWINT) | (1<<TWEN);
		
		// wail until transmission completed
		while(!(TWCR0 & (1<<TWINT)));
		
		// check value of TWI Status Register.
		twi_status = TW_STATUS & 0xF8;
		if ( (twi_status == TW_MT_SLA_NACK )||(twi_status ==TW_MR_DATA_NACK) )
		{
			/* device busy, send stop condition to terminate write operation */
			TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
			
			// wait until stop condition is executed and bus released
			while(TWCR0 & (1<<TWSTO));
			
			continue;
		}
		break;
	}
}

//  Send one byte to twi device,  Return 0 if write successful or 1 if write failed
unsigned char twi_write( unsigned char data )
{
	// send data to the previously addressed device
	TWDR0 = data;
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	// wait until transmission completed
	while(!(TWCR0 & (1<<TWINT)));
	if( (TW_STATUS & 0xF8) != TW_MT_DATA_ACK) return 1;
	return 0;
}

// Send repeated start condition, address, transfer direction
//Return:  0 device accessible
//1 failed to access device
unsigned char twi_rep_start(unsigned char address)
{
	return twi_start( address );
}

// Terminates the data transfer and releases the twi bus
void twi_stop(void)
{
	// send stop condition
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	// wait until stop condition is executed and bus released
	while(TWCR0 & (1<<TWSTO));
}

void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
	twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
	twi_write(reg);
	twi_write(value);
	twi_stop();
}

uint8_t PCA9555_0_read(PCA9555_REGISTERS reg)
{
	uint8_t ret_val;
	twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
	twi_write(reg);
	twi_rep_start(PCA9555_0_ADDRESS + TWI_READ);
	ret_val = twi_readNak();
	twi_stop();
	return ret_val;
}


void write_2_nibbles(uint8_t data)
{
	uint8_t temp;
	uint8_t state;

	temp = PCA9555_0_read(REG_OUTPUT_0) & 0x0F;
	state = temp | (data & 0xF0);
	PCA9555_0_write(REG_OUTPUT_0, state);

	state |= (1 << PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);
	_delay_ms(1);
	state &= ~(1 << PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);

	temp = PCA9555_0_read(REG_OUTPUT_0) & 0x0F;
	state = temp | ((data << 4) & 0xF0);
	PCA9555_0_write(REG_OUTPUT_0, state);

	state |= (1 << PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);
	_delay_ms(1);
	state &= ~(1 << PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);
}

void lcd_command(uint8_t command){
	state &= ~(1<<PD2);
	PCA9555_0_write(REG_OUTPUT_0, state);
	write_2_nibbles(command);
	_delay_us(250);
}

void lcd_data(uint8_t data){
	state |= (1<<PD2);
	PCA9555_0_write(REG_OUTPUT_0, state);
	write_2_nibbles(data);
	_delay_us(250);
}

void lcd_clear_display(){
	lcd_command(1);
	_delay_ms(5);
}

void lcd_init(){
	
	_delay_ms(200);
	uint8_t state;
	
	for (int i=0; i<3; i++){
		state = 0x30;
		PCA9555_0_write(REG_OUTPUT_0, state);
		state |= (1<<PD3);
		PCA9555_0_write(REG_OUTPUT_0, state);
		_delay_ms(1);
		state &= ~(1<<PD3);
		PCA9555_0_write(REG_OUTPUT_0, state);
		_delay_us(250);
	}
	
	state = 0x20;
	PCA9555_0_write(REG_OUTPUT_0, state);
	state |= (1<<PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);
	_delay_ms(1);
	state &= ~(1<<PD3);
	PCA9555_0_write(REG_OUTPUT_0, state);
	_delay_us(250);
	
	lcd_command(0x28);
	lcd_command(0x0C);
	lcd_clear_display();
	lcd_command(0x06);
}

uint8_t scan_row(uint8_t num_row){
	
	uint8_t row = ~(1 << num_row);
	
	// set the row we want to search to 0 so when one of its keys are pressed the change form 1 to 0
	PCA9555_0_write(REG_OUTPUT_1, row);
	
	// then read IO1's value to check IO4-IO7
	uint8_t keys = PCA9555_0_read(REG_INPUT_1);
	keys = keys >> 4; // put the IO4-IO7 in the first half
	
	return keys;
}


uint16_t scan_keypad(){
	
	uint16_t scan_results=0;
	
	// scan from top to bottom
	for(int i=3; i>-1; i--){
		scan_results = scan_results << 4;
		scan_results |= scan_row(i);
	}
	return scan_results;
}

uint16_t pressed_keys=0; // global variable

uint16_t scan_keypad_rising_edge(){
	
	
	uint16_t pressed_keys_tempo = scan_keypad();
	_delay_ms(20);
	uint16_t second_scan = scan_keypad();
	
	pressed_keys_tempo &= second_scan;    // keep only the same keys in the 2 scans
	uint16_t new_keys = pressed_keys_tempo & ~pressed_keys;
	pressed_keys=pressed_keys_tempo;
	
	return new_keys;

}

char keypad_to_ascii(uint16_t value)
{
	// find which bit is 1
	for (uint8_t i = 0; i < 16; i++)
	{
		if (value & (1 << i))   // if the i-th bit is one go to the i-th case
		{
			switch (i)
			{
				
				case 15: return 'A'; 
				case 14: return '3'; 
				case 13: return '2'; 
				case 12: return '1'; 

				case 11: return 'B'; 
				case 10: return '6'; 
				case 9:  return '5'; 
				case 8:  return '4'; 

				case 7:  return 'C'; 
				case 6:  return '9'; 
				case 5:  return '8'; 
				case 4:  return '7'; 

				case 3:  return 'D';
				case 2:  return '#'; 
				case 1:  return '0'; 
				case 0:  return '*'; 
				
				default: return 0;
			}
		}
	}

	return 0; // nothing is pressed
}


int main(){
	
	twi_init();
	PCA9555_0_write(REG_CONFIGURATION_0, 0b00000000);   // we set IO0 as output
	PCA9555_0_write(REG_CONFIGURATION_1, 0b11110000);   // we set IO1_4-IO1_7 (columns) as input and IO1_0-IO1_3 (rows) as outputs
	
	pressed_keys = scan_keypad();
	
	lcd_init();
	
	
	while(1){
		
		uint16_t key =scan_keypad_rising_edge();
		
		if(key==0) continue;
		
		lcd_clear_display();
		lcd_data((uint8_t)keypad_to_ascii(key));
	}
}

