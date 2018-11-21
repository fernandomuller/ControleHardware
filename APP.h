#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <EEPROM.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         D3           // Configurable, see typical pin layout above
#define SS_PIN          D8          // Configurable, see typical pin layout above

#ifndef UNTITLED_APP_H
#define UNTITLED_APP_H


class APP {

	private:    
		
	public:

		union {

				unsigned char buf[sizeof(uint8_t) * 11];

				struct {
					uint8_t ipFlag;
					uint8_t ipServer[4]; 
					uint8_t idSala;
					uint8_t idBancada;
					int ipPort;             
				}field;   

		}data;

		struct {
					//int novo;
					char id;
					unsigned long hora_inicio;             
		}registro;		

    int ledpin , button , buttonState;
    unsigned int addr = 200;
    unsigned long timer = 0,
                  currentMillis = 0;

		unsigned char * app_buffer;		

		void init( );
		void run();
		void readId(); 
		void funcaoApp();
		void post(int x);
   
    void parseBytes(const char* str, char sep, char* bytes, int maxBytes, int base);

};


#endif 
