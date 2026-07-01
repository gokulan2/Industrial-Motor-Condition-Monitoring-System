#include <xc.h>
#include <stdio.h>

#pragma config FOSC = HS, WDTE = OFF, PWRTE = ON, BOREN = ON
#pragma config LVP = OFF, CPD = OFF, WRT = OFF, CP = OFF

#define _XTAL_FREQ 20000000

#define RS RE0
#define RW RE1
#define EN RE2

#define BTN_SAVE RB1
#define BTN_NEXT RB2
#define BTN_PREV RB4
#define BUZZER   RB0     
#define LOG_COUNT_ADDR 250
#define MAX_LOGS 125
void lcd_cmd(unsigned char cmd) {
    PORTD=cmd; RS=0; RW=0; EN=1; __delay_ms(2); EN=0; __delay_ms(2);
}
void lcd_data(unsigned char data) {
    PORTD=data; RS=1; RW=0; EN=1; __delay_ms(2); EN=0; __delay_ms(2);
}
void lcd_string(const char *str) { while(*str) lcd_data(*str++); }
void lcd_init() {
    __delay_ms(20);
    lcd_cmd(0x38); lcd_cmd(0x0C); lcd_cmd(0x06); lcd_cmd(0x01);
    __delay_ms(10);
}
void EEPROM_Write(unsigned char addr, unsigned char data) {
    EEADR=addr; EEDATA=data;
    EECON1bits.EEPGD=0; EECON1bits.WREN=1;
    INTCONbits.GIE=0;
    EECON2=0x55; EECON2=0xAA;
    EECON1bits.WR=1;
    __delay_ms(5);     
    EECON1bits.WREN=0; INTCONbits.GIE=1;
}
unsigned char EEPROM_Read(unsigned char addr) {
    EEADR=addr; EECON1bits.EEPGD=0; EECON1bits.RD=1; return EEDATA;
}
void EEPROM_Write_Int(unsigned char addr, unsigned int val) {
    EEPROM_Write(addr,   (val>>8)&0xFF);
    EEPROM_Write(addr+1,  val&0xFF);
}
unsigned int EEPROM_Read_Int(unsigned char addr) {
    return ((unsigned int)EEPROM_Read(addr)<<8) | EEPROM_Read(addr+1);
}
unsigned int ADC_Read() {
    unsigned char t=100;
    __delay_us(20);
    ADCON0bits.GO_nDONE=1;
    while(ADCON0bits.GO_nDONE && t--);
    return (((unsigned int)ADRESH<<8)|ADRESL);
}

void main() {
    unsigned int adc;
    float volt=0;
    char buf[16];

    unsigned char log_count = EEPROM_Read(LOG_COUNT_ADDR);
    if(log_count > MAX_LOGS) log_count = 0;

    unsigned char current_log=0, view_mode=0;

    ADCON1=0x8E; ADCON0=0x41;
    TRISA=0x01; TRISB=0x16; TRISC=0x00; TRISD=0x00; TRISE=0x00;
    PORTB=0; PORTC=0; PORTD=0; PORTE=0;

    lcd_init();

    lcd_cmd(0x80); lcd_string("Battery Monitor ");
    lcd_cmd(0xC0); lcd_string("System Ready... ");
    __delay_ms(1500);
    lcd_cmd(0x01); __delay_ms(2);

    while(1) {

        if(view_mode==0) {
            adc  = ADC_Read();
            volt = ((adc*5.0)/1023.0)*5.7;
            BUZZER = (volt < 11.0) ? 1 : 0;

            lcd_cmd(0x80);
            if(volt>=12.0)      sprintf(buf,"V=%.2fV GOOD  ",volt);
            else if(volt>=11.0) sprintf(buf,"V=%.2fV NORM  ",volt);
            else                sprintf(buf,"V=%.2fV LOW!  ",volt);
            lcd_string(buf);

            lcd_cmd(0xC0);
            sprintf(buf,"Logs:%d/125    ",log_count);
            lcd_string(buf);
        }
        if(BTN_SAVE==1) {
            __delay_ms(20);
            if(BTN_SAVE==1) {
                if(view_mode==1) {
                    // exit log view
                    view_mode=0;
                } else if(log_count<MAX_LOGS) {
                    unsigned int v_int=(unsigned int)(volt*100);
                    EEPROM_Write_Int(log_count*2, v_int);
                    log_count++;
                    EEPROM_Write(LOG_COUNT_ADDR, log_count);
                    lcd_cmd(0xC0);
                    sprintf(buf,"  SAVED! (%d)   ",log_count);
                    lcd_string(buf);
                    __delay_ms(800);
                } else {
                    lcd_cmd(0xC0); lcd_string(" MEMORY FULL!  ");
                    __delay_ms(800);
                }
                while(BTN_SAVE==1); __delay_ms(20);
            }
        }
        if(BTN_NEXT==1) {
            __delay_ms(20);
            if(BTN_NEXT==1) {
                view_mode=1;
                if(log_count>0) {
                    current_log++;
                    if(current_log>=log_count) current_log=0;
                    unsigned int v_int=EEPROM_Read_Int(current_log*2);
                    float v_log=v_int/100.0;
                    lcd_cmd(0x80);
                    sprintf(buf,"Log %d of %d    ",current_log+1,log_count);
                    lcd_string(buf);
                    lcd_cmd(0xC0);
                    sprintf(buf,"V=%.2fV         ",v_log);
                    lcd_string(buf);
                } else {
                    lcd_cmd(0x80); lcd_string("No logs saved!  ");
                    lcd_cmd(0xC0); lcd_string("                ");
                }
                while(BTN_NEXT==1); __delay_ms(20);
            }
        }
        if(BTN_PREV==1) {
            __delay_ms(20);
            if(BTN_PREV==1) {
                view_mode=1;
                if(log_count>0) {
                    if(current_log==0) current_log=log_count-1;
                    else current_log--;
                    unsigned int v_int=EEPROM_Read_Int(current_log*2);
                    float v_log=v_int/100.0;
                    lcd_cmd(0x80);
                    sprintf(buf,"Log %d of %d    ",current_log+1,log_count);
                    lcd_string(buf);
                    lcd_cmd(0xC0);
                    sprintf(buf,"V=%.2fV         ",v_log);
                    lcd_string(buf);
                }
                while(BTN_PREV==1); __delay_ms(20);
            }
        }

        __delay_ms(100);
    }
}