#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "lcd.c"
#include "spi_display.h"
#include "spi_display.c"

int sCount = 0;
int cursor_pos = 0;
char current_word[6];
char current_value;
char* active_word;
int points = 0;

unsigned long timer_freq = (F_CPU + (64/2)) / 64;

char* words[] = {"blowen", "duivel", "slecht", "vlaams", "worden", "kasten", "matrix", "koekje", "tattoo", "kippen", "vogels", "piraat", "zilver", "metaal", "tassen", "bedden"};

void wait( int ms )
{
	for (int tms=0; tms<ms; tms++)
	{
		_delay_ms( 1 );
	}
}

void adcInit( void )
{
	ADMUX = 0b11100001;
	ADCSRA = 0b10000110;
}

void clear_first_line(){
	for(int i = 5; i >= 0; i--){
		set_cursor(i);
		display_text(" ");
	}
	set_cursor(0);
	cursor_pos = 0;
}

void init_timer_random()
{	
	OCR1A = 8;
	TCCR1A = 0b00100001;
	TCCR1B = 0b00001101;
	TIMSK |= (1<<4);
	sei();
}

ISR(TIMER1_COMPA_vect){
	sCount++;
	if(sCount == 10000){
		sCount = 0;
	}
}

void get_random_word(){
	int random_word = sCount % 15;
	active_word = words[random_word];
}

void init_timer_pwm()
{
    TCCR3A = (1 << COM3A1) | (1 << WGM31);
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS31); // Set prescaler to 8
	OCR3A = 36;
	ICR3 = 0;
}

void set_pulse(int pulse_width)
{
	ICR3 = pulse_width;
}

int main()
{	
	// ADC
	DDRF = 0x00;
	
	// Knoppen
	DDRA = 0x00;
	
	DDRE = 0xFF;
	

	// initialisations
	initLCD();
	init_timer_random();
	init_timer_pwm();
	spi_master_init();
	displayDriverInit();
	adcInit();
	
	lcd_clear();
	spi_clear();
	
	get_random_word();
	
	while (1)
	{
		ADCSRA |= (1<<6);
		while(ADCSRA & (1<<6));
		current_value = (char)((ADCH / 10.2) + 97);
		lcd_write_data(current_value);
		
		// interrupts didn't work so this is the solution.
		
		// enter
		if(PINA & (1<<0))
		{
			current_word[cursor_pos] = current_value;
			if(cursor_pos >= 5)
			{
				set_cursor(16);
				int win = 1;
				for(int i = 0; i < 6; i++){
					if(active_word[i] == current_word[i]){
						lcd_write_data(current_word[i]);
						set_pulse(400);
						wait(150);
						set_pulse(0);
						wait(50);
					}else{
						lcd_write_data('-');
						set_pulse(1500);
						wait(200);
						set_pulse(0);
						wait(50);
						win = 0;
					}
				}
				
				clear_first_line();
				if(win){
					points++;
					if(points < 10000)
					{
						spi_write_num(points);
					}
					display_text("CORRECT!");
					wait(2000);
					lcd_clear();
					get_random_word();
				}
			}
			else
			{
				cursor_pos++;
			}
			wait(200);
			continue;
		}
		// backspace
		else if(PINA & (1<<1) && cursor_pos > 0)
		{
			current_word[cursor_pos] = ' ';
			set_cursor(cursor_pos);
			display_text(" ");
			cursor_pos--;
			wait(200);
		}
		set_cursor(cursor_pos);
		wait(1);
	}
	return (1);
}