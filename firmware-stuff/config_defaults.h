#ifndef _CONFIG_DEFAULTS_H
#define _CONFIG_DEFAULTS_H

#define RSVD_VAL 0x00
#define UNSET_VAL 0xFF

#define CFG_SLEEP_TIME 20000
#define CFG_BAUD_RATE 38400
#define CFG_BUFFER_WAIT_TIME 5
#define CFG_ADC_WAIT_TIME 5

#define CFG_NUM_HALF_BOARDS 6

//                       	0bzzrrhbbl
#define CFG_ADDR_MASK_H0 	0b11000000
#define CFG_BUF_SHIFT_H0 	1
#define CFG_CHAN_NUM_H0 	0x0
#define CFG_KEY_OFFS_H0 	36

#define CFG_ADDR_MASK_H1 	0b11001000
#define CFG_BUF_SHIFT_H1 	1
#define CFG_CHAN_NUM_H1		0x0
#define CFG_KEY_OFFS_H1 	68

#define CFG_ADDR_MASK_H2 	0b01100000
#define CFG_BUF_SHIFT_H2 	1
#define CFG_CHAN_NUM_H2		0x1
#define CFG_KEY_OFFS_H2 	36

#define CFG_ADDR_MASK_H3 	0b01101000
#define CFG_BUF_SHIFT_H3 	1
#define CFG_CHAN_NUM_H3 	0x1
#define CFG_KEY_OFFS_H3 	68

#define CFG_ADDR_MASK_H4 	0b00110000
#define CFG_BUF_SHIFT_H4 	1
#define CFG_CHAN_NUM_H4 	0x7
#define CFG_KEY_OFFS_H4 	36

#define CFG_ADDR_MASK_H5 	0b00111000
#define CFG_BUF_SHIFT_H5 	1
#define CFG_CHAN_NUM_H5 	0x6
#define CFG_KEY_OFFS_H5 	0

#define CFG_ADDR_MASK_H6	UNSET_VAL
#define CFG_BUF_SHIFT_H6	UNSET_VAL
#define CFG_CHAN_NUM_H6		UNSET_VAL
#define CFG_KEY_OFFS_H6		UNSET_VAL

#define CFG_ADDR_MASK_H7	UNSET_VAL
#define CFG_BUF_SHIFT_H7	UNSET_VAL
#define CFG_CHAN_NUM_H7		UNSET_VAL
#define CFG_KEY_OFFS_H7		UNSET_VAL



#define CFG_NUM_CONTROLS 1

//                   		0brrdddaaa
#define CFG_ADMUX_C0 		0b11000111
#define CFG_ADC_SHIFT_C0 	3
#define CFG_CHAN_NUM_C0 	0x1
#define CFG_CNTRL_NUM_C0 	0x0B

#define CFG_ADMUX_C1 		UNSET_VAL
#define CFG_ADC_SHIFT_C1 	UNSET_VAL
#define CFG_CHAN_NUM_C1 	UNSET_VAL
#define CFG_CNTRL_NUM_C1 	UNSET_VAL

#define CFG_ADMUX_C2 		UNSET_VAL
#define CFG_ADC_SHIFT_C2 	UNSET_VAL
#define CFG_CHAN_NUM_C2 	UNSET_VAL
#define CFG_CNTRL_NUM_C2 	UNSET_VAL

#define CFG_ADMUX_C3 		UNSET_VAL
#define CFG_ADC_SHIFT_C3 	UNSET_VAL
#define CFG_CHAN_NUM_C3 	UNSET_VAL
#define CFG_CNTRL_NUM_C3 	UNSET_VAL

#define CFG_ADMUX_C4 		UNSET_VAL
#define CFG_ADC_SHIFT_C4 	UNSET_VAL
#define CFG_CHAN_NUM_C4 	UNSET_VAL
#define CFG_CNTRL_NUM_C4 	UNSET_VAL

#define CFG_ADMUX_C5 		UNSET_VAL
#define CFG_ADC_SHIFT_C5 	UNSET_VAL
#define CFG_CHAN_NUM_C5 	UNSET_VAL
#define CFG_CNTRL_NUM_C5 	UNSET_VAL

#define CFG_ADMUX_C6 		UNSET_VAL
#define CFG_ADC_SHIFT_C6 	UNSET_VAL
#define CFG_CHAN_NUM_C6 	UNSET_VAL
#define CFG_CNTRL_NUM_C6 	UNSET_VAL

#define CFG_ADMUX_C7 		UNSET_VAL
#define CFG_ADC_SHIFT_C7 	UNSET_VAL
#define CFG_CHAN_NUM_C7 	UNSET_VAL
#define CFG_CNTRL_NUM_C7 	UNSET_VAL

#endif