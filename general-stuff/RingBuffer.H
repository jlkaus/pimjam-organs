#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <string.h>
#include <stdint.h>

template <class T>
class RingBuffer {
	
	public:
		inline RingBuffer(uint32_t e_count) : xElemCount(e_count), xHead(0), xTail(0), xFillCount(0) { 
			xBuffer = new T[xElemCount];
			memset(xBuffer, 0, xElemCount * sizeof(T));
		}

		inline ~RingBuffer() { delete xBuffer; };

		inline int read(T* buffer, size_t buffer_size) {
			if(empty()) { return 0; }

			uint32_t elem_to_read = buffer_size;
			if(buffer_size > getFillCount()) {
				elem_to_read = getFillCount();
			}
			if(elem_to_read > (xElemCount - xHead)) {
				memcpy(buffer, &xBuffer[xHead], (xElemCount - xHead) * sizeof(T));
				memcpy(buffer + (xElemCount - xHead), &xBuffer[0], (elem_to_read - (xElemCount - xHead)) * sizeof(T));
			} else {
				memcpy(buffer, &xBuffer[xHead], elem_to_read * sizeof(T));
			}
			xHead += elem_to_read;
			if(xHead > xElemCount) {
				xHead = xHead % xElemCount;
			}
			xFillCount -= elem_to_read;
			return elem_to_read;
		}

		inline int write(T* buffer, size_t buffer_size) {
			if(getFillCount() >= xElemCount) { return 0; }

			uint32_t elem_to_write = buffer_size;
			if(buffer_size > xElemCount - getFillCount()) {
				elem_to_write = xElemCount - getFillCount();
			}
			if(elem_to_write > (xElemCount - xTail)) {
				memcpy(&xBuffer[xTail], buffer, (xElemCount - xTail) * sizeof(T));
				memcpy(&xBuffer[0], buffer + (xElemCount - xTail), (elem_to_write - (xElemCount - xTail)) * sizeof(T));
			} else {
				memcpy(&xBuffer[xTail], buffer, elem_to_write * sizeof(T));
			}
			xTail += elem_to_write;
			if(xTail > xElemCount) {
				xTail = xTail % xElemCount;
			}
			xFillCount += elem_to_write;
			return elem_to_write;
		}

		inline bool empty() const { return xFillCount == 0; }
		inline uint32_t getFillCount() const { return xFillCount; }

		inline uint32_t getHead() const { return xHead; }
		inline uint32_t getTail() const { return xTail; }
		inline const T* getBuffer() const { return xBuffer; }
		
	private:
		uint32_t xHead;
		uint32_t xTail;
		uint32_t xFillCount;
		uint32_t xElemCount;
		T* xBuffer;

};

#endif
