#include <WiFi.h>
#include <LoRa.h>
#include <PubSubClient.h>
#include <SPI.h>
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI0 26
#define freq 869E6
#define sf 9
#define sb 250E3

union pack {
  uint8_t frame[16];
  float data[4];
} sdp;


#define ssid "iPhone de Hugo"

#define pass "Mimicbf0d"

const char* mqttServer = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {


    Serial.begin(9600); 
    WiFi.begin(ssid, pass);
    while (WiFi.status () != WL_CONNECTED) { 
      delay (500);

      Serial.print(".");

      Serial.println("Connected to the WiFi network"); 
      client.setServer (mqttServer, 1883); 
      while (!client.connected()) {
        Serial.println("Connecting to MOTT..."); 
        if (client.connect ("ESP32Client")) 
        { 
          Serial.println("connected");
        } else { 
        Serial.print ("failed with state "); 
        Serial.print (client.state()); 
        delay(2000);
      }
    }
    client.publish("/info/swag", "Bonjour voici nos supers infos youhou"); 
    Serial.println("published");
    
  }
  pinMode(DI0,INPUT);
  SPI.begin(SCK,MISO,MOSI,SS);
  LoRa.setPins(SS,RST,DI0);
  if(!LoRa.begin(freq)){
    Serial.println("Starting LoRa marche pas");
    while(1);
  }
  LoRa.setSpreadingFactor(sf);
  LoRa.setSignalBandwidth(sb);
  }



float d1,d2;
int i=0;
void loop() { 

  while (!client.connected()) {
        
        if (client.connect ("ESP32Client")) 
        { 
          Serial.println("connected");
        } else { 
        Serial.print ("failed with state "); 
        Serial.print (client.state()); 
        delay(2000);
      }
  }
  if (client.connected()) {
    client.publish("/info/swag", "869000000 9 250000 ;");
    Serial.println("published");
  } else {
    Serial.println("on saute la pause");
  }
  int packetLen;
  packetLen = LoRa.parsePacket();
  if(packetLen==16){
    i=0;
    while(LoRa.available()){
      sdp.frame[i]=LoRa.read();
      i++;
    }
    d1 = sdp.data[0];
    d2 = sdp.data[1];
    int rssi;
    rssi = LoRa.packetRssi();
    Serial.println(d1);
    Serial.println(d2);
    Serial.println(rssi);
  }
  Serial.println(packetLen);
  delay (1000);
}
