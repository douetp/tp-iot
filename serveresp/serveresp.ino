#include <WiFi.h>
#include <LoRa.h>
#include <PubSubClient.h>
#include <SPI.h>
#include "NimBLEDevice.h"

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DI0 26
#define freq 869E6
#define sf 9
#define sb 250E3

// The remote service we wish to connect to.
static BLEUUID serviceUUID("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static NimBLERemoteCharacteristic *pRemoteCharacteristic;
static NimBLEAdvertisedDevice *myDevice;


static void notifyCallback(
        NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
        uint8_t *pData,
        size_t length,
        bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char *) pData);
}

class MyClientCallback : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient *pclient) {
    }

    void onDisconnect(NimBLEClient *pclient) {
        connected = false;
        Serial.println("onDisconnect");
    }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    NimBLEClient *pClient = NimBLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(
            myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)

    // Obtain a reference to the service we are after in the remote BLE server.
    NimBLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        pClient->disconnect();
        return false;
    }


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        pClient->disconnect();
        return false;
    }

    // Read the value of the characteristic.
    if (pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
    }

    if (pRemoteCharacteristic->canNotify())
        pRemoteCharacteristic->registerForNotify(notifyCallback);
    connected = true;
    return true;
}

class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
/**
   * Called for each advertising BLE server.
   */
    void onResult(NimBLEAdvertisedDevice advertisedDevice) {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

            NimBLEDevice::getScan()->stop();
            myDevice = new NimBLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
            doScan = true;

        } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks


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
    Serial.println("Starting Arduino BLE Client application...");
    NimBLEDevice::init("");
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
    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 5 seconds.
    NimBLEScan *pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
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

   // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true) {
        if (connectToServer()) {
            Serial.println("We are now connected to the BLE Server.");
        } else {
            Serial.println("We have failed to connect to the server; there is nothin more we will do.");
        }
        doConnect = false;
    }

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected) {
        Serial.println("Connecté");
        //pRemoteCharacteristic->writeValue(meteoString.c_str(), meteoString.length());
        
    } else if (doScan) {
        NimBLEDevice::getScan()->start(
                0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
    }
  delay (1000);
}
