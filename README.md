**Introduction**

**Technical Details**

As per DHT11 datasheet, the sensor will use a single bus to transmit the data. 40 bits of data will be transmitted and this is represented by 5 values of 8bits.
Each value is as follows:
* 8 bits for the integral value for RH(Relative Humidity)
* 8 bits for the decimal value for RH
* 8 bits for the integral value for Temperature
* 8 bits for the decimal value for Temperature
* 8 bits checksum. Which must be equal to the sum of all previous data

The MCU will request for data in a LOW signal to the pin for 18ms. The image in the oscilloscope will be as below:


![alt text][start_signal]

[start_signal]: ./images/start_signal.bmp "Logo Title Text 2"

Once the start signals ends, the MCU will open the bus for receiving data from the sensor. That is simply setting the pin to INPUT mode. The 5K pull up resistor will keep the pin HIGH until the Sensor starts 
The start of transmission will be driven by the sensor. Which will send a LOW level signal and will keep it for 80us. That is to tell the MCU the request was acknowledged and to be prepared to the transmission.

The transmission is started with a LOW level signal 50us long. This is the length of each bit start signal

The Sensor uses digital HIGH signal length to determine whether the MCU should assume it as a `0` or a `1`. When the signal is 26-28us, that means it is a `0`. 

![alt text][0_value]

When the HIGH signal has a length of ~70us, that means it is a `1`.

![alt text][1_value]

For more details on the technical features of the sensor, refer to its datasheet

https://fusllstackelectron-assets.s3.ca-central-1.amazonaws.com/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf 


[start_signal]: ./images/start_signal.bmp "Start Signal"
[0_value]: ./images/0_value.bmp "Start Signal"
[1_value]: ./images/1_value.bmp "Start Signal"

**Example**

Using callback with humidity and temperature values passed as params

```
/**
 * DHT11 Library
 * Author: Fullstack Electron
 * https://fullstackelectron.com/
 *  Created: April 22, 2021
 * 
 * This library was developed with ESP8266 family in mind. Although it is fully compatible with arduino family
 */

#include "FSEDHT11.h"

#define USE_RX_PIN //uncomment this line when using RX pin
#ifdef USE_RX_PIN // using RX pin on ESP8266
FSEDHT11 dht11(3);
#else
FSEDHT11 dht11(2); // any other pin other than RX
#endif

void setup() {
#ifdef USE_RX_PIN
  pinMode(3, FUNCTION_3); // setting RX pin to FUNCTION_3(GPIO)
#endif
  Serial.begin(115200);
  Serial.setTimeout(2000);
  // Wait for serial to initialize.
  while(!Serial) { }
  
  //register your callbackx
  dht11.afterRead(setData);
  dht11.onError(error);
}

void error(FSEDHT_error_t error) {
  Serial.print("DHT11 Failed - Error Code: ");
  Serial.print(error.errorNum);
  Serial.print(", Error Message: ");
  Serial.println(error.errorMsg);
}

void setData(float _humidity, float _temperature) {
  Serial.print("DHT11 OK - H: ");
  Serial.print(_humidity);
  Serial.print(", T: ");
  Serial.println(_temperature);
}

void loop() {
  // put your main code here, to run repeatedly:
  dht11.read();
  delay(3000);
}
```

Using callback passing a pointer to the object which can be used to access other values from the library

```
#include "FSEDHT11.h"

#define USE_RX_PIN //uncomment this line when using RX pin
#ifdef USE_RX_PIN // using RX pin on ESP8266
FSEDHT11 dht11(3);
#else
FSEDHT11 dht11(2); // any other pin other than RX
#endif

void setup() {
#ifdef USE_RX_PIN
  pinMode(3, FUNCTION_3); // setting RX pin to FUNCTION_3(GPIO)
#endif
  Serial.begin(115200);
  Serial.setTimeout(2000);
  // Wait for serial to initialize.
  while(!Serial) { }
  
  //register your callbackx
  dht11.afterRead(setData);
  dht11.onError(error);
}

void error(FSEDHT_error_t error) {
  Serial.print("DHT11 Failed - Error Code: ");
  Serial.print(error.errorNum);
  Serial.print(", Error Message: ");
  Serial.println(error.errorMsg);
}

void setData(FSEDHT11 *dht11) {
  Serial.print("DHT11 OK - H: ");
  Serial.print(dht11->getHumidity());
  Serial.print(", T: ");
  Serial.print(dht11->toFahrenheit());
  Serial.println(" F");
}

void loop() {
  // put your main code here, to run repeatedly:
  dht11.read();
  delay(3000);
}
```
