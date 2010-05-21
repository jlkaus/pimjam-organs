#include "SComm.H"

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

void SComm::sendAndProcessCommandModeCommand(const cmd_mode_command_t& cmd, CommandResponseHandler* response_handler)
{

  printf("%02X %02X %02X\n", cmd.opcode, cmd.arg1, cmd.arg2);

  // Send the command
  streamWrite(&cmd, sizeof(cmd_mode_command_t));

  // Wait for the command response
  //   There is no guarentee that the stream is synced.  So there might be other data in stream that is not
  //     the command response.  Throw that data away and wait for a command respons (message header 0xFFF3)
  char command_response_header[2] = {0xFF, 0xF3};
  streamWaitForByteSequence(command_response_header, 2);

  // Retrieve the message response data
  uint16_t msg_size;
  streamRead(&msg_size, sizeof(uint16_t));
  msg_size = ntohs(msg_size);

  // Message format FFF3SSSSMM...MMFDFC
  //   Where 
  //     FF is the message header
  //     F3 is the message subtype (Command response in this case)
  //     SSSS is the message size (includes everything from the subtype to the end of the message data)
  //     MM...MM is the message data
  //     FDFC is the message footer

  printf("MSGSIZE: %04X\n", msg_size);
  msg_size -= 3; // Subtract off the subtype and message length from the message size
  
  unsigned char* response_buffer = new unsigned char[msg_size];
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
  printf("STREAM READ: %04X\n", read_size);

  streamWaitForBytes(read_size);

  ssize_t size_read = read(xCommFd, buffer, read_size);

  if(size_read == -1) {
    fprintf(stderr, "Unable to read from stream: %s\n", strerror(errno));
    exit(1);
  } else  if(read_size != size_read) {
    fprintf(stderr, "Bytes read from stream: %d do not match what was requested: %d\n", size_read, read_size);
    exit(1);
  }
}

void SComm::streamReadExpectedByte(unsigned char expected_byte)
{
  unsigned char byte_read = 0;
  streamRead(&byte_read, 1);

  if(byte_read != expected_byte) {
    fprintf(stderr, "Fatal error receiving data from stream.  Expected byte: %02X - Recieved byte: %02X\n", expected_byte, byte_read);
    exit(1);
  }
}

void SComm::streamWaitForByteSequence(const char* byte_sequence, size_t byte_sequence_count)
{
  char* bytes_read = new char[byte_sequence_count];
  bool sequence_found = false;
  
  while(!sequence_found) {
    streamRead(bytes_read, byte_sequence_count);

    sequence_found = true;
    for(int i = 0; i < byte_sequence_count; ++i) {
      if(byte_sequence[i] != bytes_read[i]) {
        sequence_found = false;
        break;
      }
    }
  }
}

void SComm::streamWrite(const void* buffer, size_t write_size)
{
  if(xCommFd == -1) {
    initPort();
  }

  printf("Write data %02X\n", write_size);
  for(int i = 0; i < write_size; i++) {
    printf("|%02X|", ((unsigned char*)buffer)[i]);
  }
  printf("\n");

  size_t bytes_written = write(xCommFd, buffer, write_size);
  if(bytes_written == -1) {
    fprintf(stderr, "Unable to write to stream: %s\n", strerror(errno));
    exit(1);
  } else if(write_size != bytes_written) {
    fprintf(stderr, "Bytes written to stream: %d do not match what was requested: %d\n", bytes_written, write_size);
    exit(1);
  }
}

void SComm::streamWaitForBytes(size_t count)
{
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
      fprintf(stderr, "ERROR: Unsupported baud rate specified. Sorry.  You can add it if you like.\n");
      close(xCommFd);
      exit(-1);
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
      fprintf(stderr, "ERROR: Unsupported number of bits specified. Sorry.  You can add it if you like.\n");
      close(xCommFd);
      exit(-1);
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
      fprintf(stderr, "ERROR: Unsupported parity specified. Sorry.  You can add it if you like.\n");
      close(xCommFd);
      exit(-1);
    }

    switch(args->stops) {
    case 1:
      options.c_cflag &= ~CSTOPB;
      break;
    case 2:
      options.c_cflag |= CSTOPB;
      break;
    default:
      fprintf(stderr, "ERROR: Unsupported stops specified. Sorry.  You can add it if you like.\n");
      close(xCommFd);
      exit(-1);
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

  while(1);

  return 0;
}
