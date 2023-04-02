#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include "spi_display.h"

#define BIT(x)		( 1<<x )
#define DDR_SPI DDRB // spi Data direction register
#define PORT_SPI PORTB // spi Output register

void spi_master_init(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
	// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
	// Mode = 0: CPOL=0, CPPH=0;
}

// Write a byte from master to slave
void spi_write( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
char spi_writeRead( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete
	data = SPDR;							// New received data (eventually, MISO) in SPDR
	return data;							// Return received byte
}

// Select device on pinnumer PORTB
void spi_slaveSelect(unsigned char chipNumber)
{
	PORTB &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slaveDeSelect(unsigned char chipNumber)
{
	PORTB |= BIT(chipNumber);
}


// Initialize the driver chip (type MAX 7219)
void displayDriverInit()
{
	spi_slaveSelect(0);				// Select display chip (MAX7219)
	spi_write(0x09);      			// Register 09: Decode Mode
	spi_write(0xFF);				// 	-> 1's = BCD mode for all digits
	spi_slaveDeSelect(0);			// Deselect display chip

	spi_slaveSelect(0);				// Select dispaly chip
	spi_write(0x0A);      			// Register 0A: Intensity
	spi_write(0x04);    			//  -> Level 4 (in range [1..F])
	spi_slaveDeSelect(0);			// Deselect display chip

	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0B);  				// Register 0B: Scan-limit
	spi_write(0x03);   				// 	-> 1 = Display digits 0..1
	spi_slaveDeSelect(0);			// Deselect display chip

	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0C); 				// Register 0B: Shutdown register
	spi_write(0x01); 				// 	-> 1 = Normal operation
	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display on ('normal operation')
void displayOn()
{
	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0C); 				// Register 0B: Shutdown register
	spi_write(0x01); 				// 	-> 1 = Normal operation
	spi_slaveDeSelect(0);			// Deselect display chip
}

// Set display off ('shut down')
void displayOff()
{
	spi_slaveSelect(0);				// Select display chip
	spi_write(0x0C); 				// Register 0B: Shutdown register
	spi_write(0x00); 				// 	-> 1 = Normal operation
	spi_slaveDeSelect(0);			// Deselect display chip
}

void spi_clear()
{
	for (char i =1; i<=4; i++)
	{
		spi_slaveSelect(0);
		spi_write(i);
		spi_write(0);
		spi_slaveDeSelect(0);
	}
}

void spi_write_num(int num)
{
	int pos = 1;
	while(num > 0)
	{
		spi_slaveSelect(0);
		spi_write(pos);
		spi_write(num % 10);
		spi_slaveDeSelect(0);
		
		pos++;
		num /= 10;
	}
}