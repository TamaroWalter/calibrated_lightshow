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
bool commandChanged = false;
int command = -1;
bool brightnessChanged = false;
int brightness = -1;
const int ledPin = 2;

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  // Safely convert payload to a printable C string
  char msg[length + 1]; // +1 for null terminator
  memcpy(msg, payload, length);
  msg[length] = '\0'; // Null-terminate

  int tempCommand = atoi(msg);
  if (command != tempCommand) {
    command = tempCommand;
    commandChanged = true;
  } else {
    commandChanged = false;
  }
  command = atoi(msg);
  Serial.print("Received commando:");
  Serial.println(command); // This prints properly with newline
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      client.subscribe("gruppe2/kommando");
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
  // Update at 100hz.
  if (now - lastUpdate > 100) {
    lastUpdate = now;
    //int brightness = 255;
    //int color[3] = {255, 0, 0};
    if (commandChanged || brightnessChanged) {
      char* effect = buildEffect(command);
      client.publish("gruppe2/api", effect);
    }
  }
}