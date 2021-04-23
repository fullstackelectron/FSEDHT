/*

Copyright 2018 Bert Melis

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "FSEDHT.h"  // NOLINT

FSEDHT::FSEDHT(uint8_t pin){
	_pin = pin;
}
void FSEDHT::sendStart() {
	// low for 20ms
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	//keep it low for 20 ms
	delay(18);
	pinMode(_pin, INPUT); // switch the bus for reading data. The pull up resistor will make it HIGH
}

int FSEDHT::readData() {
    uint8_t mask = 128;
    uint8_t idx = 0;
    // EMPTY BUFFER
	for (uint8_t i = 0; i < 5; i++) bits[i] = 0;

	char logger[255];
	uint8_t start=micros();
	delayMicroseconds(40);
	uint16_t loopCnt = DHTLIB_TIMEOUT;
	while (digitalRead(_pin) == LOW) { // still waiting for DHT to acknowledge.
		if (--loopCnt == 0) {//This shouldnt take more the 40us but, I will give it a 50
			sprintf(logger, "No response signal received after: %d micro seconds", elapsed_micro_s(start));
			error.errorMsg = logger;
			error.errorNum = DHTLIB_ERROR_TIMEOUT;
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	while (digitalRead(_pin) == HIGH) {
		if (--loopCnt == 0){
			error.errorMsg = "Timeout waiting transmission starts signal";
			error.errorNum = DHTLIB_ERROR_TIMEOUT;
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
    // READ THE OUTPUT - 40 BITS => 5 BYTES
    for (uint8_t i = 40; i != 0; i--)
    {
        loopCnt = DHTLIB_TIMEOUT;
        while(digitalRead(_pin) == LOW)
        {
            if (--loopCnt == 0) {
            	sprintf(logger, "Timeout waiting position %d", i);
            	error.errorMsg = logger;
            	error.errorNum = DHTLIB_ERROR_TIMEOUT;
            	return DHTLIB_ERROR_TIMEOUT;
            }
        }

        uint32_t t = micros();

        loopCnt = DHTLIB_TIMEOUT;
        while(digitalRead(_pin) == HIGH)
        {
            if (--loopCnt == 0){
            	sprintf(logger, "Timeout reading position %d", i);
            	error.errorMsg = logger;
            	error.errorNum = DHTLIB_ERROR_TIMEOUT;
            	return DHTLIB_ERROR_TIMEOUT;
            }
        }

        if ((micros() - t) > 40)
        {
            bits[idx] |= mask;
        }
        mask >>= 1;
        if (mask == 0)
        {
            mask = 128;
            idx++;
        }
    }
	return DHTLIB_OK;

}
uint8_t FSEDHT::elapsed_ms(uint8_t start) {
	return millis() - start;
}

uint8_t FSEDHT::elapsed_micro_s(uint8_t start) {
	return micros() - start;
}

void FSEDHT::read() {
	_data[0] = _data[1] = _data[2] = _data[3] = _data[4] = 0;
	_counter = 0;
	_result = 0;
//	noInterrupts(); // turning interrupts off
	sendStart();
	int result = readData();
//	interrupts();
	if (result != DHTLIB_OK) {
		_errorCallback(error);
		return;
	}

	// TEST CHECKSUM
	uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
	if (bits[4] != sum)
	{
		DEBUG_FSE_SENSOR("Fail checksum");
		error.errorMsg = "Fail checksum";
		error.errorNum = DHTLIB_ERROR_CHECKSUM;
		if (_errorCallback) {
			_errorCallback(error);
		}

		return;
	}

	// CONVERT AND STORE
	humidity = bits[0] + bits[1] * 0.1;
	temperature = (bits[2] & 0x7F) + bits[3] * 0.1;
	if (bits[2] & 0x80)  // negative temperature
	{
		temperature = -temperature;
	}

	if (_afterRead) {
		_afterRead(getHumidity(), toCelsius());
	}

	if (_afterReadPt) {
		_afterReadPt(this);
	}
}

float FSEDHT::toCelsius(){
	return temperature;
}

float FSEDHT::toFahrenheit(){
	return (temperature * 9/5) + 32;
}

float FSEDHT::toKelvin() {
	return temperature + 273.15;
}
float FSEDHT::getHumidity(){
	return humidity;
}

void FSEDHT::DEBUG_FSE_SENSOR(const char *text){
	Serial.print("**FSE: ");
	Serial.println(text);
}

void FSEDHT::afterRead(afterRead_CB callback){
	_afterRead = callback;
}

void FSEDHT::afterRead(afterReadPt_CB callback){
	_afterReadPt = callback;
}

void FSEDHT::onError(onError_CB error_callback) {
	_errorCallback = error_callback;
}


