/* Title: LED Interrupt
 *
 * EECS 497.32: Programming Assignment #5
 *
 * Author: Nicholas LaJeunesse
 */

#include "drvreg.h"

// sei - enable interrupts
#define sei() __asm__ __volatile__ ("sei" ::)

// Write value to register
void WriteReg(int RegAddr, int value){
	*((volatile unsigned char *)RegAddr) = value;
}

// Read value from register
unsigned char ReadReg(int RegAddr){
	unsigned char val;
    do
	{
	  val = *((volatile unsigned char *)RegAddr);
	}while( (val != *((volatile unsigned char *)RegAddr)) );  

	return (val);  
}

// Initialize Timer0
void Timer_init(void){
    // Set Prescalar to 1024
	WriteReg(TCCR0B, (1<<CS02)|(1<<CS00)); 
    
    // Set Delay Count
	WriteReg(TCNT0, 0);
    
    // Enable Overflow Interrupt
	WriteReg(TIMSK0, 1<<TOIE0);
    
    // Enable global interrupts
    sei();
}

extern void __vector_23 (void) __attribute__ ((interrupt));
// Global data
unsigned char timer_isr_state = 0;
unsigned char LED_index = 1;

// Called whenever TCNT0 overflows
void __vector_23(void){
    switch (timer_isr_state){
        case 0:
            // Read LED Switches
            if(ReadReg(PINA) != 0xff){
				if((~ReadReg(PINA) & LED_index) == LED_index){
					timer_isr_state = 1;
                } else{
                    timer_isr_state = 2;
                }
            } else{
                timer_isr_state = 1;
            }
            break;
        case 1:
            // Turn LED of interest ON
            WriteReg(PORTB, ~LED_index);
            timer_isr_state = 2;
            break;
        case 2:
            // Turn all LEDS OFF
            WriteReg(PORTB, 0xff);
            timer_isr_state = 0;
            break;
    }
    
    // Cycle through LED0 - LED7
    if(timer_isr_state == 0){
        if(LED_index == (1<<7)){
            LED_index = 1;
        } else{
            LED_index <<= 1;
        }
    }

}

int main(void){
    // Set PORTB DATA Direction (DDRB) as output
    WriteReg(DDRB, 0xff);
    
    // Turn OFF all LEDS by writing 1's to PORTB
    WriteReg(PORTB, 0xff);

    // Set PORTA DATA Direction (DDRA) as input
    WriteReg(DDRA, 0x0);

    // Enable internal pull-ups for all PORTA
    WriteReg(PORTA, 0xff);
    
    Timer_init();
    
    while(1){;}
    
    return (0);
} 
