#include "lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define LCD_E     3
#define LCD_RS    2

void initLCD()
{
        // PORTC output mode and all low (also E and RS pin)
        DDRC = 0xFF;
        PORTC = 0x00;

        // Step 2 (table 12)
        PORTC = 0x20;    // function set
        lcd_strobe_lcd_e();

        // Step 3 (table 12)
        PORTC = 0x20;   // function set
        lcd_strobe_lcd_e();
        PORTC = 0x80;
        lcd_strobe_lcd_e();

        // Step 4 (table 12)
        PORTC = 0x00;   // Display on/off control
        lcd_strobe_lcd_e();
        PORTC = 0xF0;
        lcd_strobe_lcd_e();

        // Step 4 (table 12)
        PORTC = 0x00;   // Entry mode set
        lcd_strobe_lcd_e();
        PORTC = 0x60;
        lcd_strobe_lcd_e();
}
void display_text(char *str)
{
    for(;*str; str++){
        lcd_write_data(*str);
    }
}
void set_cursor(int position){
    lcd_write_command(0x80 | (position & 0x0F) | ((position << 2) & 0x40));
}

/* 
    Voorbeeld code van github: https://github.com/AvansTi/TI2.3-Microcontrollers-examples/blob/main/Les-2/lcd_bigavr6/bigavr_lcd.c
*/
void lcd_write_data(unsigned char byte) {
    // First nibble.
    PORTC = byte;
    PORTC |= (1<<LCD_RS);
    lcd_strobe_lcd_e();

    // Second nibble
    PORTC = (byte<<4);
    PORTC |= (1<<LCD_RS);
    lcd_strobe_lcd_e();
}

void lcd_strobe_lcd_e(void) {
    PORTC |= (1<<LCD_E);
    _delay_ms(1);
    PORTC &= ~(1<<LCD_E);
    _delay_ms(1);
}

void lcd_write_command(unsigned char byte) {
    // First nibble.
    PORTC = byte;
    PORTA &= ~(1<<LCD_RS);
    lcd_strobe_lcd_e();

    // Second nibble
    PORTC = (byte<<4);
    PORTA &= ~(1<<LCD_RS);
    lcd_strobe_lcd_e();
}

void lcd_clear() {
    lcd_write_command (0x01);                        //Leeg display
    _delay_ms(2);
    lcd_write_command (0x80);                        //Cursor terug naar start
}