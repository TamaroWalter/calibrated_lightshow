/*
 Basic ESP32 MQTT example

 
*/

#include <WiFi.h>   //Bibliothek --> Kommunikation auf unteren Leveln
#include <PubSubClient.h> //MQTT Protokoll

// Update these with values suitable for your network.


//const char* ssid = "TP-Link_0072";
//const char* password = "45208362";
//const char* mqtt_server = "192.168.0.100";

//WIFI einstellen
const char* ssid = "ploen_iot";   
const char* password = "ploenerdoener";
const char* mqtt_server = "agiot.reparts.org";


WiFiClient mqtt_node;
PubSubClient client(mqtt_node);
long lastMsg = 0;  //verschiedene Variablen-Typen definieren und Wert zuweisen
char msg[50];
float value1 = 0.0;
float value2 = 0.0;

const int ledPin = 2;  //read only


void setup_wifi() {

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
  String topicStr = String(topic);
  String message = "";

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  float num = message.toFloat();

  if (topicStr == "gruppe2/testwerte/1") {
    value1 = num;
    Serial.print("Received value1: ");
    Serial.println(value1);
  } else if (topicStr == "gruppe2/testwerte/2") {
    value2 = num;
    Serial.print("Received value2: ");
    Serial.println(value2);
  } else {
    Serial.print("Unknown topic: ");
    Serial.println(topicStr);
  }
  
  //schreiben nach einer Aenderung
  if (value1 > 10){
      client.publish("gruppe2", "on");
    }
    if (value1 <= 10){
      client.publish("gruppe2", "off");
    }
}

//mit Brokter verbinden
void reconnect() {
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
      client.subscribe("gruppe2/testwerte/1");
      client.subscribe("gruppe2/testwerte/2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//setup und loop sind oft getrennt (also setup ist Initialisierung, loop laeuft dann unendlich durch)
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
  client.loop();  //aktiviert Callback, wenn Nachricht reinkommt
}
