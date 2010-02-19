#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <util/delay.h>

/* set the UART baud rate */
#define BAUD_RATE 38400

/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER	 0x02
#define SW_MAJOR 0x01
#define SW_MINOR 0x10

/* onboard LED is used to indicate, that the bootloader was entered (3x flashing) */
/* if monitor functions are included, LED goes on after monitor was entered */
#define LED_DDR  DDRD
#define LED_PORT PORTD
#define LED_PIN  PIND
#define LED      PIND6

#define NUM_LED_FLASHES 3
#define MAX_TIME_COUNT 8000000L>>1

#define NUM_INPUT_BUFFERS 8

/* define various device id's */
/* manufacturer byte is always the same */
#define SIG1	0x1E	// Yep, Atmel is the only manufacturer of AVR micros.  Single source :(
#define SIG2	0x96
#define SIG3	0x09
#define PAGE_SIZE		0x080U   //128 words
#define PAGE_SIZE_BYTES	0x100U   //256 bytes

/* function prototypes */
void putch(char);
char getch(void);
void flash_led(uint8_t count);

/* main program starts here */
int main(void)
{
    //uint8_t ch;
    uint8_t cur_state[NUM_INPUT_BUFFERS];
    uint8_t i,j;
	
    asm volatile("nop\n\t");

    //initialize our serial port.
    UBRR0L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
    UBRR0H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    /* Enable internal pull-up resistor on pin D0 (RX), in order
    to supress line noise */
    DDRD &= ~_BV(PIND0);
    PORTD |= _BV(PIND0);

    /* Set data direction of PORTC for input */
    DDRC &= 0x00;
    PORTC |= 0xFF;

    /* set LED pin as output */
    LED_DDR |= _BV(LED);

    /* set data direction of PORTB for output */
    DDRB |= 0xFF;

    /* Initialize cur_state array assume all keys off */
    for(i = 0; i < NUM_INPUT_BUFFERS; ++i) {
        cur_state[i] = 0x00;
    }
    	
    flash_led(NUM_LED_FLASHES);

    /* forever loop */
    for (;;)
    {
	for(i = 0; i < NUM_INPUT_BUFFERS; ++i) {

	    PORTB = i;
	    uint8_t tmp = cur_state[i];
	    _delay_us(1);
	    cur_state[i] = PINC;
	    tmp = tmp ^ cur_state[i];
	    for(j = 0; j < 8; ++j) {
    		if(tmp & (1 << j)) {
		    uint8_t state_val = cur_state[i] & (1 << j) ? 1 : 0;

                    /* flash_led(1); */

                    /* Send changed event */
		    putch(0xFF);
		    putch(0xFE);
		    putch(state_val);
                    putch(i);
                    putch(j);
		    putch(0);
                    putch(0xFD);
		    putch(0xFC);
		}
	    }
	}

	/* Wait 20 ms before polling again */
	_delay_ms(20);
    }
    /* end of forever loop */
}


void putch(char ch)
{
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch;
}


char getch(void)
{
    while(!(UCSR0A & _BV(RXC0)));
    return UDR0;
}


void flash_led(uint8_t count)
{
    /* flash onboard LED three times to signal entering of bootloader */
	/* l needs to be volatile or the delay loops below might get
	optimized away if compiling with optimizations (DAM). */

    if (count == 0)
    {
      count = 3;
    }

	int8_t i;
    for (i = 0; i < count; ++i)
    {
	LED_PORT |= _BV(LED);
	_delay_ms(50);
	LED_PORT &= ~_BV(LED);
	_delay_ms(50);
    }
}
