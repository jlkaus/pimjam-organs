#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>


#ifndef F_CPU
#error Need F_CPU to be defined!
#endif

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
/#error Need gcc version 4.3.0 at minimum (for the 0bxxxxxxxx binary constants.)
#endif


/* firmware version information (gets put into the config data in EEPROM, along with the __TIMESTAMP__ value) */
#define FW_BOOT "OrganBop BOOTED"
#define FW_VERSION 0x11
#include "config_structures.h"

/* LED Flash codes */
// when in normal mode, LED is solid off
// when in command mode waiting for command, LED is solid on
// when in command mode processing command, LED is solid off
#define SRC_ENTERING_COMMAND_MODE	1
#define SRC_ENTERING_NORMAL_MODE	3
#define SRC_INVALID_CONFIG_HANG		5
#define SRC_INVALID_COMMAND		7

/* Commands (all are 3 bytes long)	opcode	   arg1			arg2	results */
#define CMD_ENTER_COMMAND_MODE		0xB0	// 00			00	no data. in command mode.
#define CMD_READ_BUFFER_VALUE		0xB1	// PORTB to send	00	one byte PINC
#define CMD_READ_ADC_VALUE		0xB2	// ADMUX to use		00	ADC status byte, ADCL, ADCH
#define CMD_READ_CONFIG_LIVE_COPY	0xB3	// offset to read from	length	length bytes of data from live config data.  Only valid before doing any other read config saved or backup commands (same buffer is used)
#define CMD_READ_CONFIG_SAVED_COPY	0xB4	// offset to read from  length	length bytes of data from saved config data.  Live copy data is destroyed.
#define CMD_READ_CONFIG_BACKUP_COPY	0xB5	// offset to read from	length	length bytes of data from backup config data  Live copy data is destroyed.
#define CMD_CHANGE_SAVED_CONFIG_BYTE	0xB6	// offset to change	value	no data.  saved config data was modified.
#define CMD_RESTORE_BACKUP_CONFIG	0xB7	// 00			00	no data.  backup config data restored to saved config data area.  Live copy data is destroyed.
#define CMD_EXIT_COMMAND_MODE		0xBF	// 00			00	no data.  firmware is reset, and will re-enter normal mode with (possibly new) saved config data.

/* Message framing */
#define MSG_HEAD	0xFF
#define SUBMSG_END	0x00
#define SUBMSG_MIN_SIZE	3
#define MSG_TRAIL	0xFD
#define MSG_END		0xFC

/* Message types */
#define MSG_TYPE_BOOTUP		0xF0  // submsgs: 1: bootmsg, 2: date, 3: version
#define MSG_TYPE_CMD_RSP	0xF3  // submsgs: 1: cmd data, [2: resulting data]
#define MSG_TYPE_MIDI_EVENT	0xFE  // NO submsgs! Standard MIDI event 3-byte format: op/chan, arg1, arg2

/* MIDI OPCODES */
#define MIDI_KEY_OFF		0b10000000
#define MIDI_KEY_ON		0b10010000
#define MIDI_CONTROL_CHANGE	0b10110000
#define MIDI_VELOCITY		0x40

/* defines ports and pins to use for various functions */
// Port A
// Used for ADC functions.  (All 8 bits for now, though only bits 6 and 7 have wires from them to potential analog sources)
#define ADC_MASK 0xFF

// Port B
// Used for sending address to buffer boards (high 7 bits) and the status LED control (low 1 bit)
#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
#define LED_BIT  PINB0

#define ADDR_DDR DDRB
#define ADDR_PORT PORTB
#define ADDR_PIN PINB
#define ADDR_MASK 0xFE

// Port C
// Used for receiving data from the buffer boards (all 8 bits)
#define DATA_DDR DDRC
#define DATA_PORT PORTC
#define DATA_PIN PINC
#define DATA_MASK 0xFF

// Port D
// Used for sending and receiving data to the USB link to the host.  (low 2 bits)  6 unused digital bits.
#define UART_DDR DDRD
#define UART_PORT PORTD
#define UART_PIN PIND
#define UART_RX_BIT PIND0
#define UART_TX_BIT PIND1



/* global vars */
struct config_t liveCopy; // Lives in SRAM.  Loaded from savedCopy at reset. Values from here are used throughout the FW.
struct config_t savedCopy EEMEM = CONFIG_DEFAULT_INITIALIZER;  // Lives in low EEPROM.  Loaded to liveCopy at reset.  Change using command mode to change FW behavior.
struct config_t backupCopy EEMEM = CONFIG_DEFAULT_INITIALIZER;  // Lives in high EEPROM.  Can be used to reset savedCopy if you mess things up.  Reload the EEPROM to change these.


/* function prototypes */
void putch(uint8_t);
uint8_t getch(void);
uint8_t getch_async(void);

void flash_led(uint8_t count);
void src_hang(uint8_t count);
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void soft_reset(void);

void send_midi_msg(uint8_t opcode, uint8_t channel, uint8_t arg1, uint8_t arg2);
void send_boot_msg();
void send_response_msg(struct command_t* cmd, void* response_data, uint8_t response_data_len);

void send_msg_header(uint8_t type, uint16_t length);
void send_submsg(uint8_t num, uint8_t length, void* data);
void send_msg_footer();

void doSetup();
void doCommandMode();
void doNormalMode();
void restoreDefaultConfig();
void loadSavedConfig();
uint8_t loadEepromConfigData(uint8_t offset, uint8_t fromBackup, uint8_t len);  // return 1 if offset/length were ok and we loaded the data to liveCopy, otherwise 0, and no data was loaded.
void checkHalfBoardForChanges(uint8_t hbn);
void checkControlForChanges(uint8_t ctl);

/* main program starts here */
int main(void)
{
    asm volatile("nop\n\t");

    // load the config, then setup the ports and stuff and send our bootup msg
    loadSavedConfig();
    doSetup();
    send_boot_msg();

    // Here is where we spend most of our time, processing organ keypresses, etc.
    doNormalMode();

    // We exited normal mode, so we must have wanted to go into command mode
    doCommandMode();

    // We are done in command mode, so reset the AVR to get the new config data and go back into normal mode.
    soft_reset();
}


void putch(uint8_t ch)
{
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = ch;
}


uint8_t getch(void)
{
    while(!(UCSR0A & _BV(RXC0)));
    return UDR0;
}

uint8_t getch_async(void)
{
  if(UCSR0A & _BV(RXC0)) {
    return UDR0;
  } else {
    return 0x00;
  }
}

void flash_led(uint8_t count)
{
  /* l needs to be volatile or the delay loops below might get
     optimized away if compiling with optimizations (DAM). */

  int8_t i;
  for (i = 0; i < count; ++i) {
    LED_PORT |= _BV(LED_PIN);
    _delay_ms(100);
    LED_PORT &= ~_BV(LED_PIN);
    _delay_ms(100);
  }
}

void src_hang(uint8_t count)
{
  // hit an error, so well just hang ourselves, flashing the error code over and over.
  for(;;) {
    flash_led(count);
    _delay_ms(1000);
  }
}

void wdt_init(void)
{
  // make sure we don't get reset continuously
    MCUSR = 0;
    wdt_disable();
    return;
}

void soft_reset(void)
{
  wdt_enable(WDTO_15MS);
  for(;;) {
    // do nothing, just wait to be blown away...
  }
}


void send_msg_header(uint8_t type, uint16_t length)
{
  putch(MSG_HEAD);
  putch(type);
  putch((char)(length >> 8));
  putch((char)(length & 0xFF));
  return;
}

void send_boot_msg()
{
    send_msg_header(MSG_TYPE_BOOTUP, SUBMSG_MIN_SIZE+liveCopy.firmware_date_string_length + SUBMSG_MIN_SIZE+liveCopy.firmware_boot_string_length + SUBMSG_MIN_SIZE+1);
    send_submsg(1, liveCopy.firmware_boot_string_length, &liveCopy + liveCopy.firmware_boot_string_offset);
    send_submsg(2, liveCopy.firmware_date_string_length, &liveCopy + liveCopy.firmware_date_string_offset);
    send_submsg(3, 1, &liveCopy.firmware_version);
    send_msg_footer();
    return;
}

void send_response_msg(struct command_t* cmd, void* response_data, uint8_t response_data_len )
{
  send_msg_header(MSG_TYPE_CMD_RSP, SUBMSG_MIN_SIZE+sizeof(struct command_t) + SUBMSG_MIN_SIZE+response_data_len);
  send_submsg(1, sizeof(struct command_t), cmd);
  send_submsg(2, response_data_len, response_data);
  send_msg_footer();
  return;
}

void send_midi_msg(uint8_t opcode, uint8_t channel, uint8_t arg1, uint8_t arg2)
{
  send_msg_header(MSG_TYPE_MIDI_EVENT, 3);
  putch(opcode | channel);
  putch(arg1);
  putch(arg2);
  send_msg_footer();
  return;
}

void send_submsg(uint8_t num, uint8_t length, void* data)
{
  if(length && data) {
    putch(num);
    putch(length);

    for(int i=0; i < length; ++i) {
      putch(((char*)data)[i]);
    }

    putch(SUBMSG_END);
  }
  return;
}

void send_msg_footer()
{
  putch(MSG_TRAIL);
  putch(MSG_END);
  return;
}

void doSetup()
{
    // Serial port initialization
    UBRR0L = liveCopy.uart_baud_rate_register_low;
    UBRR0H = liveCopy.uart_baud_rate_register_high;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    // enable internal pull-up for UART RX to suppress line noise
    UART_DDR &= ~_BV(UART_RX_BIT);
    UART_PORT |= _BV(UART_RX_BIT);

    // Set up data bus for input
    DATA_DDR &= ~DATA_MASK;
    DATA_PORT |= DATA_MASK;

    // set up LED pin for output
    LED_DDR |= _BV(LED_BIT);
    LED_PORT &= ~_BV(LED_BIT);

    // set up address bus for ouput
    ADDR_DDR |= ADDR_MASK;
    ADDR_PORT &= ~ADDR_MASK;

    // ADC initialization
    ADCSRB = 0x00; /* Select free running mode */
    DIDR0 = ADC_MASK;  /* Disable digital inputs on analog pins */
    ADCSRA = 0xE7; /* 0xE0 - Enables & starts A/D Converter */
                   /* 0x07 - Sets frequency prescaler to 128 */
    return;
}

void doNormalMode()
{
    flash_led(SRC_ENTERING_NORMAL_MODE);
    for (;;)
    {
      if(getch_async() == CMD_ENTER_COMMAND_MODE) {
	break;
      }

      for(int i=0; i < liveCopy.num_half_boards; ++i) {
	checkHalfBoardForChanges(i);
      }

      for(int i=0; i < liveCopy.num_controls; ++i) {
	checkControlForChanges(i);
      }

      _delay_us(liveCopy.sleep_time);
    }
    return;
}

void doCommandMode()
{
    struct command_t current_command;
    uint8_t data_len = 0;
    char* data_ptr = NULL;
    char temp_data[3];


    // we assume on entry that we already received the CMD_ENTER_COMMAND_MODE byte
    current_command.opcode = CMD_ENTER_COMMAND_MODE;
    current_command.arg1 = getch();
    current_command.arg2 = getch();

    send_response_msg(&current_command, data_ptr, data_len);

    flash_led(SRC_ENTERING_COMMAND_MODE);

    for(;;) {
      // turn on LED while waiting for command
      LED_PORT |= _BV(LED_PIN);
      current_command.opcode = getch();
      current_command.arg1 = getch();
      current_command.arg2 = getch();

      // turn off LED while processing command
      LED_PORT &= ~_BV(LED_PIN);

      switch(current_command.opcode) {

      case CMD_ENTER_COMMAND_MODE:
	// already in command mode. Nothing to do.
	data_ptr = NULL;
	data_len = 0;
	break;

      case CMD_READ_BUFFER_VALUE:
	PORTB = current_command.arg1;
	_delay_us(liveCopy.buffer_wait_time);
	temp_data[0] = PINC;
	data_ptr = temp_data;
	data_len = 1;
	break;

      case CMD_READ_ADC_VALUE:
	ADMUX = current_command.arg1;
	// after changing ADMUX, we need to discard the next read value.
	_delay_us(liveCopy.adc_wait_time);
	temp_data[0] = ADCSRA;
	temp_data[1] = ADCL;
	temp_data[2] = ADCH;
	_delay_us(liveCopy.adc_wait_time);
	temp_data[0] = ADCSRA;
	temp_data[1] = ADCL;
	temp_data[2] = ADCH;
	data_ptr = temp_data;
	data_len = 3;
	break;

      case CMD_READ_CONFIG_LIVE_COPY:
	data_ptr = ((char*)&liveCopy) + current_command.arg1;
	data_len = current_command.arg2;
	break;

      case CMD_READ_CONFIG_SAVED_COPY:
	if(loadEepromConfigData(current_command.arg1, 0, current_command.arg2)) {
	  data_ptr = ((char*)&liveCopy) + current_command.arg1;
	  data_len = current_command.arg2;
	} else {
	  flash_led(SRC_INVALID_COMMAND);
	  data_ptr = NULL;
	  data_len = 0;
	}
	break;

      case CMD_READ_CONFIG_BACKUP_COPY:
	if(loadEepromConfigData(current_command.arg1, 1, current_command.arg2)) {
	  data_ptr = ((char*)&liveCopy) + current_command.arg1;
	  data_len = current_command.arg2;
	} else {
	  flash_led(SRC_INVALID_COMMAND);
	  data_ptr = NULL;
	  data_len = 0;
	}
	break;

      case CMD_CHANGE_SAVED_CONFIG_BYTE:
	eeprom_write_byte(((uint8_t*)&savedCopy) + current_command.arg1, current_command.arg2);
	data_ptr = NULL;
	data_len = 0;
	break;

      case CMD_RESTORE_BACKUP_CONFIG:
	restoreDefaultConfig();
	data_ptr = NULL;
	data_len = 0;
	break;
	
      case CMD_EXIT_COMMAND_MODE:
	// We'll check for this op again later, after sending the response.
	data_ptr = NULL;
	data_len = 0;
	break;

      default:
	// I don't understand. flash SRC, but then just wait for another command.
	flash_led(SRC_INVALID_COMMAND);
	data_ptr = NULL;
	data_len = 0;
      }

      send_response_msg(&current_command, data_ptr, data_len);

      if(current_command.opcode == CMD_EXIT_COMMAND_MODE) {
	break;
      }
  }
  return;
}

void restoreDefaultConfig()
{
  eeprom_read_block(&liveCopy, &backupCopy, sizeof(struct config_t));

  if((liveCopy.config_layout_version != CONFIG_LAYOUT_VERSION) || (liveCopy.config_copy_size != sizeof(struct config_t))) {
    // something must be wrong.  Hang.
    src_hang(SRC_INVALID_CONFIG_HANG);
  } else {
    eeprom_write_block(&liveCopy, &savedCopy, sizeof(struct config_t));
  }
  return;
}

void loadSavedConfig()
{
  eeprom_read_block(&liveCopy, &savedCopy, sizeof(struct config_t));

  if((liveCopy.config_layout_version != CONFIG_LAYOUT_VERSION) || (liveCopy.config_copy_size != sizeof(struct config_t))) {
    // something must be wrong.  Hang.
    src_hang(SRC_INVALID_CONFIG_HANG);
  }
  return;
}

uint8_t loadEepromConfigData(uint8_t offset, uint8_t fromBackup, uint8_t len)
{
  if(offset+len < sizeof(struct config_t)) {
    eeprom_read_block(&liveCopy + offset, (fromBackup?(&backupCopy):(&savedCopy)) + offset, len);
  } else {
    return 0;
  }
  return 1;
}

void checkHalfBoardForChanges(uint8_t hbn)
{
  for(int i = 0; i < BUFFERS_PER_HALF_BOARD; ++i) {
    PORTB = (liveCopy.half_board_settings[hbn].address_mask | (i << liveCopy.half_board_settings[hbn].buffer_shift));

    uint8_t tmp = liveCopy.half_board_values[hbn].buffer_values[i];

    _delay_us(liveCopy.buffer_wait_time);

    uint8_t tmp2 = PINC;
    liveCopy.half_board_values[hbn].buffer_values[i] = tmp2;

    tmp = tmp ^ tmp2;
    for(int j = 0; j < 8; ++j) {
      if(tmp & (1 << j)) {
	uint8_t opcode = tmp2 & (1 << j) ? MIDI_KEY_ON : MIDI_KEY_OFF;

	send_midi_msg(opcode, liveCopy.half_board_settings[hbn].channel_num, i*8 + j + liveCopy.half_board_settings[hbn].key_offset, MIDI_VELOCITY);
      }
    }
  }
  return;
}

void checkControlForChanges(uint8_t ctl)
{
  uint16_t adc_value = 0;
  uint8_t new_control_value = 0;

  ADMUX = liveCopy.control_settings[ctl].admux;
  // after changing ADMUX, we need to discard the next read value.
  _delay_us(liveCopy.adc_wait_time);
  adc_value = ADCL;
  adc_value |= (ADCH << 8);
  _delay_us(liveCopy.adc_wait_time);
  adc_value = ADCL;
  adc_value |= (ADCH << 8);
  new_control_value = (adc_value >> liveCopy.control_settings[ctl].adc_shift) & 0xff;

  if(new_control_value != liveCopy.control_values[ctl].control_value) {
    // control change, so save new value and send an event

    liveCopy.control_values[ctl].control_value = new_control_value;

    send_midi_msg(MIDI_CONTROL_CHANGE, liveCopy.control_settings[ctl].channel_num, liveCopy.control_settings[ctl].control_num, new_control_value);
  }
  return;
}




