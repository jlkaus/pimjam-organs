#include "SComm.H"
#include "OrganSupport.H"

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

  this_scom->syncStream();
  
  while(this_scom->xProcessEvents) {
    this_scom->processEvent();
  }

  close(this_scom->xCommFd);
  this_scom->xCommFd = -1;
}

void SComm::syncStream()
{
  // Issue a soft reset to initialize the stream
  softReset();

  exit(1);

  // Wait for the boot message

/*
  bool synced = false;

  // 0xFF is the start of an event
  //   Once this byte has been read, the rest of the event will
  //     be read and thrown away.  At that point the next thing
  //     in the stream must be a new event
  while(!synced) {

    int count = 0;
    ioctl(xCommFd, FIONREAD, &count);

    // If there are no bytes in the stream assume it is already synced
    if(count == 0) {
	return;
    }

    // Check for 3 bytes in the buffer
    //   The start of an event contains a header byte (0xFF) a byte which indicates the message type
    //   and a byte that indicates the message size
    if(count >= 3) {
      unsigned char sync_byte;
      read(xCommFd, &sync_byte, 1);

      if(sync_byte == 0xFF) {
        unsigned char msg_type;
        read(xCommFd, &msg_type, 1);

	unsigned char msg_size;
        read(xCommFd, &msg_size, 1);
	
	// Once the message size is known, read that many bytes from the buffer + 2 for the message footer (0xFD 0xFC)
	//   The next byte in the stream must be a new event
	do {
          ioctl(xCommFd, FIONREAD, &count);
        } while(count < (msg_size + 2));
        
        for(int i = 0; i < msg_size + 2; ++i) {
          read(xCommFd, &sync_byte, 1);
	}

	synced = true;
      }
    }
  }
*/
}

void SComm::processEvent()
{
  int count = 0;

  do {
    ioctl(xCommFd, FIONREAD, &count);
  } while(count < sizeof(midi_event_t));

  struct midi_event_t midi_event;

  read(xCommFd, &midi_event, sizeof(midi_event_t));

  printf("%02X %02X %02X\n", midi_event.op_channel, midi_event.arg1, midi_event.arg2);
}

void SComm::softReset()
{
	// Command mode will screw with the event handling.
	//   Exit out of event processing if events were running
	bool events_were_processing = false;
	if(xProcessEvents) {
		events_were_processing = true;
		stopEvents();
	}

	OrganSupport::logMsg(OrganSupport::Info, "SComm::softReset initiating soft reset\n");

	// Issue the commands to the organ firmware to soft reset
	//   this is done by entering "Command ode" and immediately exiting
	
	// Enter command mode command 0xB00000
	struct cmd_mode_command_t enter_command_mode = { 0xB0, 0x00, 0x00 };

	// Exit command mode commmand 0xBF0000
	struct cmd_mode_command_t exit_command_mode = { 0xBF, 0x00, 0x00 };

        sendAndProcessCommandModeCommand(enter_command_mode);
        sendAndProcessCommandModeCommand(exit_command_mode);

	// Process the boot message
	receiveMessage(0xF0, &handleBootMsg);
}

void SComm::handleBootMsg(SComm* this_scom, void* reponse_data, size_t reponse_data_size)
{
	struct sub_msg_header_t* msg_header = (sub_msg_header_t*)reponse_data;

	for(int i = 0; i < 2; ++i) {
		char boot_msg_format[30];
		snprintf(boot_msg_format, sizeof(boot_msg_format), "Boot Msg: %%.%ds\n", msg_header->msg_size);

		OrganSupport::logMsg(OrganSupport::Info, boot_msg_format, (char*)(msg_header) + sizeof(sub_msg_header_t));

		msg_header = (sub_msg_header_t*)(((char*)msg_header) + sizeof(sub_msg_header_t) + msg_header->msg_size + 1);
	}
	exit(1);
}

void SComm::sendAndProcessCommandModeCommand(const cmd_mode_command_t& cmd, MsgResponseHandler* response_handler)
{
  // If events are currently being processed, the command mode command will screw that up
  //   So stop processing events prior to running the command
  if(xProcessEvents) {
    stopEvents();
  }

  // Send the command
  streamWrite(&cmd, sizeof(cmd_mode_command_t));

  // Process the reponse
  receiveMessage(0xF3, response_handler);
}

void SComm::receiveMessage(uint8_t msg_type, MsgResponseHandler* response_handler)
{
	OrganSupport::logMsg(OrganSupport::Debug, "SComm::receiveMessage - type 0x%02X\n", msg_type);

	// Wait for the message response
	// Message response format FFTTSSSSMM...MMFDFC
	//   Where 
	//     FF is the message header
	//     TT is the message subtype
	//     SSSS is the message size (includes everything from the subtype to the end of the message data)
	//     MM...MM is the message data
	//     FDFC is the message footer
	//   There is no guarentee that the stream is synced.  So there might be other data in stream that is not
	//     the response.  Throw that data away and wait for the reponse of the specified type.

	uint8_t response_header[2] = {0xFF, msg_type};
	streamWaitForByteSequence(response_header, 2);
  
	// Retrieve the message response data
	uint16_t msg_size;
	streamRead(&msg_size, sizeof(uint16_t));
	msg_size = ntohs(msg_size);
	if(msg_type == 0xF3) {
		msg_size -= 3; // Subtract off the subtype and message length from the message size
	}

	printf("MSGSIZE: %04X\n", msg_size);
  
	uint8_t* response_buffer = new uint8_t[msg_size];
	streamRead(response_buffer, msg_size);

	printf("RESPONSE\n");
	for(int i = 0; i < msg_size; ++i) {
		printf("%02X", response_buffer[i]);
	}
	printf("\n");

	// Retrieve the message footer
	streamReadExpectedByte(0xFD);
	streamReadExpectedByte(0xFC);

	// If a reponse handler is defined, call it
	if(response_handler) {
		response_handler(this, response_buffer, msg_size);
	}

	delete [] response_buffer;
}


void SComm::streamRead(void* buffer, size_t read_size)
{
	OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamRead: 0x%02X bytes\n", read_size);
	streamWaitForBytes(read_size);

	ssize_t size_read = read(xCommFd, buffer, read_size);

	if(size_read == -1) {
		OrganSupport::errorMsg("Unable to read from stream: %s\n", strerror(errno));
	} else  if(read_size != size_read) {
		OrganSupport::errorMsg("Bytes read from stream: %d do not match what was requested: %d\n", size_read, read_size);
	}
}

void SComm::streamReadExpectedByte(unsigned char expected_byte)
{
	OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamReadExpectedByte: 0x%02X\n", expected_byte);
	
	unsigned char byte_read = 0;
	streamRead(&byte_read, 1);

	if(byte_read != expected_byte) {
		OrganSupport::errorMsg("Fatal error receiving data from stream.  Expected byte: %02X - Recieved byte: %02X\n", expected_byte, byte_read);
	}
}

void SComm::streamWaitForByteSequence(const uint8_t* byte_sequence, size_t byte_sequence_count)
{
	if(OrganSupport::getVerbosityLevel() >= OrganSupport::Debug) {
		OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamWaitForByteSequence 0x%02X bytes - ", byte_sequence_count);
		for(int i = 0; i < byte_sequence_count; i++) {
			OrganSupport::logMsg(OrganSupport::Debug, "|%02X|", byte_sequence[i]);
		}
		OrganSupport::logMsg(OrganSupport::Debug, "\n");
	}

	uint8_t* bytes_read = new uint8_t[byte_sequence_count];
	bool sequence_found = false;
  
	while(!sequence_found) {
		streamRead(bytes_read, byte_sequence_count);

		sequence_found = true;
		OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamWaitForByteSequence Bytes Read: ");
		for(int i = 0; i < byte_sequence_count; ++i) {
			OrganSupport::logMsg(OrganSupport::Debug, "%02X", bytes_read[i]);
			if(byte_sequence[i] != bytes_read[i]) {
				sequence_found = false;
				break;
			}
		}
		OrganSupport::logMsg(OrganSupport::Debug, "\n");
	}

	delete [] bytes_read;
}

void SComm::streamWrite(const void* buffer, size_t write_size)
{
	if(OrganSupport::getVerbosityLevel() >= OrganSupport::Debug) {
		OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamWrite 0x%02X bytes - ", write_size);
		for(int i = 0; i < write_size; i++) {	
			OrganSupport::logMsg(OrganSupport::Debug, "|%02X|", ((uint8_t*)buffer)[i]);
		}
		OrganSupport::logMsg(OrganSupport::Debug, "\n");
	}

	if(xCommFd == -1) {
		initPort();
	}

	size_t bytes_written = write(xCommFd, buffer, write_size);
	if(bytes_written == -1) {
		OrganSupport::errorMsg("Unable to write to stream: %s\n", strerror(errno));
	} else if(write_size != bytes_written) {
		OrganSupport::errorMsg("Bytes written to stream: %d do not match what was requested: %d\n", bytes_written, write_size);
	}
}

void SComm::streamWaitForBytes(size_t count)
{
	OrganSupport::logMsg(OrganSupport::Debug, "SComm::streamWaitForBytes: 0x%02X\n", count);

	if(xCommFd == -1) {
		initPort();
	}

	size_t size_available = 0;
	do {
		ioctl(xCommFd, FIONREAD, &size_available); 
	} while(size_available < count);
}


// Setup the device into the right mode and open the file device and return it if it worked.  Needs to be closed later.
int SComm::portSetup(struct port_arguments_t *args) 
{
	OrganSupport::logMsg(OrganSupport::Info, "SComm::portSetup device:%s baud:%d bits:%d parity:0x%02X stops:%d\n", 
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
			OrganSupport::errorMsg("ERROR: Unsupported baud rate specified. Sorry.  You can add it if you like.\n");
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
			OrganSupport::errorMsg("ERROR: Unsupported number of bits specified. Sorry.  You can add it if you like.\n");
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
			OrganSupport::errorMsg("ERROR: Unsupported parity specified. Sorry.  You can add it if you like.\n");
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
			OrganSupport::errorMsg("ERROR: Unsupported stops specified. Sorry.  You can add it if you like.\n");
		}

		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		options.c_oflag &= ~OPOST;

		tcsetattr(xCommFd, TCSANOW, &options);
	}

	return xCommFd;
}

int main(int argc, char* argv[]) {

	printf("!!!%d!!!\n", OrganSupport::create(OrganSupport::Info));
  SComm test_comm(NULL);

  test_comm.startEvents();

  while(1);

  return 0;
}
