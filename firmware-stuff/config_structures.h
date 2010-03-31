#ifndef _CONFIG_STRUCTURES_H
#define _CONFIG_STRUCTURES_H

#include <inttypes.h>
#include <stddefs.h>

// Don't change this unless you really know what you're doing (all sorts of structures change size, address bits change, eeprom and sram layouts change...)
#define BUFFERS_PER_HALF_BOARD 4

struct command_t {
  uint8_t opcode;
  uint8_t arg1;
  uint8_t arg2;  
};

struct half_board_settings_t {
	uint8_t address_mask;
	uint8_t buffer_shift;
	uint8_t channel_num;
	uint8_t key_offset;
};

struct control_settings_t {
	uint8_t admux;
	uint8_t adc_shift;
	uint8_t channel_num;
	uint8_t control_num;
};

struct half_board_values_t {
	uint8_t buffer_values[BUFFERS_PER_HALF_BOARD];
};

struct control_values_t {
	uint8_t control_value;
	uint8_t rsvd[3];
};

#defined CONFIG_LAYOUT_VERSION 0x01
struct config_t {
  // basic settings (grouped into reasonable 32bit chunks)
	uint16_t config_copy_size;		// 00-01
	uint8_t config_layout_version;		// 02
	uint8_t rsvd0;				// 03
	
	uint8_t rsvd1;				// 04
	uint8_t firmware_date_string_length;	// 05
	uint16_t firmware_date_string_offset;	// 06-07
	
	uint8_t rsvd2;				// 08
	uint8_t firmware_boot_string_length;	// 09
	uint16_t firmware_boot_string_offset;	// 0A-0B
	
	uint8_t firmware_version;		// 0C
	uint8_t rsvd3;				// 0D
	uint16_t sleep_time;			// 0E-0F

	uint8_t uart_baud_rate_register_low;	// 10
	uint8_t uart_baud_rate_register_high;	// 11
	uint8_t rsvd4;				// 12
	uint8_t rsvd5;				// 13
	
	uint8_t rsvd6;				// 14
	uint8_t num_half_boards;		// 15
	uint16_t buffer_wait_time;		// 16-17
	
	uint8_t rsvd7;				// 18
	uint8_t num_controls;			// 19
	uint16_t adc_wait_time;			// 1A-1B
	
	uint8_t rsvd8[4];			// 1C-1F

  // string area
	char date_string[16];			// 20-2F
	char boot_string[16];			// 30-3F
	
  // half-board and control settings arrays
  	struct half_board_settings_t half_board_settings[8];	// 40-5F
	struct control_settings_t control_settings[8];		// 60-7F

  // half-board and control last value arrays
  	struct half_board_values_t half_board_values[8];	// 80-9F
	struct control_values_t control_values[8];		// A0-BF

  // reserved space for future expansion
  	char rsvd7[0x100-0xC0];					// C0-FF
};

#include config_defaults.h

// setbaud.h sets UBRRL_VALUE and UBRRH_VALUE based on F_CPU and BAUD
#define BAUD CFG_BAUD_RATE
#include <util/setbaud.h>


#define CONFIG_DEFAULT_INITIALIZER { \
	sizeof(struct config_t), CONFIG_LAYOUT_VERSION, RSVD_VAL, \
	RSVD_VAL, sizeof(FW_DATE), offsetof(struct config_t, date_string), \
	RSVD_VAL, sizeof(FW_BOOT), offsetof(struct config_t, boot_string), \
	FW_VERSION, RSVD_VAL, CFG_SLEEP_TIME, \
	UBRRL_VALUE, UBRRH_VALUE, RSVD_VAL, RSVD_VAL, \
	RSVD_VAL, CFG_NUM_HALF_BOARDS, CFG_BUFFER_WAIT_TIME, \
	RSVD_VAL, CFG_NUM_CONTROLS, CFG_ADC_WAIT_TIME, \
	{RSVD_VAL}, \
	FW_DATE, FW_BOOT, \
	{ \
		{ CFG_ADDR_MASK_H0, CFG_BUF_SHIFT_H0, CFG_CHAN_NUM_H0, CFG_KEY_OFFS_H0 }, \
		{ CFG_ADDR_MASK_H1, CFG_BUF_SHIFT_H1, CFG_CHAN_NUM_H1, CFG_KEY_OFFS_H1 }, \
		{ CFG_ADDR_MASK_H2, CFG_BUF_SHIFT_H2, CFG_CHAN_NUM_H2, CFG_KEY_OFFS_H2 }, \
		{ CFG_ADDR_MASK_H3, CFG_BUF_SHIFT_H3, CFG_CHAN_NUM_H3, CFG_KEY_OFFS_H3 }, \
		{ CFG_ADDR_MASK_H4, CFG_BUF_SHIFT_H4, CFG_CHAN_NUM_H4, CFG_KEY_OFFS_H4 }, \
		{ CFG_ADDR_MASK_H5, CFG_BUF_SHIFT_H5, CFG_CHAN_NUM_H5, CFG_KEY_OFFS_H5 }, \
		{ CFG_ADDR_MASK_H6, CFG_BUF_SHIFT_H6, CFG_CHAN_NUM_H6, CFG_KEY_OFFS_H6 }, \
		{ CFG_ADDR_MASK_H7, CFG_BUF_SHIFT_H7, CFG_CHAN_NUM_H7, CFG_KEY_OFFS_H7 } \
	}, { \
		{ CFG_ADMUX_C0, CFG_ADC_SHIFT_C0, CFG_CHAN_NUM_C0, CFG_CNTRL_NUM_C0 }, \
		{ CFG_ADMUX_C1, CFG_ADC_SHIFT_C1, CFG_CHAN_NUM_C1, CFG_CNTRL_NUM_C1 }, \
		{ CFG_ADMUX_C2, CFG_ADC_SHIFT_C2, CFG_CHAN_NUM_C2, CFG_CNTRL_NUM_C2 }, \
		{ CFG_ADMUX_C3, CFG_ADC_SHIFT_C3, CFG_CHAN_NUM_C3, CFG_CNTRL_NUM_C3 }, \
		{ CFG_ADMUX_C4, CFG_ADC_SHIFT_C4, CFG_CHAN_NUM_C4, CFG_CNTRL_NUM_C4 }, \
		{ CFG_ADMUX_C5, CFG_ADC_SHIFT_C5, CFG_CHAN_NUM_C5, CFG_CNTRL_NUM_C5 }, \
		{ CFG_ADMUX_C6, CFG_ADC_SHIFT_C6, CFG_CHAN_NUM_C6, CFG_CNTRL_NUM_C6 }, \
		{ CFG_ADMUX_C7, CFG_ADC_SHIFT_C7, CFG_CHAN_NUM_C7, CFG_CNTRL_NUM_C7 } \
	}, \
	{UNSET_VAL}, {UNSET_VAL}, \
	{UNSET_VAL} }


	




#endif

