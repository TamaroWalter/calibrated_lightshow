/*
 Basic ESP32 MQTT example
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include "json_controller.h"
// Update these with values suitable for your network.


//const char* ssid = "TP-Link_0072";
//const char* password = "45208362";
//const char* mqtt_server = "192.168.0.100";

const char* ssid = "ploen_iot";
const char* password = "ploenerdoener";
const char* mqtt_server = "agiot.reparts.org";


WiFiClient mqtt_node;
PubSubClient client(mqtt_node);
long lastUpdate = 0;
const int ledPin = 2;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

/**
 * This function can read the content of a subcribed topic and then do something with it.
 * @param topic - the topic that was subscribed to
 * @param payload array - The content that was send (e.g. the json that was published to topic/api)
 * @param length - the length of the callback
 */
void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      client.subscribe("gruppe2/api");
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);     // Initialize the ledPin pin as an output
  digitalWrite(ledPin, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastUpdate > 10000) {
    lastUpdate = now;
    int brightness = 255;
    int color[3] = {255, 0, 0};
    char* effect = buildJson(0, brightness, color, nullptr, nullptr)
    client.publish("gruppe2/api", effect);
  }
}