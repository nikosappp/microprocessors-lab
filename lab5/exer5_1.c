#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>

#define F_CPU 16000000UL
#define PCA9555_0_ADDRESS 0x40
#define TWI_READ 1
#define TWI_WRITE 0
#define SCL_CLOCK 100000L   // frequency of twi
//in order to have the above frequency we set:
#define TWBR0_VALUE 72

//pca9555 registers
typedef enum {
	REG_INPUT_0            = 0,
	REG_INPUT_1            = 1,
	REG_OUTPUT_0           = 2,
	REG_OUTPUT_1           = 3,
	REG_POLARITY_INV_0     = 4,
	REG_POLARITY_INV_1     = 5,
	REG_CONFIGURATION_0    = 6,
	REG_CONFIGURATION_1    = 7
} PCA9555_REGISTERS;

#define TW_START 0x08    // start status code
#define TW_REP_START 0x10    // repeated start status code

// master transmitter
#define TW_MT_SLA_ACK       0x18    // avr sent slave's adress + W and slave replied with ack
#define TW_MT_SLA_NACK      0x20    // avr sent slave's adress + W and slave replied with nack
#define TW_MT_DATA_ACK      0x28    // avr sent a data byte and slave replied with ack


#define TW_STATUS_MASK      0b11111000
#define TW_STATUS           (TWSR0 & TW_STATUS_MASK)


//initialize twi clock
void twi_init(){
	TWSR0=0; // prescaler = 0
	TWBR0 = TWBR0_VALUE; // 100 khz
}


unsigned char twi_start(unsigned char address){
	
	uint8_t twi_status;
	
	//send start condition
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	//wait until transmission completed
	while(!(TWCR0 & (1<<TWINT)));
	
	// check value of TWI Status Register.
	// if it is not the value of "start" or "repeated start" return with 1
	twi_status = TW_STATUS & 0xF8;
	if ( (twi_status != TW_START) && (twi_status != TW_REP_START)) return 1;
	
	//send device address
	TWDR0 = address;
	TWCR0 = (1<<TWINT) | (1<<TWEN);
	
	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR0 & (1<<TWINT)));
	
	// check value of TWI Status Register.
	// if avr sent slave's adress + W and slave did not reply with ack
	twi_status = TW_STATUS & 0xF8;
	if (twi_status != TW_MT_SLA_ACK) return 1;
	
	
	// if nothing was wrong
	return 0;
}


// next function is the polling version of the previous one. if it gets nack it retries
// we define it in case the simple one does not work

void twi_start_wait(unsigned char address)
{
	uint8_t twi_status;

	while (1)  // loop until we get ACK
	{
		// send start
		TWCR0 = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

		// wait
		while (!(TWCR0 & (1<<TWINT)));

		// status check
		twi_status = TW_STATUS;
		if ((twi_status != TW_START) && (twi_status != TW_REP_START))
		continue; // something is wrong try again (next loop)

		// send slaves address
		TWDR0 = address;
		TWCR0 = (1<<TWINT) | (1<<TWEN);

		// wait
		while (!(TWCR0 & (1<<TWINT)));

		// status check
		twi_status = TW_STATUS;
		if ((twi_status == 0x20) || (twi_status == 0x48))
		{
			// slave did not answer, send stop and retry
			TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
			while (TWCR0 & (1<<TWSTO)); // wait for stop
			continue;
		}

		// if nothing was wrong
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

// Terminates the data transfer and releases the twi bus
void twi_stop(void){
	
	// send stop
	TWCR0 = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	//wait
	while(TWCR0 & (1<<TWSTO));
}


void PCA9555_0_write(PCA9555_REGISTERS reg, uint8_t value)
{
	twi_start_wait(PCA9555_0_ADDRESS + TWI_WRITE);
	twi_write(reg);
	twi_write(value);
	twi_stop();
}

int main(void){
	
	uint8_t A, B, C, D;
	uint8_t F0, F1, output;

	DDRB = 0b00000000;   // PORTB[3:0] inputs
	//PORTB |= 0x0F;// enable pull up resistors
	
	twi_init();
	PCA9555_0_write(REG_CONFIGURATION_0, 0b11111100); // we set IO0_0 and IO0_1 of PCA9555 as outputs

	while (1)
	{
		uint8_t val = PINB & 0x0F;  // read input

		A = ~(val >> 0) & 1;
		B = ~(val >> 1) & 1;
		C = ~(val >> 2) & 1;
		D = ~(val >> 3) & 1;

		// functions
		F0 = !((A && !B) || (C && B && D));
		F1 = (A || C) && (B && D);

		// F1 = bit1, F0 = bit0
		output = ((F1 << 1) | F0);
		
		// send the result to the external chip and write it in the "0" I/O register
		PCA9555_0_write(REG_OUTPUT_0, output);
		
	}
}
