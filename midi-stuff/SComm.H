#ifndef _SCOMM_H
#define _SCOMM_H

#include "EventGenerator.H"

#include <unistd.h>
#include <stdint.h>

class EventHandler;
class SComm;

class SComm : public EventGenerator {

public:
	SComm(EventHandler* event_handler) : EventGenerator(event_handler, &eventLoop),  xCommFd(-1) { }
	~SComm();

private:

	struct port_arguments_t {
		const char* device;
		int baud;
		int bits;
		char parity;
		int stops;
	};

	struct midi_event_t {
		unsigned char message_header;
		unsigned char message_type;
		unsigned char message_size;
		unsigned char op_channel;
		unsigned char arg1;
		unsigned char arg2;
		unsigned char footer1;
		unsigned char footer2;
	};

	struct cmd_mode_command_t {
		uint8_t opcode;
		uint8_t arg1;
		uint8_t arg2;
	};

	struct sub_msg_header_t {
		uint8_t msg_number;
		uint8_t msg_size;
	};

	typedef void (MsgResponseHandler)(SComm*, void*, size_t);

	void syncStream();
	void processEvent();
	int portSetup(struct port_arguments_t* args);
        void sendAndProcessCommandModeCommand(const cmd_mode_command_t& cmd, MsgResponseHandler* response_handler = NULL);
	void receiveMessage(uint8_t msg_type, MsgResponseHandler* response_handler = NULL);

	void softReset();
        static void handleBootMsg(SComm* this_scom, void* reponse_data, size_t reponse_data_size);

	inline void initPort() { struct port_arguments_t port_args = {"/dev/ttyUSB0", 38400, 8, 'N', 1}; portSetup(&port_args); }
	
	void streamWaitForBytes(size_t count);
	void streamRead(void* buffer, size_t read_size);
	void streamWrite(const void* buffer, size_t write_size);
	void streamReadExpectedByte(unsigned char expected_byte);
	void streamWaitForByteSequence(const uint8_t* byte_sequence, size_t byte_sequence_size);

	static void* eventLoop(void *arg);

	int xCommFd;
};

#endif
