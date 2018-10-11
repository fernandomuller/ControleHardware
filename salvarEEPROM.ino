

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <EEPROM.h>
////needed for library
//#include <DNSServer.h>
//#include <ESP8266WebServer.h>
//#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

void parseBytes(const char* str, char sep, /*byte*/ char* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}



int ledpin = D1, button = D2, buttonState=0;
unsigned long timer = 0;
unsigned long currentMillis = 0;
char ipbanco[20] = "ipbanco";

union {
    unsigned char buf[sizeof(uint8_t) * 5];
    struct {
        uint8_t ipSave;// = 0;
        uint8_t ipServer[4];// = "";              
    }campos;    
}data;
unsigned char buf2[sizeof(uint8_t) * 5];
unsigned int addr = 250;

void setup() {

  Serial.begin(115200);
  
    Serial.println();
    for(int i = 0; i < 5; ++i)Serial.print(buf2[i]);
    Serial.println();     
    
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(ledpin,OUTPUT);
    pinMode(button,INPUT);    

    EEPROM.begin(512);//sizeof(data.buf));//Inicia a EEPROM com tamanho de n Bytes 
    EEPROM.get(addr,buf2); 
    
    Serial.println();
    for(int i = 0; i < 5; ++i)Serial.print(buf2[i]);
    Serial.println();
    
//    for (int i = 0; i < sizeof(data.buf); ++i){
//      //ipbanco[i] = EEPROM.read(addr+i);
//      EEPROM.get(addr+i,data.buf);
//    }   

//    data.campos.ipSave = 0;
//    for(int i = 0; i < 4; ++i)data.campos.ipServer[i]=i+4;
//    
//    if(data.campos.ipSave==0){
//         for (int i = 0; i < sizeof(data.buf); ++i){      
//            EEPROM.write(addr+i, data.buf[i]);
//         } 
//         Serial.println("\n Gravou na EEPROM......");    
//    }
//    EEPROM.commit();
    //EEPROM.get(addr,buf2); 
    
    Serial.println();
    for(int i = 0; i < 5; ++i)Serial.print(buf2[i]);
    Serial.println();
    //EEPROM.end();//Fecha a EEPROM.    

          const char* ipStr = "50.100.150.200";
          char ip[4];
          parseBytes(ipStr, '.', ip, 4, 10);
          
          IPAddress novoip(ip[0], ip[1], ip[2], ip[3]);
          Serial.print("novoip address formatado: ");
          Serial.println(novoip);

             
          
}

void loop() {

    currentMillis = millis();
    if (currentMillis - timer >= 4000) {
      timer = currentMillis;   
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      //EEPROM.get(addr,buf2); 
      for (int i = 0; i < sizeof(data.buf); ++i){      
            buf2[i] = EEPROM.read(addr+i);
    }
      Serial.println();
      for(int i = 0; i < 5; ++i)Serial.print(buf2[i]);
      Serial.println();
    }
  
    buttonState=digitalRead(button); 
    if (buttonState == 1){
    data.campos.ipSave = 0;
    for(int i = 0; i < 4; ++i)data.campos.ipServer[i]=i+4;   
     Serial.println();
    for(int i = 0; i < 5; ++i)Serial.print(data.buf[i]);
     Serial.println();
     
    for (int i = 0; i < sizeof(data.buf); ++i){      
            EEPROM.write(addr+i, data.buf[i]);
    } 
    Serial.println("\n Gravou na EEPROM......");    
    
    EEPROM.commit();
                        
//        EEPROM.begin(5);//Inicia a EEPROM com tamanho de 5 Bytes (minimo).        
//        Serial.println(EEPROM.read(0));
//        EEPROM.write(0, 1);
//        for (int i = 0; i < 3; ++i){      
//          EEPROM.write(i, ipStr[i]);
//        }        
//        for (int i = 0; i < 3; ++i){
//          ipbanco[i] = EEPROM.read(i);
//              //EEPROM.write(i, request[i]);
//        } 
//        Serial.println(ipbanco);
//        EEPROM.end();//Fecha a EEPROM.
                
        digitalWrite(ledpin, HIGH); 
        delay(200);
     }
     if (buttonState==0){
         digitalWrite(ledpin, LOW); 
         delay(200);
     }    
}
