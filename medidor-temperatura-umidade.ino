/* WiFi */
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
const char* ssid     = "VOGEL_NETWORK";
const char* password = "lalaamoraluna";
WiFiClient client;

char server[] = "su-climate-scanner.herokuapp.com";

/* DHT22 */
#include <DHT.h>
#define DHTPIN D3  
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
float hum = 0;
float temp = 0;

/* Messaging */
String payload; 
String mac;

void setup() {
  Serial.begin(115200);
  
  // Wait for serial to initialize.
  while(!Serial) { }

  dht.begin();

  mac = WiFi.macAddress();

  wifiConnect();

  delay(2000);
}


void loop() {
  if(WiFi.status() != WL_CONNECTED){
    wifiConnect();
  } else {
    if (client.connect(server, 80)) {
      Serial.println("client connected.");
      
      String data = fillPayload().c_str();

      HTTPClient http;

      http.begin("http://"+ String(server) + "/measure");
      http.addHeader("Content-Type", "text/plain");

      //Serial.println("data: " + data);
      int httpCode = http.POST(data);

      String payload = http.getString();

      Serial.println("HTTP Code: " + httpCode);
      Serial.println("HTTP Response: " + payload);
 
      http.end();
    } else {
      // If you couldn't make a connection:
      Serial.println("Connection failed");
      Serial.println("Disconnecting.");
      client.stop();
    }
  }
  //60000 = 1 minute
  ESP.deepSleep(60 * 60000);
}

/***************************************************
 * WIFI Connect
 **************************************************/
void wifiConnect(){
    
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    for(int i = 0; i <= 5; i++){

      if(WiFi.status() != WL_CONNECTED){
       
        Serial.print(".");

        if(i == 5){
          Serial.println("Error occurred while connecting!");
          //fazer piscar led erro wifi
        }
        
        delay(500);
        
      } else {

        Serial.println();
        Serial.println("WiFi connected");  
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        //fazer piscar led sucesso wifi
        
        break;
        
      }
    }
}

/***************************************************
 * Fill Payload
 **************************************************/
String fillPayload()
{

  getDhtData();

  payload = "mac=";
  payload += mac;

  //início sensores
  payload += "&humidity=";
  payload += hum;
  payload += "&temperature=";
  payload += temp;
  
  //final sensores

  return payload;

}

/***************************************************
 * Get DHT data
 **************************************************/
void getDhtData(void)
{
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(hum) || isnan(temp))   // Check if any reads failed and exit early (to try again).
  {
    Serial.println("Failed to read from DHT sensor!");
    temp = 0;
    hum = 0;
    return;
  }
}
