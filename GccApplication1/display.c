/*
 * display.c
 *
 * Created: 1/16/2018 8:16:59 AM
 *  Author: Diederich Kroeske
 */ 

#include <avr/io.h>

#include "display.h"
#include "fonts.h"

// HT16K33 routines
void displayInitHT16K33(uint8_t i2c_address);

// I2C routines
void twi_init(void);
void twi_start(void);
void twi_stop(void);
void twi_tx(unsigned char data);

// I2C address of display
#define D0_I2C_ADDR	((0x70 + 0) << 1)

// Display buffer in ATMEGA memory
#define	width	8 * 1		// 1 displays width
#define	height	8			// 1 display height
uint8_t buf[width*height/8];

/******************************************************************/
void displayInit(void) 
/*
short:			Init display
inputs:			
outputs:		-
notes:			Init display
Version :    	DMK, Initial code
*******************************************************************/
{
	twi_init();							// Enable TWI interface
	displayInitHT16K33(D0_I2C_ADDR);	// Iit display
}

/******************************************************************/
void displayInitHT16K33(uint8_t i2c_address) 
/*
short:
inputs:
outputs:
notes:
Version :    	DMK, Initial code
*******************************************************************/
{
	// System setup page 30 ht16k33 datasheet
	twi_start();
	twi_tx(i2c_address);	// Display I2C addres + R/W bit
	twi_tx(0x21);	// Internal osc on (page 10 HT16K33)
	twi_stop();
		
	// ROW/INT set. Page 31 ht16k33 datasheet
	twi_start();
	twi_tx(i2c_address);	// Display I2C address + R/W bit
	twi_tx(0xA0);	// HT16K33 pins all output (default)
	twi_stop();

	// Dimming set
	twi_start();
	twi_tx(i2c_address);	// Display I2C address + R/W bit
	twi_tx(0xE1);	// Display Dimming 2/16 duty cycle
	twi_stop();

	// Display set
	twi_start();
	twi_tx(i2c_address);	// Display I2C address + R/W bit
	twi_tx(0x81);			// Display ON, Blinking OFF
	twi_stop();
	
	// Beeld een patroon af op display (test)
	twi_start();
	twi_tx(i2c_address);
	twi_tx(0x00);
	uint8_t a = 0x55;
	for( uint8_t idx = 0; idx < 8; idx++ ) {
		a ^= 0xFF;
		uint8_t data = (a >> 1) | ((a<<7) & 0x80);
		twi_tx( data);
		twi_tx( 0x00);
	}
	twi_stop();
}

/******************************************************************/
void displaySetPixel(uint8_t x, uint8_t y)
/*
short:			
inputs:			
outputs:		
notes:			
Version :    	DMK, Initial code
*******************************************************************/
{
}

/******************************************************************/
void displayClrPixel(uint8_t x, uint8_t y)
/*
short:
inputs:
outputs:
notes:
Version :    	DMK, Initial code
*******************************************************************/
{
}

/******************************************************************/
void display()
/*
short:		Write buffer to display
inputs:
outputs:
notes:		Let op de 'vreemde' shift, foutje in printplaat?
Version:	DMK, Initial code
*******************************************************************/
{
	// Second display
	twi_start();
	twi_tx(D0_I2C_ADDR);
	twi_tx(0x00);
	for( uint8_t idx = 0; idx < 8; idx++ ) {
		uint8_t a = buf[7 + 0 * 8 - idx];
		uint8_t data = (a >> 1) | ((a<<7) & 0x80);
		twi_tx( data);
		twi_tx( 0x00);
	}
	twi_stop();
}

/******************************************************************/
void displayRotl(void)
/*
short:		Rotate buffer to the left
inputs:
outputs:
notes:
Version:	DMK, Initial code
*******************************************************************/
{
}

/******************************************************************/
void displayRotr(void)
/*
short:		Rotate buffer to the right
inputs:
outputs:
notes:
Version:	DMK, Initial code
*******************************************************************/
{
}

/******************************************************************/
void displayClr(void)
/*
short:		Clear display
inputs:
outputs:
notes:
Version:	DMK, Initial code
*******************************************************************/
{
	for( uint8_t idx = 0; idx < width - 1; idx++) {
		buf[idx] = 0;
	}
}


/******************************************************************/
void displayChar(char ch, uint8_t x, uint8_t y)
/*
short:		Print character op display
inputs:
outputs:
notes:
Version:	DMK, Initial code
*******************************************************************/
{	
}

/******************************************************************/
void displayString(char *str, uint8_t x, uint8_t y)
/*
short:		Print string op display
inputs:
outputs:
notes:		Maakt gebruik van displayChar(..)
Version:	DMK, Initial code
*******************************************************************/
{
}


/******************************************************************/
void twi_init(void)
/*
short:			Init AVR TWI interface and set bitrate
inputs:
outputs:
notes:			TWI clock is set to 100 kHz
Version :    	DMK, Initial code
*******************************************************************/
{
	TWSR = 0;
	TWBR = 32;	 // TWI clock set to 100kHz, prescaler = 0
}

/******************************************************************/
void twi_start(void)
/*
short:			Generate TWI start condition
inputs:
outputs:
notes:
Version :    	DMK, Initial code
*******************************************************************/
{
	TWCR = (0x80 | 0x20 | 0x04);
	while( 0x00 == (TWCR & 0x80) );
}

/******************************************************************/
void twi_stop(void)
/*
short:			Generate TWI stop condition
inputs:
outputs:
notes:
Version :    	DMK, Initial code
*******************************************************************/
{
	TWCR = (0x80 | 0x10 | 0x04);
}

/******************************************************************/
void twi_tx(unsigned char data)
/*
short:			transmit 8 bits data
inputs:
outputs:
notes:
Version :    	DMK, Initial code
*******************************************************************/
{
	TWDR = data;
	TWCR = (0x80 | 0x04);
	while( 0 == (TWCR & 0x80) );
}
