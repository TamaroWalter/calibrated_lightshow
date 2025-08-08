
#include <WiFi.h>   //Bibliothek --> Kommunikation auf unteren Leveln
#include <PubSubClient.h> //MQTT Protokoll
#include <ArduinoJson.h>


//WIFI einstellen
const char* ssid = "ploen_iot";   
const char* password = "ploenerdoener";
const char* mqtt_server = "agiot.reparts.org";


WiFiClient mqtt_node;
PubSubClient client(mqtt_node);

//float t_sens;
//float axr, ayr, azr;
//float axb, ayb, azb;
float axw, ayw, azw; //float azwmg, azwog;
float gx, gy, gz;
float gxrad, gyrad, gzrad;
//float sinr, cosr, sinp, cosp, siny, cosy;
float vxw, vyw, vzw, vxyw, vges;
//const float timestep = 0.01;
int lightid; int lightidstable;
//#define DEG_TO_RAD 0.01745329251994329576923690768489
//#define PI 3.14159265358979323846
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
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");

  // Copy payload into a string and add null terminator
  char jsonStr[length + 1];
  memcpy(jsonStr, payload, length);
  jsonStr[length] = '\0';

//  Serial.println(jsonStr);  // Debug print

  // Parse JSON
  StaticJsonDocument<200> doc;  // Adjust size if needed
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Extract values
//  t_sens = doc["t"];
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

  /*
  axb = axr * 1.02; 
  ayb = ayr * 0.99;
  azb = azr * 0.96;

  //Serial.print("t: "); Serial.println(t_sens);
  //Serial.print("a: "); Serial.print(axb); Serial.print(", "); Serial.print(ayb); Serial.print(", "); Serial.println(azb); 
  //Serial.print("g: "); Serial.print(gx); Serial.print(", "); Serial.print(gy); Serial.print(", "); Serial.println(gz);
  
  // Compute rotation matrix from body to world frame
  sinr = sin(gxrad);
  cosr = cos(gxrad);
  sinp = sin(gyrad);
  cosp = cos(gyrad);
  siny = sin(gzrad);
  cosy = cos(gzrad);

  // Rotation matrix R (body → world)
  float R[3][3] = {
    {cosy * cosp, cosy * sinp * sinr - siny * cosr, cosy * sinp * cosr + siny * sinr},
    {siny * cosp, siny * sinp * sinr + cosy * cosr, siny * sinp * cosr - cosy * sinr},
    {-sinp,       cosp * sinr,                      cosp * cosr}
  };

  // Rotate acceleration vector into world frame
  axw = R[0][0]*axb + R[0][1]*ayb + R[0][2]*azb;
  ayw = R[1][0]*axb + R[1][1]*ayb + R[1][2]*azb;
  azwmg = R[2][0]*axb + R[2][1]*ayb + R[2][2]*azb;
  azwog = azwmg - 9.8;

  // calculate speeds in world frame
  vxw += axw * timestep;
  vyw += ayw * timestep;
  vzw += azwog * timestep;
  */

  vxyw = sqrt(vxw*vxw+vyw*vyw);
  vges = sqrt(vxw*vxw+vyw*vyw+vzw*vzw);
  client.publish("gruppe2/vxy", String(vxyw, 5).c_str());

  //angle intervals
  gxnull = (abs(gxrad) >= 0 && abs(gxrad) <= (1 * PI / 6)) || (abs(gxrad) >= (5 * PI / 6) && abs(gxrad) <= PI);
  gynull = (abs(gyrad) >= 0 && abs(gyrad) <= (1 * PI / 6)) || (abs(gyrad) >= (5 * PI / 6) && abs(gyrad) <= PI);
  gxnz = (abs(gxrad) >= (2 * PI / 6) && abs(gxrad) <= (4 * PI / 6));
  gynz = (abs(gyrad) >= (2 * PI / 6) && abs(gyrad) <= (4 * PI / 6));
 
  //debug prints
  //Serial.print("v: "); Serial.print(vxw); Serial.print(", "); Serial.print(vyw); Serial.print(", "); Serial.print(vzw); Serial.print(", "); Serial.println(vges);

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
