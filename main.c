// Flashlight
//
// main.c (file 1 of 2)
//
// 22-Dec-2015


#include "main.h"


//-------------------------------------------------------------------------------------------------
void interrupt handler(void)
	{
	//------------------------------------------------------------------------------
	if(INT0IF) // external interrupt 0
		{
		INT0IF=0; //clear int. flag
		INT0IE=0; //disable ext. int. 0
		}
			
	//------------------------------------------------------------------------------
	if(TMR0IF) //timer 0 overflow interrupt //~100Hz (once per ten milliseconds)
		{
		TMR0IF=0;
		TMR0=TMR0_LOAD;
		
		//-----------------------------------------------------------
		if(refrcnt) refrcnt--; //the refresh timer
		
		//-----------------------------------------------------------
		if(flashcnt) flashcnt--; //timer of flash
			
		//-----------------------------------------------------------
		if(BUT1_PIN==0) //if the button 1 is pressed
			{
			if(++but1cnt>=BUT_HLD)
				{
				but1cnt=0;
				but1stat=2; //long press
				but1hold=1;
				}
			}
		else
			{
			if(but1cnt>=BUT_DEB && but1hold==0) but1stat=1; //short press
			but1hold=0;
			but1cnt=0;
			}
		
		//-----------------------------------------------------------
		if(BUT2_PIN==0) //if the button 2 is pressed
			{
			if(++but2cnt>=BUT_HLD)
				{
				but2cnt=0;
				but2stat=2; //long press
				but2hold=1;
				}
			}
		else
			{
			if(but2cnt>=BUT_DEB && but2hold==0) but2stat=1; //short press
			but2hold=0;
			but2cnt=0;
			}
		}	
	}

	
//-------------------------------------------------------------------------------------------------
void delay_ms(uint16_t val)  //delay milliseconds  //correct for 8MHz speed
{
while(val--) __delay_ms(1);
}


//-------------------------------------------------------------------------------------------------
void delay_us(uint16_t val)
{
while(val--) __delay_us(1);
}
	
	
//-------------------------------------------------------------------------------------------------
void upd_volt(void)  //update the battery voltage
	{
	TL431_PIN=HIGH; //TL431 POWER ON

	delay_us(2); //min. 1us pause for slow TL431

	GODONE=1; //start ADC
	while(GODONE); //wait the end of ADC
	
	TL431_PIN=LOW; //TL431 POWER OFF
	
	battvolt=ADRES;  //ADC result
	}


//-------------------------------------------------------------------------------------------------
void check_batt(void)
	{
	uint8_t cnt = 0; //counter
	
	for(uint8_t i=0; i<10; i++)  //detect the low voltage  //10x50ms=500ms max.
		{
		upd_volt(); //update voltage
		
		if((battvolt==0)||(battvolt>850))   // ((TL431 < 2.5V) OR (battery < 3.0V))  //850 ~3.0V  
			{
			delay_ms(50);
			if(++cnt==10) battlow=1; //battery discharged
			}
		else break;  //battery OK //out of loop
		}
	}
	

//-------------------------------------------------------------------------------------------------	
void displ_batt(void)
	{	
	if(battlow==1)        { LED1_OFF; LED2_OFF; LED3_OFF; LED4_OFF; }  //<3.0V (battery discharged)
	else if(battvolt>800) { LED1_RED; LED2_OFF; LED3_OFF; LED4_OFF; }  //800 ~ 3.2V
	else if(battvolt>730) { LED1_GRN; LED2_OFF; LED3_OFF; LED4_OFF; }  //730 ~ 3.5V
	else if(battvolt>690) { LED1_GRN; LED2_GRN; LED3_OFF; LED4_OFF; }  //690 ~ 3.7V
	else if(battvolt>670) { LED1_GRN; LED2_GRN; LED3_GRN; LED4_OFF; }  //670 ~ 3.8V		
	else if(battvolt>640) { LED1_GRN; LED2_GRN; LED3_GRN; LED4_GRN; }  //640 ~ 4.0V
	else                  { LED1_RED; LED2_RED; LED3_RED; LED4_RED; }  //>4.0V
	}


//-------------------------------------------------------------------------------------------------
void put_sleep(void)
	{
j1:	LIGHT_OFF;
	
	LEDS_OFF;
	
	IDLEN=0; SCS1=0; SCS0=0; //set sleep mmode
	
	IRCF2=0; IRCF1=0; IRCF0=0; //31kHz internal RC oscillator
	
	INTEDG0=0; //external interrupt 0 on falling edge
	INT0IE=1; //enable external interrupt 0 for wake-up
	
	SLEEP(); //mcu in sleep mmode
	
	IRCF2=1; IRCF1=1; IRCF0=1; //8MHz internal RC oscillator
	
	delay_ms(50); //debounce
	
	check_batt();
	
	if(battlow) {battlow=0; LED1_FLASH;  goto j1; }   //if battery is low then return to sleep mmode
			
	if(BUT1_PIN) goto j1;  //if button not pressed then return to sleep mmode
	
	displ_batt();
	
	LIGHT_ON;
				
	while(!BUT1_PIN); //wait button 2
	
	but1cnt=0; //reset all buttons
	but1stat=0;
	but2cnt=0;
	but2stat=0;
	
	refrcnt=0;
	flashcnt=0;
	}


//-------------------------------------------------------------------------------------------------
void init_mcu(void)
	{
	IRCF2=1; IRCF1=1; IRCF0=1; //8MHz internal RC oscillator
	
	SWDTEN=0; //disable watchdog timer

	ADCON0=0b000001;    //Channel 0  //A/D converter mmodule is enabled
	ADCON1=0b001110;    //Voltage Reference  //A/D Port Configuration Control bits
	ADCON2=0b10000101;  //A/D Result Format  //000=0TAD  //110=FOSC/64, 010=FOSC/32, 101=FOSC/16

	CCP2CON=0b1100;    //CCP in PWM mmode
	CCPR2=PWM_MIN;      //PWM duty cycle
	T2CON=0b00000100;  //Timer2 On (for PWM)
	
	T0CON=0b10010011;  //prescaler 011 - 1:16  Fosc/4=2000, 2000/16=125kHz
	TMR0=TMR0_LOAD;    //preload  //65536-64286=1250 125k/1250=100Hz
	TMR0IE=1;          //timer0 overflow intterrupt enable
	
	RBPU=0; //enable internal pull-ups

	LIGHT_OFF; //switch off the light
	
	BUT_COM_DIR = OUTPUT;  //common wire for buttons
	BUT_COM_PIN = LOW;
	
	BUT1_DIR = INPUT;  //pins for buttons
	BUT2_DIR = INPUT;
	
	TL431_DIR = OUTPUT; 
	TL431_PIN = LOW;  //switch off the TL431
	
	LEDS_INIT;
	LEDS_OFF; //all dual color LEDs switch off
	
	delay_ms(800);
		
	GIE=1; //global interrupts enable
	PEIE=1;

	put_sleep(); //MCU sleep
	}


//-------------------------------------------------------------------------------------------------
void main(void) 
	{
	uint8_t pmod = 0;  //operating mmode: 0-moonlight, 1-maximum
	uint8_t fled = 0;  //flag for flashing LEDs
	
	init_mcu();
	
	for(;;)
		{
		if(refrcnt==0) //refresh
			{
			if(pmod) refrcnt=20; //x10ms
			else refrcnt=180; //1-2sec. pause
			
			check_batt(); //check battery //update the batt. voltage and the batt. low flag
			
			if(battlow) put_sleep(); //if battery is low, then go to sleep
			else displ_batt(); //else show batt. level on LED
			
			fled=1; //run the flash
			flashcnt=10; //duration of the flash  //x10ms
			}
			
		if(pmod==0 && fled==1 && flashcnt==0) { fled=0; LEDS_OFF; } //flashing LEDs in moonlight mmode 
		
		switch(but1stat) //check the button 1 state
			{
			case 1: //short press  //on/off
				but1stat=0;
				put_sleep();
				fled=0;
				break;
			
			case 2: //long press
				but1stat=0;
				LED4_FLASH;
				break;
			}
		
		switch(but2stat) //check the button 2 state
			{
			case 1: //a short press of the button  //change mmode
				but2stat=0;
				if(pmod==0) { CCPR2L=PWM_MAX; pmod=1; }
				else { CCPR2L=PWM_MIN; pmod=0; }
				refrcnt=0;
				flashcnt=0;
				fled=0;
				break;
			
			case 2: //long press
				but2stat=0;
				LED4_FLASH;
				break;
			}
		}
	}
	
