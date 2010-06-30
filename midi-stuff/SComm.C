#include "Env.H"
#include "SComm.H"
#include "Event.H"
#include "EventHandler.H"
#include "organbop_common.h"

#include <stdio.h>   /* Standard input/output definitions */
#include <sys/types.h> /* system types */
#include <sys/stat.h>  /* system stat stuff */
#include <sys/ioctl.h> /* system ioctls */
#include <stdlib.h>  /* standard library stuff */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>

SComm::~SComm() 
{
	Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying SComm event generator");
	if(xProcessEvents) {
		stopEvents();
	}

	if(xCommFd != -1) {
		close(xCommFd);
		xCommFd = -1;
	}
}

void* SComm::eventLoop(void* args) 
{
	SComm* this_scom = (SComm*)args;

	this_scom->softReset();

	while(this_scom->xProcessEvents) {
		this_scom->receiveMessage(MSG_TYPE_MIDI_EVENT, &handleMidiEventMsg);
	}

	close(this_scom->xCommFd);
	this_scom->xCommFd = -1;
}

void SComm::softReset()
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm - Initiating soft reset");

	// Issue the commands to the organ firmware to soft reset
	//   this is done by entering "Command mode" and immediately exiting
	
	// Enter command mode command 0xB00000
	struct cmd_mode_command_t enter_command_mode = { CMD_ENTER_COMMAND_MODE, 0x00, 0x00 };

	// Exit command mode commmand 0xBF0000
	struct cmd_mode_command_t exit_command_mode = { CMD_EXIT_COMMAND_MODE, 0x00, 0x00 };

        sendAndProcessCommandModeCommand(enter_command_mode);
        sendAndProcessCommandModeCommand(exit_command_mode);

	// Process the boot message
	receiveMessage(MSG_TYPE_BOOTUP, &handleBootMsg);
}

void SComm::handleBootMsg(SComm* this_scom, void* response_data, size_t response_data_size)
{
	struct sub_msg_header_t* msg_header = (sub_msg_header_t*)response_data;

	// Boot message consists of two string messages and the firmware version
	//   String Messages: Boot welcome and current date
	//   Log the boot welcome and current date
	for(int i = 0; i < 2; ++i) {
		char boot_msg_format[20];
		snprintf(boot_msg_format, sizeof(boot_msg_format), "Boot Msg: %%.%ds", msg_header->msg_size);
		Env::logMsg(Env::OperationMsg, Env::Debug, boot_msg_format, (char*)(msg_header) + sizeof(sub_msg_header_t));
		msg_header = (sub_msg_header_t*)(((char*)msg_header) + sizeof(sub_msg_header_t) + msg_header->msg_size + 1);
	}

	// Log the firmware version
	uint8_t firmware_version_size = 1;

	if(msg_header->msg_size != firmware_version_size) {
		Env::errorMsg("Boot message format error, firmware version length mismatch.  Expected: 0x%02X Received: 0x%02X", firmware_version_size, msg_header->msg_size);
		throw 9;
	}

	Env::logMsg(Env::OperationMsg, Env::Debug, "Firmware Version: 0x%02X", *((uint8_t*)(msg_header) + sizeof(sub_msg_header_t)));
}

void SComm::handleMidiEventMsg(SComm* this_scom, void* response_data, size_t response_data_size)
{
	struct midi_event_t* midi_event = (midi_event_t*)response_data;
	Env::logMsg(Env::OperationMsg, Env::Debug, "MIDI Event: 0x%02X 0x%02X 0x%02X", midi_event->op_channel, midi_event->arg1, midi_event->arg2);
	
	uint8_t op = (midi_event->op_channel & 0xF0);
	uint8_t channel = (midi_event->op_channel & 0x0F);
	switch(op) {
		case MIDI_KEY_OFF:
		{
			Input key_input(channel, midi_event->arg1);
			Event key_event(key_input, 0);  // Key off
			this_scom->xEventHandler->enqueueEvent(key_event);
			break;
		}
		case MIDI_KEY_ON:
		{
			Input key_input(channel, midi_event->arg1);
			Event key_event(key_input, 1);  // Key on
			this_scom->xEventHandler->enqueueEvent(key_event);
			break;
		}
		case MIDI_CONTROL_CHANGE:
		{
			Input control_input(channel, midi_event->arg1, true);
			Event control_event(control_input, midi_event->arg2);
			this_scom->xEventHandler->enqueueEvent(control_event);
			break;
		}
		default:
		{
			Env::errorMsg("Unknown midi opcode: %02X", midi_event->op_channel);
			throw 9;
			break;
		}
	}
}

void SComm::sendAndProcessCommandModeCommand(const cmd_mode_command_t& cmd, MsgResponseHandler* response_handler)
{
	// Send the command
	streamWrite(&cmd, sizeof(cmd_mode_command_t));

	// Process the response
	receiveMessage(MSG_TYPE_CMD_RSP, response_handler);
}

void SComm::receiveMessage(uint8_t msg_type, MsgResponseHandler* response_handler)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "Receive message of type: 0x%02X", msg_type);

	// Wait for the message response
	// Message response format FFTTSSSSMM...MMFDFC
	//   Where 
	//     FF is the message header
	//     TT is the message subtype
	//     SSSS is the message size (includes everything from the subtype to the end of the message data)
	//     MM...MM is the message data
	//     FDFC is the message footer
	//   There is no guarentee that the stream is synced.  So there might be other data in stream that is not
	//     the response.  Throw that data away and wait for the response of the specified type.

	uint8_t response_header[2] = {MSG_HEAD, msg_type};
	streamWaitForByteSequence(response_header, 2);
  
	// Retrieve the message response data
	uint16_t msg_size;
	streamRead(&msg_size, sizeof(uint16_t));
	msg_size = ntohs(msg_size);
	Env::logMsg(Env::OperationMsg, Env::Debug, "Received message size: 0x%02X", msg_size);
  
	uint8_t* response_buffer = new uint8_t[msg_size];
	streamRead(response_buffer, msg_size);

	if(Env::getOperationLoudness() >= Env::Debug) {
		for(int i = 0; i < msg_size; ++i) {
			Env::logMsg(Env::OperationMsg, Env::Debug, "Response_buffer[%d] = 0x%02X", i, response_buffer[i]);
		}
	}

	// Retrieve the message footer
	streamReadExpectedByte(MSG_TRAIL);
	streamReadExpectedByte(MSG_END);

	// If a response handler is defined, call it
	if(response_handler) {
		response_handler(this, response_buffer, msg_size);
	}

	delete [] response_buffer;
}


void SComm::streamRead(void* buffer, size_t read_size)
{
	uint8_t* buffer_position = (uint8_t*)buffer;
	size_t total_bytes_read = 0;

	while(total_bytes_read < read_size) {
		Env::logMsg(Env::OperationMsg, Env::Debug, "Read 0x%02X bytes", read_size - total_bytes_read);
		ssize_t size_read = read(xCommFd, buffer_position, read_size - total_bytes_read);
		if(size_read == -1) {
			Env::errorMsg("Unable to read from stream: %s", strerror(errno));
			throw 9;
		}
	       
		total_bytes_read += size_read;
		buffer_position += size_read;
	}
}

void SComm::streamReadExpectedByte(unsigned char expected_byte)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "Read expected byte: 0x%02X", expected_byte);
	
	unsigned char byte_read = 0;
	streamRead(&byte_read, 1);

	if(byte_read != expected_byte) {
		Env::errorMsg("Fatal error receiving data from stream.  Expected byte: 0x%02X - Received byte: 0x%02X",expected_byte, byte_read);
		throw 9;
	}
}

void SComm::streamWaitForByteSequence(const uint8_t* byte_sequence, size_t byte_sequence_count)
{
	if(Env::getOperationLoudness() >= Env::Debug) {
		Env::logMsg(Env::OperationMsg, Env::Debug, "Waiting for 0x%02X bytes -", byte_sequence_count);
		for(int i = 0; i < byte_sequence_count; i++) {
			Env::logMsg(Env::OperationMsg, Env::Debug, "byte_sequence[%d] = 0x%02X", i, byte_sequence[i]);
		}
	}

	uint8_t byte_read;
	size_t current_sequence_position = 0;
  
	while(current_sequence_position < byte_sequence_count) {
		streamRead(&byte_read, 1);
		Env::logMsg(Env::OperationMsg, Env::Debug, "Read Byte: 0x%02X", byte_read);

		if(byte_read == byte_sequence[current_sequence_position]) {
			current_sequence_position++;
		} else {
			current_sequence_position = 0;
		}
	}
}

void SComm::streamWrite(const void* buffer, size_t write_size)
{
	if(Env::getOperationLoudness() >= Env::Debug) {
		Env::logMsg(Env::OperationMsg, Env::Debug, "Writing 0x%02X bytes -", write_size);
		for(int i = 0; i < write_size; i++) {	
			Env::logMsg(Env::OperationMsg, Env::Debug, "buffer[%d] = 0x%02X", i, ((uint8_t*)buffer)[i]);
		}
	}

	if(xCommFd == -1) {
		initPort();
	}

	size_t bytes_written = write(xCommFd, buffer, write_size);
	if(bytes_written == -1) {
		Env::errorMsg("Unable to write to stream: %s", strerror(errno));
		throw 9;
	} else if(write_size != bytes_written) {
		Env::errorMsg("Bytes written to stream: 0x%02X do not match what was requested: 0x%02X", bytes_written, write_size);
		throw 9;
	}
}

// Setup the device into the right mode and open the file device and return it if it worked.  Needs to be closed later.
int SComm::portSetup(struct port_arguments_t *args) 
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "Port setup device:%s baud:%d bits:%d parity:0x%02X stops:%d",
		args->device,
		args->baud,
	      	args->bits,
	       	args->parity,
	       	args->stops);	
	
	if(xCommFd != -1) {
		close(xCommFd);
		xCommFd = -1;
	}

	//xCommFd = open(args->device,O_RDWR | O_NOCTTY | O_NDELAY);
	xCommFd = open(args->device,O_RDWR | O_NOCTTY);

	if(xCommFd != -1) {
		//fcntl(xCommFd, F_SETFL, FNDELAY);

		struct termios options;
		tcgetattr(xCommFd, &options);

		switch(args->baud) {
		case 38400:
			cfsetispeed(&options, B38400);
			cfsetospeed(&options, B38400);
			break;
		case 19200:
			cfsetispeed(&options, B19200);
			cfsetospeed(&options, B19200);
			break;
		case 9600:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;
		case 4800:
			cfsetispeed(&options, B4800);
			cfsetospeed(&options, B4800);
			break;
		case 2400:
			cfsetispeed(&options, B2400);
			cfsetospeed(&options, B2400);
			break;
		default:
			close(xCommFd);
			Env::errorMsg("Unsupported baud rate specified. Sorry.  You can add it if you like.");
			throw 9;
		}

		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~CSIZE;

		switch(args->bits) {
		case 8:
			options.c_cflag |= CS8;
			break;
		case 7:
			options.c_cflag |= CS7;
			break;
		case 5:
			options.c_cflag |= CS5;
			break;
		default:
			close(xCommFd);
			Env::errorMsg("Unsupported number of bits specified. Sorry.  You can add it if you like.");
			throw 9;
 		}

		switch(args->parity) {
		case 'N':
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~(INPCK | ISTRIP);
			break;
		case 'E':
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'O':
			options.c_cflag |= PARENB;
			options.c_cflag |= PARODD;
			options.c_iflag |= (INPCK | ISTRIP);
			break;
		default:
			close(xCommFd);
			Env::errorMsg("Unsupported parity specified. Sorry.  You can add it if you like.");
			throw 9;
		}

		switch(args->stops) {
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			close(xCommFd);
			Env::errorMsg("Unsupported stops specified. Sorry.  You can add it if you like.");
			throw 9;
		}

		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		options.c_oflag &= ~OPOST;

		if(-1 == tcsetattr(xCommFd, TCSANOW, &options)) {
			Env::errorMsg("Unable to run tcsetattr: %s", strerror(errno));
			throw 9;
		}
	}

	return xCommFd;
}
