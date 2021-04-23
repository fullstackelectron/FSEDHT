/**
 * DHT11 Library
 * Author: Fullstack Electron
 * https://fullstackelectron.com/
 *  Created: April 22, 2021
 * 
 * This library was developed with ESP8266 family in mind. Although it is fully compatible with arduino family.
 * 
 * this example uses a callback for reading which will pass through the humidity and temperature values
 */

#include <FSEDHT.h>

#define USE_RX_PIN //uncomment this line when using RX pin
#ifdef USE_RX_PIN // using RX pin on ESP8266
FSEDHT dht(3);
#else
FSEDHT dht(2); // any other pin other than RX
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
  dht.afterRead(setData);
  dht.onError(error);
}

void error(FSEDHT_error_t error) {
  Serial.print("DHT11 Failed - Error Code: ");
  Serial.print(error.errorNum);
  Serial.print(", Error Message: ");
  Serial.println(error.errorMsg);
}

void setData(float _humidity, float _temperature) { // default Celsius
  Serial.print("DHT11 OK - H: ");
  Serial.print(_humidity);
  Serial.print(", T: ");
  Serial.println(_temperature);
  Serial.println(" C");
}

void loop() {
  // put your main code here, to run repeatedly:
  dht.read();
  delay(3000);
}