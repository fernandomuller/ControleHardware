//https://www.dobitaobyte.com.br/wifi-manager-com-esp8266-na-ide-do-arduino/
//http://pedrominatel.com.br/pt/esp8266/biblioteca-wifimanager-para-o-esp8266/

//http://www.martyncurrey.com/esp8266-and-the-arduino-ide-part-5-adding-wifimanager/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <EEPROM.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

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

//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;
//static const uint8_t D9   = 3;
//static const uint8_t D10  = 1;

int ledpin = D1, button = D2, buttonState=0;
unsigned long timer = 0;
unsigned long currentMillis = 0;
char ipbanco[20] = "ipbanco";



class App{  //Classe que contém os dados do Banco. Esses valores são salvos na EEPROM
  public:   // e recuperados na inicialização (função setup()) do Esp.
    union {
    unsigned char buf[sizeof(uint8_t) * 7];
    struct {
        uint8_t ipSave;// = 0;
        uint8_t ipServer[4];// = "";
        uint16_t ipPort;              
    }field;    
}data;      
      //static const void processaMsg();
};
unsigned char buf2[sizeof(uint8_t) * 7];

//static const void App::processaMsg(){
//         // mySerial.print("\nProcessando Mensagem...Counter:  ");
//          //mySerial.println((uint16_t) user.app.valores.bufferRX[3]);
//}

App node;

unsigned int addr = 250;

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(ledpin,OUTPUT);
    pinMode(button,INPUT);    

    EEPROM.begin(512);//sizeof(data.buf));//Inicia a EEPROM com tamanho de n Bytes 
    EEPROM.get(addr,node.data.buf); 
    
    Serial.println();
    for(int i = 0; i < sizeof(node.data.buf); ++i)Serial.print(node.data.buf[i]);
    Serial.println();       
        
    WiFiManager wifiManager;    

    if(node.data.field.ipSave == 0){
        wifiManager.resetSettings(); 
        
        WiFiManagerParameter wifiParameter_IpBanco("ipbanco1", "ipbanco2", ipbanco, 20);
        wifiManager.addParameter(&wifiParameter_IpBanco);
        Serial.print("output1: ");Serial.println(ipbanco);    
        strcpy(ipbanco, wifiParameter_IpBanco.getValue());
        Serial.print("output2: ");Serial.println(ipbanco);
        
        //set custom ip for portal
        wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
        wifiManager.autoConnect("ESP_RFID_AP");

        // Trata dados do servidor passados pelo browser
        strcpy(ipbanco, wifiParameter_IpBanco.getValue()); 
               
        const char* ipStr = ipbanco;//"50.100.150.200";
          char ip[4];
          parseBytes(ipStr, '.', ip, 4, 10);
          
          IPAddress novoip(ip[0], ip[1], ip[2], ip[3]);
          Serial.print("IPAdress novoip passado pelo browser: "+novoip);
          //node.data.field.ipSave == 1;
          
          // Grava flag na EEPROM
          EEPROM.write(addr, 1);

          // Grava IP do servidor na EEPROM
          for (int i = 0; i < 4; ++i){      
                EEPROM.write(addr+i+1, ip[i]);
                //Serial.println("%d", ip[i]);
        } 
        EEPROM.commit();
        Serial.println("\n Gravou flag (modo STA) e dados do servidor na EEPROM......");    

        Serial.println("\n Verificando leitura......");
        EEPROM.get(addr,buf2);    
        Serial.println();
        for(int i = 0; i < sizeof(node.data.buf); ++i)Serial.print(buf2[i]);
        Serial.println();
        
        
    }else{
        wifiManager.autoConnect("ESP_RFID_AP");
        
    }
    
    Serial.print("softAPIP address1: "+WiFi.softAPIP());//  Serial.println(WiFi.softAPIP());          
    Serial.print("localIP address: ");  Serial.println(WiFi.localIP());
    
    //if you get here you have connected to the WiFi
    Serial.println("Conectado na rede WiFi local!)");
    
    //strcpy(ipbanco, wifiParameter_IpBanco.getValue());
    Serial.print("output3: ");  Serial.println(ipbanco);         
                     
}

void loop() {
  
    timer_1();    
  
    buttonState=digitalRead(button);
     
    if (buttonState == 1){       
       
        node.data.field.ipSave = 0;
        for(int i = 0; i < 4; ++i)node.data.field.ipServer[i]=i+4; 
          
        Serial.println();
        for(int i = 0; i < 5; ++i)Serial.print(node.data.buf[i]);Serial.println();
         
        for (int i = 0; i < sizeof(node.data.buf); ++i){      
                EEPROM.write(addr+i, node.data.buf[i]);
        } 
        Serial.println("\n Gravou flag (modo AP) na EEPROM......");    
        
        EEPROM.commit();
        EEPROM.end();
                
        digitalWrite(ledpin, HIGH); 
        delay(200);
        ESP.reset();
     }
     if (buttonState==0){
         digitalWrite(ledpin, LOW); 
         delay(200);
     }    
}
void timer_1(){
    currentMillis = millis();
    if (currentMillis - timer >= 5000) {
      timer = currentMillis;
      
      Serial.print("IP Servidor: ");
      for(int i = 1; i < 5; ++i)Serial.print(node.data.buf[i]);
      Serial.println();
         
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}  


