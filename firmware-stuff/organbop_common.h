#ifndef ORGANBOP_COMMON_H
#define ORGANBOP_COMMON_H

/* Commands (all are 3 bytes long)      opcode     arg1                 arg2    results */
#define CMD_ENTER_COMMAND_MODE          0xB0    // 00                   00      no data. in command mode.
#define CMD_READ_BUFFER_VALUE           0xB1    // PORTB to send        00      one byte PINC
#define CMD_READ_ADC_VALUE              0xB2    // ADMUX to use         00      ADC status byte, ADCL, ADCH
#define CMD_READ_CONFIG_LIVE_COPY	0xB3	// offset to read from	length	length bytes of data from live config data.  Only valid before doing any other read config saved or backup commands (same buffer is used)
#define CMD_READ_CONFIG_SAVED_COPY      0xB4    // offset to read from  length  length bytes of data from saved config data.  Live copy data is destroyed.
#define CMD_READ_CONFIG_BACKUP_COPY     0xB5    // offset to read from  length  length bytes of data from backup config data  Live copy data is destroyed.
#define CMD_CHANGE_SAVED_CONFIG_BYTE    0xB6    // offset to change     value   no data.  saved config data was modified.
#define CMD_RESTORE_BACKUP_CONFIG       0xB7    // 00                   00      no data.  backup config data restored to saved config data area.  Live copy data is destroyed.
#define CMD_EXIT_COMMAND_MODE           0xBF    // 00                   00      no data.  firmware is reset, and will re-enter normal mode with (possibly new) saved config data.

/* Message framing */
#define MSG_HEAD        0xFF
#define SUBMSG_END      0x00
#define SUBMSG_MIN_SIZE 3
#define MSG_TRAIL       0xFD
#define MSG_END         0xFC

/* Message types */
#define MSG_TYPE_BOOTUP         0xF0  // submsgs: 1: bootmsg, 2: date, 3: version
#define MSG_TYPE_CMD_RSP        0xF3  // submsgs: 1: cmd data, [2: resulting data]
#define MSG_TYPE_MIDI_EVENT     0xFE  // NO submsgs! Standard MIDI event 3-byte format: op/chan, arg1, arg2

/* MIDI OPCODES */
#define MIDI_KEY_OFF            0b10000000
#define MIDI_KEY_ON             0b10010000
#define MIDI_CONTROL_CHANGE     0b10110000
#define MIDI_VELOCITY           0x40

#endif
