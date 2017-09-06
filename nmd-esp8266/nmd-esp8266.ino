/***************************************************
  MQTT Button on Wemos

  James Sutton 2017 - jsutton.co.uk
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "iPhone van Michiel"
#define WLAN_PASS       "laminaat"

/************************* MQTT Setup *********************************/

#define MQTT_SERVER      "m20.cloudmqtt.com"
#define MQTT_SERVERPORT  11732                   // use 8883 for SSL
#define MQTT_USERNAME    "wqdmisbg"
#define MQTT_PASSWORD    "BmpJyW7W2Tz-"

/************************* Button Config  *********************************/
const int analogPin = 0;

/************************* Heartrate  *********************************/
const int heartratePin = D1;

int HeartrateSignal = 0;                // holds the incoming raw data. Signal value can range from 0-1024
int LastHeartrateSignal = 0;

/************************* Light  *********************************/
const int lightPint = D1;

int LightSignal = 0;                // holds the incoming raw data. Signal value can range from 0-1024
int LastLightSignal = 0;

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed for publishing.
Adafruit_MQTT_Publish heartratePub = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/nmd/heartratePub");
Adafruit_MQTT_Publish lightPub = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/nmd/lightPub");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {

  Serial.begin(115200);
  delay(10);

  //
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  
  Serial.println(F("MQTT Emergency Button"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Send Hello World Notification
   MQTT_connect();
}


void loop() {

  // read heartrate
  digitalWrite(D1, HIGH);
  delay(50);
  HeartrateSignal = analogRead(analogPin);
  sendUpdateHeartrate();
  digitalWrite(D1, LOW);
  //read light
  
  digitalWrite(D2, HIGH);
  LightSignal = analogRead(analogPin);
  sendUpdateLight();
  digitalWrite(D2, LOW);
  
  LastHeartrateSignal = HeartrateSignal;
  LastLightSignal = LightSignal;
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  
}

void sendUpdateHeartrate() {
  MQTT_connect();

  // Now we can publish stuff!
  Serial.print(F("\nSending heartrate"));
  Serial.print("...");
  if(HeartrateSignal != LastHeartrateSignal) {
    if (! heartratePub.publish(HeartrateSignal)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
  }

  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
    if(! mqtt.ping()) {
    mqtt.disconnect();
    }
}

void sendUpdateLight() {
  MQTT_connect();

  // Now we can publish stuff!
  Serial.print(F("\nSending light"));
  Serial.print("...");
  if(LightSignal != LastLightSignal) {
    if (! lightPub.publish(LightSignal)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
