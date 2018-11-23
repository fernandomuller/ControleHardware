#include "APP.h"

using namespace std;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

WiFiUDP ntpUDP; 
int16_t utc = -2; //UTC -3:00 Brazil 
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");//, utc*3600, 60000);

void APP::init( ){
    ledpin = D1;
    button = D2;
    buttonState=0;
    delay(200);
  
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

    timeClient.begin();
    timeClient.update();

    
   
    for (byte i = 0; i < 6; i++)key.keyByte[i] = 0xFF;

	  Serial.begin(115200);
    digitalWrite(button, LOW);
    pinMode(LED_BUILTIN,OUTPUT);
    pinMode(ledpin,OUTPUT);
    pinMode(button,INPUT);    
    
    EEPROM.begin(512); 
    EEPROM.get(addr,data.buf); 
    
    Serial.println();
    for(int i = 0; i < sizeof(data.buf); ++i){
          Serial.print(data.buf[i]);Serial.print(" ");
    } Serial.println(); 
    Serial.println(data.field.ipPort);
    Serial.println(data.field.idSala);
    Serial.println(data.field.idBancada);     

    while ( !timeClient.update() ) {
        Serial.println("Time Epoch: "+ String(timeClient.getEpochTime()));//printf
        Serial.println("Time UTC: "+ timeClient.getFormattedTime()+ "  Brazil: UTC-2:00 ");
        delay ( 3000 );
    }
      
    Serial.println("Time Epoch: "+ String(timeClient.getEpochTime()));//printf
    Serial.println("Time UTC: "+ timeClient.getFormattedTime()+ "  Brazil: UTC-2:00 ");
    
    WiFiManager wifiManager;    

    if(data.field.ipFlag == 0){
        wifiManager.resetSettings(); 
        
        WiFiManagerParameter wifiParameter_IpBanco("ipbanco", "ipbanco2", "ip servidor", 20);
        WiFiManagerParameter wifiParameter_portaBanco("portabanco", "portabanco2", "porta servidor", 20);
        WiFiManagerParameter id_sala("sala", "sala2", "id sala", 20);
        WiFiManagerParameter id_bancada("bancada", "bancada2", "id bancada", 20);

        wifiManager.addParameter(&wifiParameter_IpBanco);        
        wifiManager.addParameter(&wifiParameter_portaBanco);
        wifiManager.addParameter(&id_sala);
        wifiManager.addParameter(&id_bancada);

        //set custom ip for portal
        wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
        wifiManager.autoConnect("ESP RFID AP");

        // Trata dados do servidor passados pelo browser
        //strcpy(ipbanco, wifiParameter_IpBanco.getValue()); 
               
        const char* ipStr = wifiParameter_IpBanco.getValue();//ipbanco;
        char ip[4];
        parseBytes(ipStr, '.', ip, 4, 10);
          
          /*IPAddress novoip(ip[0], ip[1], ip[2], ip[3]);
          Serial.print("IPAdress novoip passado pelo browser: "+novoip);*/

        data.field.ipFlag = 1;
        for (int i = 0; i < 4; ++i)
                data.field.ipServer[i] = ip[i];
        data.field.ipPort =  String(wifiParameter_portaBanco.getValue()).toInt();
        data.field.idSala = String( id_sala.getValue()).toInt();
        data.field.idBancada = String(id_bancada.getValue()).toInt();
         
        // Grava Flag, IP, porta, id sala e id bancada na EEPROM
        for (int i = 0; i < sizeof(data.buf); ++i)     
                EEPROM.write(addr+i, data.buf[i]);

        EEPROM.commit();
        Serial.println("\n Gravou flag (modo STA) e dados do servidor na EEPROM......");    

        Serial.print("\n Verificando leitura......");
        EEPROM.get(addr,data.buf);   

        Serial.println();
        for(int i = 0; i < sizeof(data.buf); ++i){
          Serial.print(data.buf[i]);Serial.print(" ");
        }
        Serial.println();
        Serial.println(data.field.ipPort);
        Serial.println(data.field.idSala);
        Serial.println(data.field.idBancada);
        
        
    }else{
        Serial.println("\n autoConnect......");
        wifiManager.autoConnect("ESP RFID AP");
        
    }
    
    Serial.print("softAPIP address1: "+WiFi.softAPIP());          
    Serial.print("localIP address: ");  Serial.println(WiFi.localIP());
    Serial.print("GatewayIP address: ");  Serial.println(WiFi.gatewayIP().toString());    
    
    //if you get here you have connected to the WiFi
    Serial.println("Conectado na rede WiFi local!");    
   
}

void APP::run(){

  while(1){
    funcaoApp(); 
    readId();  
    buttonState=digitalRead(button);
     
    if (buttonState == 1){       
        Serial.print("  Reset por D2 nível alto: "+buttonState);  //Serial.println(buttonState);
               
        for (int i = 0; i < sizeof(data.buf); ++i){ 
                data.buf[i] = 0;    
                EEPROM.write(addr+i, data.buf[i]);
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
    
}
void APP::readId(){
  
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
        
    byte sector         = 1;
    byte blockAddr      = 4;    
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);
    
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
   
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    //dump_byte_array(buffer, 1); Serial.println();
    Serial.print(buffer[0], HEX);

    if(registro.id == 0){
        registro.id = buffer[0];
        registro.hora_inicio = timeClient.getEpochTime();
        post(1);
        timer = millis();
    }
    
     
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();



}

void APP::post(int inicio){
    HTTPClient http;    //Declare object of class HTTPClient
  
    StaticJsonDocument<500> doc;
    JsonObject jsonData = doc.to<JsonObject>();
        
    jsonData["Funcionario_ID"] = registro.id;
    jsonData["Bancada_ID"] = data.field.idBancada;
    jsonData["Bancada_Sala_ID"] = data.field.idSala;
    jsonData["Hora_inicio"] = String(registro.hora_inicio);
  
    if(inicio > 0){
          jsonData["Hora_fim"] = "0";        
          timer = millis();
    }else{
        jsonData["Hora_fim"] = String(timeClient.getEpochTime());
        registro.id = 0;
        registro.hora_inicio = 0;   
    }
    IPAddress novoip(data.buf[1],data.buf[2], data.buf[3], data.buf[4]);
  
    String  postData = "",
            postIp   = novoip.toString(),//"192.168.1.102",
            postPort = String(data.field.ipPort),//"5000",
            postApp  = "utilizaBancada",
            postDestination = "http://" + postIp + ":" + postPort + "/" + postApp;
            
    Serial.print(F("\nString postDestination: ")); Serial.println(postDestination);  
    
    serializeJson(jsonData,postData);
    Serial.print(F("String postData: ")); Serial.println(postData);  
    
    http.begin(postDestination);    //Specify request destination
    //http.begin("http://192.168.1.102:5000/insereSala");    //Specify request destination
    http.addHeader("Content-Type", "application/json");    //Specify content-type header
  
    Serial.println("\nFazendo Post no servidor...");
    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload
  
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
  
    http.end();  //Close connection

}

void APP::funcaoApp(){
  
    currentMillis = millis();
    if (currentMillis - timer >= 30000) {

        Serial.println("Time Epoch: "+ String(timeClient.getEpochTime()));//printf
        Serial.println("Time UTC: "+ timeClient.getFormattedTime()+ "  Brazil: UTC-2:00 ");

        if(registro.id > 0){
            post(0);
            
        }

        timer = currentMillis;
      
        Serial.println();
        for(int i = 0; i < sizeof(data.buf); ++i){
          Serial.print(data.buf[i]);Serial.print(" ");
        }
        Serial.println();

        Serial.print("IP Servidor: ");
        for(int i = 1; i < 5; ++i){
          Serial.print(data.buf[i]);
          Serial.print(".");
        }
        Serial.println();
        Serial.println(data.field.ipPort);
        Serial.println((int)data.field.idSala);
        Serial.println((int)data.field.idBancada);
         
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}
void APP::parseBytes(const char* str, char sep, char* bytes, int maxBytes, int base) {
    for (int i = 0; i < maxBytes; i++) {
        bytes[i] = strtoul(str, NULL, base);  // Convert byte
        str = strchr(str, sep);               // Find next separator
        if (str == NULL || *str == '\0') {
            break;                            // No more separators, exit
        }
        str++;                                // Point to next character after separator
    }
}
