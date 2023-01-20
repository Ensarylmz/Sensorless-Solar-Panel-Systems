/******************************************************
 MARMARA UNIVERSITY
 ELECTRICAL AND ELECTRONICS ENGINEERING 
 ENES SENGUL & ENSAR YILMAZ
 SENSORLESS SOLAR TRACING SYSTEM 
 20/01/2023
*******************************************************/
#include <tez.h>
#include <flex_lcd_tur.c>
#use fast_io(b)

#ZERO_RAM

#define LCD_E     PIN_D3 
#define LCD_RS    PIN_D2
#define LCD_DB5   PIN_D5 
#define LCD_DB4   PIN_D4 
#define LCD_DB7   PIN_D7 
#define LCD_DB6   PIN_D6     
#define lcd_line_two 0x40 // LCD RAM address for the 2nd line 
#define lcd_type 2        // 0=5x7, 1=5x10, 2=2 lines 

void lcd_send_nibble(int8 nibble)
{
digitalWrite(LCD_DB4, !!(nibble & 1));
digitalWrite(LCD_DB5, !!(nibble & 2));
digitalWrite(LCD_DB6, !!(nibble & 4));
digitalWrite(LCD_DB7, !!(nibble & 8));

delayMicroseconds(1);
digitalWrite(LCD_E, HIGH);
delayMicroseconds(2);
digitalWrite(LCD_E, LOW);
}



#ifdef USE_LCD_RW
int8 lcd_read_nibble(void)
{
int8 retval = 0;
output_high(LCD_E);
delay_cycles(1);
if(input(LCD_DB4)) retval |= 1;
if(input(LCD_DB5)) retval |= 2;
if(input(LCD_DB6)) retval |= 4;
if(input(LCD_DB7)) retval |= 8;

output_low(LCD_E); 
return retval;    
}
#endif
//--------------------------------------- 

#ifdef USE_LCD_RW
int8 lcd_read_byte(void)
{
int8 retval = 0;
output_high(LCD_RW);
delay_cycles(1);
retval = lcd_read_nibble(); 
retval <<= 4;
retval |= lcd_read_nibble();
return retval;
}
#endif
//---------------------------------------- 


void lcd_send_byte(int8 address, int8 n)
{
output_low(LCD_RS);
#ifdef USE_LCD_RW 
while(bit_test(lcd_read_byte(),7)) ; 
#else 
delay_us(60);  
#endif 
if(address) 
   output_high(LCD_RS); 
else 
   output_low(LCD_RS); 
delay_cycles(1); 
#ifdef USE_LCD_RW 
output_low(LCD_RW); 
delay_cycles(1); 
#endif 
output_low(LCD_E); 

lcd_send_nibble(n >> 4); 
lcd_send_nibble(n & 0xf); 
}

//----------------------------

void turkce()
{
	int i = 64;
	int s = 64;
	int adres = 64;
	const byte karakter_[8][8]=
	{
	{ 0, 0,15,16,14, 5, 30, 0},//{ 0,15,16,14, 1,30, 4, 0},//
	{10, 0,14,17,17,17,14, 0},//
	{14,17,16,16,16,21,14, 0},//{14,17,16,16,17,14, 4, 0},//
	{ 0, 0,12, 4, 4, 4,14, 0},//i
	{ 0, 0,14,16,16,21,14, 0},//{ 0,14,16,16,17,14, 4, 0},//
	{14, 0,15,17,15, 1,14, 0},//
	{10, 0,17,17,17,17,14, 0},//
	{15,16,16,14, 1, 5,30, 0},//{15,16,16,14, 1,30, 4, 0},//
	};

	for(i=0;i<=7;i++)
	{
		lcd_send_byte(0,adres);
		for(s=0;s<=7;s++) {
			lcd_send_byte(1,karakter_[i][s]);
		}
		adres=adres+8;
	}
}

//----------------------------

void lcd_init(void) 
{ 
	int8 i; 

	output_low(LCD_RS); 

	#ifdef USE_LCD_RW 
	output_low(LCD_RW); 
	#endif 

	output_low(LCD_E); 

	delay_ms(15); 

	for(i=0 ;i < 3; i++) 
	{ 
		lcd_send_nibble(0x03); 
		delay_ms(5); 
	} 

	lcd_send_nibble(0x02); 

	for(i=0; i < sizeof(LCD_INIT_STRING); i++) 
	{ 
		lcd_send_byte(0, LCD_INIT_STRING[i]); 
     
		#ifndef USE_LCD_RW 
		delay_ms(5); 
		#endif 
	} 

	turkce();  

} 

//---------------------------- 

void lcd_pozisyon_ayarla(int8 x, int8 y)
{
int8 adres;
if(y != 1) 
   adres = lcd_line_two; 
else 
   adres=0; 

adres += x-1; 
lcd_send_byte(0, 0x80 | adres); 




//----------------------------- 
void lcd_putc(char c) 
{ 
	switch(c) 
	{ 
		case '\f': 
		lcd_send_byte(0,1); 
		delay_ms(2); 
		break; 
    
		case '\n': 
		lcd_gotoxy(1,2); 
		break; 
		
		case '': 
		lcd_send_byte(1,0x00); 
		break;

		case '': 
		lcd_send_byte(1,0x01); 
		break;

		case '': 
		lcd_send_byte(1,0x02); 
		break;
    
		case '\b': 
		lcd_send_byte(0,0x10); 
		break;
		
		case '': 
		lcd_send_byte(1,0x05); 
		break;
		
		case '': 
		lcd_send_byte(1,0x03); 
		break;

		case '': 
		lcd_send_byte(1,0x04); 
		break;
		
		case '': 
		lcd_send_byte(1,0x07); 
		break; 
	
		case '': 
		lcd_send_byte(1,0x06); 
		break;
    
		default: 
		lcd_send_byte(1,c); 
		break; 
	} 
} 

#ifdef USE_LCD_RW
char get_lcd_char(int8 x, int8 y) {
set_lcd_cursor(x, y);
// Wait until the busy flag is low.
while(is_lcd_busy());
char c;
set_lcd_read_mode();
c = read_lcd_data();
set_lcd_write_mode();
return c;
}
#endif


