#/******************************************************
#               MARMARA UNIVERSITY
#      ELECTRICAL AND ELECTRONICS ENGINEERING 
#           ENES SENGUL & ENSAR YILMAZ
#         SENSORLESS SOLAR TRACING SYSTEM 
#                   20/01/2023
#*******************************************************/

#include <tez.h>
#include <flex_lcd_tur.c>
#use fast_io(b)

#ZERO_RAM

#define fine 10

#define motorxr pin_C0
#define motorxl pin_C1
#define motoryr pin_C2
#define motoryl pin_C3
#define motorxe pin_C5
#define motorye pin_C4

float solar1v, solar2v, solar3v, solar4v;
char sw;
unsigned int display = 0;
unsigned int adcsec, l, i, motorxf;
unsigned long solar1t;
unsigned long solar2t;
unsigned long solar3t;
unsigned long solar4t;
unsigned long solartx;
unsigned long solarty;

typedef struct {
   long solar1[5];
   long solar2[5];
   long solar3[5];
   long solar4[5];
} voltaj;

voltaj adcoku;

void olcum() {  
   switch (adcsec) {
      case 0 : 
         for(l=4;l>0;--l) {
            adcoku.solar1[l]=adcoku.solar1[l-1];
         }
         adcoku.solar1[0]=read_adc();
         solar1t=(adcoku.solar1[0]+adcoku.solar1[1]+adcoku.solar1[2]+adcoku.solar1[3]+adcoku.solar1[4])/5;
         solar1v=solar1t/200.8;
         break;
      case 1 :
         for(l=4;l>0;--l) {
            adcoku.solar2[l]=adcoku.solar2[l-1];
         }
         adcoku.solar2[0]=read_adc();
         solar2t=(adcoku.solar2[0]+adcoku.solar2[1]+adcoku.solar2[2]+adcoku.solar2[3]+adcoku.solar2[4])/5;
         solar2v=solar2t/200.8;
         break;
      case 2 :
         for(l=4;l>0;--l) {
            adcoku.solar3[l]=adcoku.solar3[l-1];
         }
         adcoku.solar3[0]=read_adc();
         solar3t=(adcoku.solar3[0]+adcoku.solar3[1]+adcoku.solar3[2]+adcoku.solar3[3]+adcoku.solar3[4])/5;
         solar3v=solar3t/200.8;
         break;
      case 3 :
         for(l=4;l>0;--l) {
            adcoku.solar4[l]=adcoku.solar4[l-1];
         }
         adcoku.solar4[0]=read_adc();
         solar4t=(adcoku.solar4[0]+adcoku.solar4[1]+adcoku.solar4[2]+adcoku.solar4[3]+adcoku.solar4[4])/5;
         solar4v=solar4t/200.8;
         break;
   }         
   ++adcsec;
   if (adcsec>3) adcsec=0;
   set_adc_channel(adcsec);    
   
   solartx=solar1t-solar2t;
   solarty=solar3t-solar4t;
}

void yaz() {
   
   switch (display) {
      case 0 : 
         printf(lcd_putc, "\nSolar V: %4.2fV", (solar1v + solar2v + solar3v + solar4v) / 4);
         break;
      case 1 : 
         lcd_gotoxy(1,1);
         printf(lcd_putc, "A=%4.2fV", solar1v);
         lcd_gotoxy(9,1);
         printf(lcd_putc, "B=%4.2fV", solar2v);
         lcd_gotoxy(1,2);
         printf(lcd_putc, "C=%4.2fV", solar3v);
         lcd_gotoxy(9,2);
         printf(lcd_putc, "D=%4.2fV", solar4v);
         break;
      case 2 : 
         lcd_gotoxy(1,1);
         printf(lcd_putc, "X1=%lu", solar1t);
         lcd_gotoxy(1,2);
         printf(lcd_putc, "X2=%lu", solar2t);
         lcd_gotoxy(6,1);
         printf(lcd_putc, "DX=%lu", solar1t - solar2t);
         lcd_gotoxy(6,2);
         printf(lcd_putc, "DX=%lu", solar2t - solar1t);
         lcd_gotoxy(16,1);
         printf(lcd_putc, "M=%u", motorxf);
         break;
      case 3 : 
         lcd_gotoxy(1,1);
         printf(lcd_putc, "Y1=%lu", solar1t);
         lcd_gotoxy(1,2);
         printf(lcd_putc, "Y2=%lu", solar3t);
         lcd_gotoxy(6,1);
         printf(lcd_putc, "DY=%lu", solar1t - solar3t);
         lcd_gotoxy(6,2);
         printf(lcd_putc, "DY=%lu", solar3t - solar1t);
         lcd_gotoxy(16,1);
         printf(lcd_putc, "M=%u", motorxf);
         break;
   }
}
void mxr() {

   OUTPUT_C(0x21);delay_ms(5);OUTPUT_C(0x23);delay_ms(5);
      
}

void mxl() {

   OUTPUT_C(0x22);delay_ms(5);OUTPUT_C(0x23);delay_ms(5);
      
}

void myu() {

   OUTPUT_C(0x14);delay_ms(10);OUTPUT_C(0x1C);delay_ms(30);          
      
}

void myd() {

   OUTPUT_C(0x18);delay_ms(10);OUTPUT_C(0x1C);delay_ms(30);
      
}
#INT_RTCC
void RTCC_isr(void) {
measureVoltage();
}

#INT_RB
void RB_isr(void) {
sw = input_B();
if (testbit(sw, 4)) motorxf = 1;
if (testbit(sw, 5)) motorxf = 2;
if (testbit(sw, 6)) motorxf = 3;
if (testbit(sw, 7)) motorxf = 4;
}

void main ( )
{
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32|RTCC_8_bit);                          //4,0 ms overflow
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_tris_B(0xF0);
   output_b(0x00);
   output_c(0x00);
   enable_interrupts(INT_RTCC);
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);

   lcd_init();
   
   printf(lcd_putc,"\f*SOLAR  TRACKER*");
   delay_ms(1000);
   printf(lcd_putc,"\n ENES – ENSAR");
   delay_ms(1000);
   
   printf(lcd_putc,"\fX Axis Motor"); 
   printf(lcd_putc,"\ntesting");
   while (!input_state(pin_B4)) {OUTPUT_C(0x21);delay_ms(10);OUTPUT_C(0x23);delay_ms(25);}
   printf(lcd_putc,"\.");
   while (!input_state(pin_B5)) {OUTPUT_C(0x22);delay_ms(10);OUTPUT_C(0x23);delay_ms(15);}
   printf(lcd_putc,"\.");
   for (i=0;i<10;i++) {OUTPUT_C(0x21);delay_ms(10);OUTPUT_C(0x23);delay_ms(15);} 
   printf(lcd_putc,"\.");
   delay_ms(500);
   
   printf(lcd_putc,"\fY Axis Motor"); 
   printf(lcd_putc,"\ntesting");
   while (!input_state(pin_B6)) {OUTPUT_C(0x14);delay_ms(10);OUTPUT_C(0x1C);delay_ms(30);}
   printf(lcd_putc,"\.");
   while (!input_state(pin_B7)) {OUTPUT_C(0x18);delay_ms(10);OUTPUT_C(0x1C);delay_ms(30);}
   printf(lcd_putc,"\.");
   for (i=0;i<3;i++) {OUTPUT_C(0x14);delay_ms(10);OUTPUT_C(0x1C);delay_ms(30);} 
   printf(lcd_putc,"\.");
   delay_ms(500);
   
   printf(lcd_putc,"\f* SYSTEM  GOOD *");
   delay_ms(500);
   printf(lcd_putc,"\f");
   
   while(true) 
   {     
      
      yaz();
      
      if (((solartx>fine)&&(solartx<1024))&&(!input_state(pin_B4))) { 
         mxr();
         if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\  Sun tracking  ");
         if (display==2) lcd_gotoxy(16,2);printf(lcd_putc,"\L");                                 
      }
         else {
            if (((solartx<(65535-fine))&&(solartx>=64512))&&(!input_state(pin_B5))) {       
               mxl();
               if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\  Sun tracking  ");
               if (display==2) lcd_gotoxy(16,2);printf(lcd_putc,"\R");                           
            }
               else {
                  if (((solartx>=0)&&(solartx<=fine))||((solartx>=(65535-fine))&&(solartx<=65535))) {
                     motorxf=0;
                     if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\     Locked     ");
                     if (display==2) lcd_gotoxy(16,2);printf(lcd_putc,"\O");                     
                  }     
               }
         }
  
  
      if (((solarty>fine)&&(solarty<1024))&&(!input_state(pin_B7))) { 
         myd();
         if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\  Sun tracking  ");
         if (display==3) lcd_gotoxy(16,2);printf(lcd_putc,"\U");                                 
      }
         else {
            if (((solarty<(65535-fine))&&(solarty>=64512))&&(!input_state(pin_B6))) {       
               myu();
               if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\  Sun tracking  ");
               if (display==3) lcd_gotoxy(16,2);printf(lcd_putc,"\D");                           
            }
               else {
                  if (((solarty>=0)&&(solarty<=fine))||((solarty>=(65535-fine))&&(solarty<=65535))) {
                     motorxf=0;
                     if (!display) lcd_gotoxy(1,1);printf(lcd_putc,"\     Locked     ");
                     if (display==3)  lcd_gotoxy(16,2);printf(lcd_putc,"\O");                     
                  }     
               }
         }
   }
}
