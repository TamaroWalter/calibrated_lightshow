/*
 Basic ESP32 MQTT example

 
*/

#include "pubSub.h"
#include <WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.


//const char* ssid = "TP-Link_0072";
//const char* password = "45208362";
//const char* mqtt_server = "192.168.0.100";

const char* ssid = "ploen_iot";
const char* password = "ploenerdoener";
const char* mqtt_server = "agiot.reparts.org";


WiFiClient mqtt_node;
PubSubClient client(mqtt_node);
long lastMsg = 0;
char msg[50];
int value = 0;
int rob_status = 0;
int led = 0;

const int ledPin = 2;


void setup_wifi(void) {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, LOW);   // Turn the LED on (Note that LOW is the voltage level
    rob_status = 1;
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(ledPin, HIGH);  // Turn the LED off by making the voltage HIGH
    rob_status = 0;
  }

}

void reconnect(void) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("ploen/ag4/reconnect", "hello world");
      // ... and resubscribe
      // client.subscribe("gruppe2/imu");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void initMQTT(void) {
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void pushJson(const char* topic, const char* jsonString) {
  bool dummy = client.publish(topic, jsonString);
}

void updateMQTT(void) {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
