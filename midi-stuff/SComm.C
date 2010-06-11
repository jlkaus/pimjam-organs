#include "Env.H"
#include "SComm.H"
//#include "EventHandler.H"
//#include "Event.H"

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
#include <sys/select.h>
#include <iomanip>

SComm::~SComm() 
{
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
		this_scom->receiveMessage(0xFE, &handleMidiEventMsg);
	}

	close(this_scom->xCommFd);
	this_scom->xCommFd = -1;
}

void SComm::softReset()
{
	Env::logMsg(Env::OperationMsg, Env::Info, "SComm - Initiating soft reset");

	// Issue the commands to the organ firmware to soft reset
	//   this is done by entering "Command mode" and immediately exiting
	
	// Enter command mode command 0xB00000
	struct cmd_mode_command_t enter_command_mode = { 0xB0, 0x00, 0x00 };

	// Exit command mode commmand 0xBF0000
	struct cmd_mode_command_t exit_command_mode = { 0xBF, 0x00, 0x00 };

        sendAndProcessCommandModeCommand(enter_command_mode);
        sendAndProcessCommandModeCommand(exit_command_mode);

	// Process the boot message
	receiveMessage(0xF0, &handleBootMsg);
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
		Env::logMsg(Env::OperationMsg, Env::Info, boot_msg_format, (char*)(msg_header) + sizeof(sub_msg_header_t));
		msg_header = (sub_msg_header_t*)(((char*)msg_header) + sizeof(sub_msg_header_t) + msg_header->msg_size + 1);
	}

	// Log the firmware version
	uint8_t firmware_version_size = 1;

	if(msg_header->msg_size != firmware_version_size) {
		Env::errorMsg("Boot message format error, firmware version length mismatch.  Expected: 0x%02X Received: 0x%02X", firmware_version_size, msg_header->msg_size);
		throw 9;
	}

	Env::logMsg(Env::OperationMsg, Env::Info, "Firmware Version: 0x%02X", *((uint8_t*)(msg_header) + sizeof(sub_msg_header_t)));
}

void SComm::handleMidiEventMsg(SComm* this_scom, void* response_data, size_t response_data_size)
{
	struct midi_event_t* midi_event = (midi_event_t*)response_data;
	Env::logMsg(Env::OperationMsg, Env::Info, "MIDI Event: 0x%02X 0x%02X 0x%02X", midi_event->op_channel, midi_event->arg1, midi_event->arg2);
}

void SComm::sendAndProcessCommandModeCommand(const cmd_mode_command_t& cmd, MsgResponseHandler* response_handler)
{
	// Send the command
	streamWrite(&cmd, sizeof(cmd_mode_command_t));

	// Process the response
	receiveMessage(0xF3, response_handler);
}

void SComm::receiveMessage(uint8_t msg_type, MsgResponseHandler* response_handler)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::receiveMessage - type: 0x%02X", msg_type);

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

	uint8_t response_header[2] = {0xFF, msg_type};
	streamWaitForByteSequence(response_header, 2);
  
	// Retrieve the message response data
	uint16_t msg_size;
	streamRead(&msg_size, sizeof(uint16_t));
	msg_size = ntohs(msg_size);
	/*if(msg_type == 0xF3) {
		msg_size -= 3; // Subtract off the subtype and message length from the message size
	}*/
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::recieveMessage - message size: 0x%02X", msg_size);
  
	uint8_t* response_buffer = new uint8_t[msg_size];
	streamRead(response_buffer, msg_size);

	if(Env::getOperationLoudness() >= Env::Debug) {
		for(int i = 0; i < msg_size; ++i) {
			Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::receiveMessage - response_buffer[%d] = 0x%02X", i, response_buffer[i]);
		}
	}

	// Retrieve the message footer
	streamReadExpectedByte(0xFD);
	streamReadExpectedByte(0xFC);

	// If a response handler is defined, call it
	if(response_handler) {
		response_handler(this, response_buffer, msg_size);
	}

	delete [] response_buffer;
}


void SComm::streamRead(void* buffer, size_t read_size)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamRead: 0x%02X bytes", read_size);
	streamWaitForBytes(read_size);

	ssize_t size_read = read(xCommFd, buffer, read_size);

	if(size_read == -1) {
		Env::errorMsg("Unable to read from stream: %s", strerror(errno));
		throw 9;
	} else  if(read_size != size_read) {
		Env::errorMsg("Bytes read from stream: 0x%02X do not match what was requested: 0x%02X", size_read, read_size);
		throw 9;
	}
}

void SComm::streamReadExpectedByte(unsigned char expected_byte)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamReadExpectedByte: 0x%02X", expected_byte);
	
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
		Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWaitForByteSequence 0x%02X bytes -", byte_sequence_count);
		for(int i = 0; i < byte_sequence_count; i++) {
			Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWaitForByteSquence - byte_sequence[%d] = 0x%02X", i, byte_sequence[i]);
		}
	}

	uint8_t* bytes_read = new uint8_t[byte_sequence_count];
	bool sequence_found = false;
  
	while(!sequence_found) {
		streamRead(bytes_read, byte_sequence_count);

		sequence_found = true;
		for(int i = 0; i < byte_sequence_count; ++i) {
			Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWaitForByteSequence - Read Byte: 0x%02X", bytes_read[i]);
			if(byte_sequence[i] != bytes_read[i]) {
				sequence_found = false;
				break;
			}
		}
	}

	delete [] bytes_read;
}

void SComm::streamWrite(const void* buffer, size_t write_size)
{
	if(Env::getOperationLoudness() >= Env::Debug) {
		Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWrite 0x%02X bytes -", write_size);
		for(int i = 0; i < write_size; i++) {	
			Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWrite - buffer[%d] = 0x%02X", i, ((uint8_t*)buffer)[i]);
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

void SComm::streamWaitForBytes(size_t count)
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::streamWaitForBytes: 0x%02X", count);

	if(xCommFd == -1) {
		initPort();
	}

	size_t size_available = 0;
	do {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(xCommFd, &rfds);
		if(-1 == select(xCommFd + 1, &rfds, NULL, NULL, NULL)) {
			Env::errorMsg("Error waiting for stream input: %s", strerror(errno));
		}
		ioctl(xCommFd, FIONREAD, &size_available); 
	} while(size_available < count);
}


// Setup the device into the right mode and open the file device and return it if it worked.  Needs to be closed later.
int SComm::portSetup(struct port_arguments_t *args) 
{
	Env::logMsg(Env::OperationMsg, Env::Debug, "SComm::portSetup device:%s baud:%d bits:%d parity:0x%02X stops:%d",
		args->device,
		args->baud,
	      	args->bits,
	       	args->parity,
	       	args->stops);	
	
	if(xCommFd != -1) {
		close(xCommFd);
		xCommFd = -1;
	}

	xCommFd = open(args->device,O_RDWR | O_NOCTTY | O_NDELAY);

	if(xCommFd != -1) {
		fcntl(xCommFd, F_SETFL, FNDELAY);

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

		tcsetattr(xCommFd, TCSANOW, &options);
	}

	return xCommFd;
}

int main(int argc, char* argv[]) {

  SComm test_comm(NULL);

  test_comm.startEvents();

  while(1) {
	sleep(1);	  
  };

  return 0;
}
