#include <Arduino.h>
#include <avr/pgmspace.h>

#ifndef _SevenSegShiftReg
#define _SevenSegShiftReg

class SevenSegShiftReg
{
	private:
		int mDataPin;
		int mLatchPin; 
		int mClockPin;
		void byteOutSerial(unsigned int writeByte);
	public:
		SevenSegShiftReg(int dataPin, int latchPin, int clockPin);
		void displayByte(byte digit);
};
#endif		