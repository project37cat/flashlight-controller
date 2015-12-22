// Flashlight v.0.2 (prototype)
//
// main.h (file 2 of 2)
//
// 10-Dec-2015  Toxic Cat :3  Copyleft
//
// Compiler: HI-TECH C PRO for the PIC18 MCU Family  V9.63PL3



#define _XTAL_FREQ 8000000


#include <htc.h>
#include <stdio.h>


/********************************** Config Bits for PIC18F4320 **********************************/

__CONFIG(1, RCIO );                        //internal RC oscillator
__CONFIG(2, BORDIS & WDTDIS & WDTPS512 );  //disable BOR  //disable WDT  //WDT prescaler
__CONFIG(3, CCP2RC1 );                     //CCP2 multiplexed to RC1
__CONFIG(4, LVPEN );                       //low-voltage ICSP enabled

/************************************************************************************************/


// stdint
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;


#define OFF    0
#define ON     1

#define LOW    0
#define HIGH   1

#define OUTPUT 0
#define INPUT  1

	
// dual color LED 1
#define LED1_DIR   TRISA2=0; TRISA1=0
#define LED1_RED   RA2=1; RA1=0
#define LED1_GRN   RA1=1; RA2=0
#define LED1_OFF   RA2=0; RA1=0

// dual color LED 2
#define LED2_DIR   TRISB4=0; TRISA3=0
#define LED2_RED   RB4=1; RA3=0
#define LED2_GRN   RA3=1; RB4=0
#define LED2_OFF   RB4=0; RA3=0

// dual color LED 3
#define LED3_DIR   TRISE0=0; TRISA5=0
#define LED3_RED   RE0=1; RA5=0
#define LED3_GRN   RA5=1; RE0=0
#define LED3_OFF   RE0=0; RA5=0

// dual color LED 4
#define LED4_DIR   TRISE2=0; TRISE1=0
#define LED4_RED   RE2=1; RE1=0
#define LED4_GRN   RE1=1; RE2=0
#define LED4_OFF   RE2=0; RE1=0


//external ref. volt. +2.5V (TL431 and 470-510 ohms res.)  //+Vref=Vdd(3.7V), -Vref=Vss, AIN0=2.5V
#define TL431_DIR  TRISD0
#define TL431_PIN  RD0

//power LED
#define LIGHT_DIR  TRISC1
#define LIGHT_PIN  RC1

//switch ON/OFF the power LED
#define LIGHT_OFF   LIGHT_DIR = INPUT
#define LIGHT_ON    LIGHT_DIR = OUTPUT

//button PINs
#define BUT1_DIR     TRISB0
#define BUT1_PIN     RB0

#define BUT2_DIR     TRISB1
#define BUT2_PIN     RB1

#define BUT_COM_PIN  RD7
#define BUT_COM_DIR  TRISD7

// button hold time x10ms
#define BUT_HLD  80

// button debounce x10ms
#define BUT_DEB  4

// 65536-64286=1250 125k/1250=100Hz
#define TMR0_LOAD 64286

//brightness
#define PWM_MIN 10
#define PWM_MAX 130

//init pins for 4x Dual Color LED
#define LEDS_INIT    LED1_DIR; LED2_DIR; LED3_DIR; LED4_DIR

//all LEDs switch off
#define LEDS_OFF     LED1_OFF; LED2_OFF; LED3_OFF; LED4_OFF

//а short flash of LED #1
#define LED1_FLASH   LED1_RED; delay_ms(5); LED1_OFF

//а short flash of LED #4
#define LED4_FLASH   LED4_RED; delay_ms(100); LED4_OFF


uint8_t but1stat; //button #1 state  //2 - long press, 1 - short press, 0 - not pressed
uint8_t but1hold; //button #1 hold flag
uint8_t but1cnt;  //counter for button #1

uint8_t but2stat; //button #2 state
uint8_t but2hold; //button #2 hold flag
uint8_t but2cnt;  //counter for button #2

uint8_t refrcnt;
uint8_t flashcnt;

uint16_t battvolt; //battery voltage (raw data: ~510..1020 for Vdd 5.0..2.5V)
uint8_t battlow; //the battery low flag
