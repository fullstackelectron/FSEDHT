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

#ifndef FSEDHT11_H_
#define FSEDHT11_H_

#include <Arduino.h>

typedef struct {
	int errorNum;
	String errorMsg;
} FSEDHT_error_t;

class FSEDHT;

typedef std::function<void(float humid, float temp)> afterRead_CB;
typedef std::function<void(FSEDHT *dht11)> afterReadPt_CB;
typedef std::function<void(FSEDHT_error_t _error)> onError_CB;

typedef enum {
    NO_RESPONSE_SIGNAL,
} SENSOR_ERROR_t;


class FSEDHT{
public:
	FSEDHT();
	FSEDHT(uint8_t pin);
	void setPin(uint8_t pin);
	void afterRead(afterRead_CB callback);
	void afterRead(afterReadPt_CB callback);
	void onError(onError_CB error_callback);
	void read();
	float getHumidity();
	float toCelsius();
	float toFahrenheit();
	float toKelvin();
protected:
	volatile int8_t _result;
	volatile uint8_t _data[5];
private:
	const int DHTLIB_OK              = 0;
	const int DHTLIB_ERROR_CHECKSUM  = -1;
	const int DHTLIB_ERROR_TIMEOUT   = -2;
	const int DHTLIB_INVALID_VALUE   = -999;
	afterReadPt_CB _afterReadPt;
	afterRead_CB _afterRead;
	onError_CB _errorCallback;

	uint8_t _pin;
	void sendStart();
	volatile uint8_t _counter;
	float temperature=NAN, humidity= NAN;
	uint8_t elapsed_ms(uint8_t start);
	uint8_t elapsed_micro_s(uint8_t start);
	int readData();
	SENSOR_ERROR_t _error;
	void DEBUG_FSE_SENSOR(const char *text);

	const int DHTLIB_TIMEOUT = (F_CPU/40000);
	//const int DHTLIB_TIMEOUT = 1000;
	uint8_t bits[5];  // buffer to receive data
	FSEDHT_error_t error;
};
#endif
