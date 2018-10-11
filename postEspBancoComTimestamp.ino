#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char *ssid = "??????";  //ENTER YOUR WIFI SETTINGS
const char *password = "??????";

WiFiUDP ntpUDP; 
int16_t utc = -3; //UTC -3:00 Brazil 
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);

uint32_t  currentMillis = 0,
          previousMillis = 0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router

  // Wait for connection
  Serial.print("\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.print("\nConnected to: "+String(ssid)+". IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  timeClient.begin();
  timeClient.update();
}

void forceUpdate(void) {
  timeClient.forceUpdate();
}
void checkOST(void) {
  currentMillis = millis();//Tempo atual em ms
  if (currentMillis - previousMillis > 5000) {
    previousMillis = currentMillis;    // Salva o tempo atual
    Serial.println("Time Epoch: "+ String(timeClient.getEpochTime()));//printf
    Serial.println(timeClient.getFormattedTime());
  }
}


void loop() {
  
  HTTPClient http;    //Declare object of class HTTPClient

  StaticJsonDocument<500> doc;
  JsonObject jsonData = doc.to<JsonObject>();
      
  jsonData["ID"] = 10;
  jsonData["Nome"] = String(timeClient.getEpochTime());//"ESP8266";
  jsonData["Ativo"] = 1;      
  jsonData["Timestamp"] = String(timeClient.getEpochTime());
      
  String  postData = "",
          postIp   = "192.168.1.102",
          postPort = "5000",
          postApp  = "insereSala",
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
  
  delay(10000);  
  checkOST();
}
//=======================================================================
