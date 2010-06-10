#include "Env.H"
#include "SComm.H"
#include "EventHandler.H"
#include "Event.H"

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
	Env::msg(Env::OperationMsg,Env::Info,Env::EventGeneratorIndent) << "SComm - Initiating soft reset" << std::endl;

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
		Env::msg(Env::OperationMsg,Env::Info,Env::EventGeneratorIndent) << "Boot Msg: " << std::setprecision(msg_header->msg_size) << (char*)(msg_header) + sizeof(sub_msg_header_t) << std::endl;
		msg_header = (sub_msg_header_t*)(((char*)msg_header) + sizeof(sub_msg_header_t) + msg_header->msg_size + 1);
	}

	// Log the firmware version
	uint8_t firmware_version_size = 1;

	if(msg_header->msg_size != firmware_version_size) {
		Env::err() << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "Boot message format error, firmware version length mismatch.  Expected: " << (int)firmware_version_size << "Recieved: " << (int)msg_header->msg_size << std::endl;
		throw 9;
	}

	Env::msg(Env::OperationMsg,Env::Info,Env::EventGeneratorIndent) << "Firmware Version: " << std::hex << std::showbase << (int)*((uint8_t*)(msg_header) + sizeof(sub_msg_header_t)) << std::endl;
}

void SComm::handleMidiEventMsg(SComm* this_scom, void* response_data, size_t response_data_size)
{
	struct midi_event_t* midi_event = (midi_event_t*)response_data;
	Env::msg(Env::OperationMsg,Env::Info,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "MIDI Event: " <<  (int)midi_event->op_channel << " " << (int)midi_event->arg1 << " " << (int)midi_event->arg2 << std::endl;
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
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::receiveMessage - type " << (int)msg_type << std::endl;

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
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::recieveMessage - message size: " << (int)msg_size << std::endl;
  
	uint8_t* response_buffer = new uint8_t[msg_size];
	streamRead(response_buffer, msg_size);

	if(Env::getOperationLoudness() >= Env::Debug) {
		for(int i = 0; i < msg_size; ++i) {
			Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::dec | std::ios_base::showbase) << "SComm::receiveMessage - response_buffer[" << i << "] = " << std::hex << (int)response_buffer[i] << std::endl;
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
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamRead: " << (int)read_size << " bytes" << std::endl;
	streamWaitForBytes(read_size);

	ssize_t size_read = read(xCommFd, buffer, read_size);

	if(size_read == -1) {
		Env::err() << "Unable to read from stream: " << strerror(errno) << std::endl;
		throw 9;
	} else  if(read_size != size_read) {
		Env::err() << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "Bytes read from stream: " << (int)size_read << " do not match what was requested: " << (int)read_size << std::endl;
		throw 9;
	}
}

void SComm::streamReadExpectedByte(unsigned char expected_byte)
{
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamReadExpectedByte: " << (int)expected_byte << std::endl;
	
	unsigned char byte_read = 0;
	streamRead(&byte_read, 1);

	if(byte_read != expected_byte) {
		Env::err() << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "Fatal error receiving data from stream.  Expected byte: " << (int)expected_byte << " - Recieved byte: " << (int)byte_read << std::endl;
		throw 9;
	}
}

void SComm::streamWaitForByteSequence(const uint8_t* byte_sequence, size_t byte_sequence_count)
{
	if(Env::getOperationLoudness() >= Env::Debug) {
		Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamWaitForByteSequence " << (int)byte_sequence_count << " bytes - " << std::endl;
		for(int i = 0; i < byte_sequence_count; i++) {
			Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::dec | std::ios_base::showbase) << "SComm::streamWaitForByteSquence - byte_sequence[" << i << "] = " << std::hex << (int)byte_sequence[i] << std::endl;
		}
	}

	uint8_t* bytes_read = new uint8_t[byte_sequence_count];
	bool sequence_found = false;
  
	while(!sequence_found) {
		streamRead(bytes_read, byte_sequence_count);

		sequence_found = true;
		for(int i = 0; i < byte_sequence_count; ++i) {
			Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamWaitForByteSequence - Read Byte: " << (int)bytes_read[i] << std::endl;
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
		Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamWrite " << (int)write_size << " bytes - " << std::endl;
		for(int i = 0; i < write_size; i++) {	
			Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamWrite - buffer[" << i << "] = " << (int)((uint8_t*)buffer)[i] << std::endl;
		}
	}

	if(xCommFd == -1) {
		initPort();
	}

	size_t bytes_written = write(xCommFd, buffer, write_size);
	if(bytes_written == -1) {
		Env::err() << "Unable to write to stream: " << strerror(errno) << std::endl;
		throw 9;
	} else if(write_size != bytes_written) {
		Env::err() << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "Bytes written to stream: " << (int)bytes_written << " do not match what was requested: " << (int)write_size << std::endl;
		throw 9;
	}
}

void SComm::streamWaitForBytes(size_t count)
{
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) << std::setiosflags(std::ios_base::hex | std::ios_base::showbase) << "SComm::streamWaitForBytes: " << (int)count << std::endl;

	if(xCommFd == -1) {
		initPort();
	}

	size_t size_available = 0;
	do {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(xCommFd, &rfds);
		if(-1 == select(xCommFd + 1, &rfds, NULL, NULL, NULL)) {
			Env::err() << "Error waiting for stream input: " << strerror(errno) << std::endl;
		}
		ioctl(xCommFd, FIONREAD, &size_available); 
	} while(size_available < count);
}


// Setup the device into the right mode and open the file device and return it if it worked.  Needs to be closed later.
int SComm::portSetup(struct port_arguments_t *args) 
{
	Env::msg(Env::OperationMsg,Env::Debug,Env::EventGeneratorIndent) 
		<< "SComm::portSetup device:" << args->device
		<< " baud:" << args->baud
	      	<< " bits:" << args->bits
	       	<< " parity:" << std::hex << std::showbase << args->parity
	       	<< " stops:" << std::dec << args->stops
	       	<< std::endl;	
	
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
			Env::err() << "ERROR: Unsupported baud rate specified. Sorry.  You can add it if you like." << std::endl;
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
			Env::err() << "ERROR: Unsupported number of bits specified. Sorry.  You can add it if you like." << std::endl;
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
			Env::err() << "ERROR: Unsupported parity specified. Sorry.  You can add it if you like." << std::endl;
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
			Env::err() << "ERROR: Unsupported stops specified. Sorry.  You can add it if you like." << std::endl;
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
