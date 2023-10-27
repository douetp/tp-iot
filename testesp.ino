/*#include <WiFi.h>
#include <PubSubClient.h>

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
}

void loop() { 

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
  if (client.connected()) {
    client.publish("/info/swag", " ");
    Serial.println("published");
  } else {
    Serial.println("on saute la pause");
  }
  
  delay (1000);
}
*/