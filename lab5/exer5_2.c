#define F_CPU 16000000UL
#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#define PCA9555_0_ADDRESS 0x40
#define TWI_READ    1
#define TWI_WRITE   0
#define SCL_CLOCK  100000L

#define TWBR0_VALUE ((F_CPU/SCL_CLOCK)-16)/2

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

int main(){
	
	//DDRD = 0b00001111; //make PD0-PD3 outputs
	
	twi_init();
	PCA9555_0_write(REG_CONFIGURATION_0, 0b11110000);   // we set IO0_0-IO0_3 as outputs
	PCA9555_0_write(REG_CONFIGURATION_1, 0b11110111);   // we set IO1_4-IO1_7 as input
	// Make IO1_3 (the row for keys "1", "2", "3", "A") output LOW.
	// The keypad works by pulling a column to 0 when a key is pressed.
	// Columns IO1_4–IO1_7 have pull-ups (they stay at 1).
	// When a key is pressed, it connects IO1_3 (0) to one column,
	// so that column becomes 0 and we can detect the key.
	PCA9555_0_write(REG_OUTPUT_1, 0b11110111);          
	
	
	// i set the pins that we do not use as inputs because in this case they are connected 
	// they are connected via pull up resistors to Vdd and they are not floating 
	
	uint8_t keys;
	uint8_t leds;
	
	while(1){
		
		keys = PCA9555_0_read(REG_INPUT_1); // read the pins IO1_4-IO1_7 and send the byte to avr
		 
		// based on the byte the avr received activates the proper led
		if (!(keys & (1 << 4))){
			 leds = (1 << 0);      // led 0
		}
		else if (!(keys & (1 << 5))){
			 leds = (1 << 1);    // led 1
		}
		else if (!(keys & (1 << 6))){
			 leds = (1 << 2);   // led 2
		}
		else if (!(keys & (1 << 7))) {
			 leds = (1 << 3);   // led 3
		}
		else {
			leds=0x00;
		} 
		
		PCA9555_0_write(REG_OUTPUT_0, leds); // send the info from avr to the PCA9555 to turn on the leds
		
	}
}