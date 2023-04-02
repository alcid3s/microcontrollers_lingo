#ifndef LCD_H
#define LCD_H

void initLCD();
void display_text(char *str);
void set_cursor(int position);

void lcd_write_data(unsigned char byte);
void lcd_strobe_lcd_e(void);
void lcd_clear();
void lcd_write_command(unsigned char byte);

#endif LCD_H