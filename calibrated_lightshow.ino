
#include <WiFi.h>   //Bibliothek --> Kommunikation auf unteren Leveln
#include <PubSubClient.h> //MQTT Protokoll
#include <ArduinoJson.h>


//WIFI einstellen
const char* ssid = "ploen_iot";   
const char* password = "ploenerdoener";
const char* mqtt_server = "agiot.reparts.org";


WiFiClient mqtt_node;
PubSubClient client(mqtt_node);
float axw, ayw, azw;
float gx, gy, gz;
float gxrad, gyrad, gzrad;
float vxw, vyw, vzw, vxyw, vges;
int lightid; int lightidstable;
bool gxnull, gynull, gxnz, gynz;
int counter = 0;
int idhistory[30] = {0};
int counts[6] = {0};

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
  // Copy payload into a string and add null terminator
  char jsonStr[length + 1];
  memcpy(jsonStr, payload, length);
  jsonStr[length] = '\0';

  // Parse JSON
  StaticJsonDocument<200> doc;  // Adjust size if needed
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Extract values
  axw = doc["ax"];  //world frame
  ayw = doc["ay"];
  azw = doc["az"];
  gx = doc["r"];  //in degree; from -180 to 180 or -90 to 90
  gy = doc["p"];
  gz = doc["y"];
  vxw = doc["vx"];  //world frame
  vyw = doc["vy"];
  vzw = doc["vz"];

  gxrad = gx * DEG_TO_RAD;
  gyrad = gy * DEG_TO_RAD;
  gzrad = gz * DEG_TO_RAD;

  vxyw = sqrt(vxw*vxw+vyw*vyw);
  vges = sqrt(vxw*vxw+vyw*vyw+vzw*vzw);
  client.publish("gruppe2/vxy", String(vxyw, 5).c_str());

  //angle intervals
  gxnull = (abs(gxrad) >= 0 && abs(gxrad) <= (1 * PI / 6)) || (abs(gxrad) >= (5 * PI / 6) && abs(gxrad) <= PI);
  gynull = (abs(gyrad) >= 0 && abs(gyrad) <= (1 * PI / 6)) || (abs(gyrad) >= (5 * PI / 6) && abs(gyrad) <= PI);
  gxnz = (abs(gxrad) >= (2 * PI / 6) && abs(gxrad) <= (4 * PI / 6));
  gynz = (abs(gyrad) >= (2 * PI / 6) && abs(gyrad) <= (4 * PI / 6));
  //do something after each delivery of new values
  if (vges < 0.01){
      if(gxnull && gynull){
          lightid = 1;
        }
        else if (gxnull && gynz){
            lightid = 3;
          }
          else if (gxnz && gynull){
              lightid = 2;
            }
            else lightid = 0;
    }
    else if ((abs(vzw) > 0.018) && (abs(vzw) > vxyw)){
        lightid = 5;
      }
      else if ((vxyw > 0.022) && (abs(vzw) < vxyw)){
          lightid = 4;
        }
        else lightid = 0;

  //buffer IDs and count how many there are
  if (counter = 30){counter = 0;}
  memset(counts, 0, sizeof(counts));
  idhistory[counter] = lightid;
  for (int i = 0; i < 30; i++) {
    int val = idhistory[i];
    if (val >= 0 && val <= 5) {
      counts[val]++;
    }
  }
  for (int i = 0; i <= 5; i++) {Serial.print(i);Serial.print(": ");Serial.print(counts[i]);}Serial.println();
  if (counts[5] > 7){lightidstable = 5;}
    else if (counts[4] > 9){lightidstable = 4;}
      else if (counts[3] > 14){lightidstable = 3;}
        else if (counts[2] > 14){lightidstable = 2;}
          else if (counts[1] > 14){lightidstable = 1;}
            else lightidstable = 0;
  counter += 1;

  //send lightid
  Serial.print("ID: "); Serial.println(lightidstable);
  client.publish("gruppe2/lightid", String(lightidstable).c_str());

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
      client.subscribe("gruppe2/imu");
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
