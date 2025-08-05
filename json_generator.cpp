#include <ArduinoJson.h>
#include "json_generator.h"

/**
 * 
 */
char* buildJson(int effectid, int[3] c1, int[3] c2 = nullptr, int[3] c3 = nullptr) {
  // Static buffer, must stay in memory after function ends
  static char jsonBuffer[256];

  StaticJsonDocument<256> doc;

  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();

  segment["fx"] = effectid;

  JsonArray col = segment.createNestedArray("col");

  JsonArray color1 = col.createNestedArray();
  for (int i = 0; i < 3; i++) {
    color1.add(c1[i]);
  }

  /*
  JsonArray color2 = col.createNestedArray();
  color2.add(240);
  color2.add(3);
  color2.add(252);

  JsonArray color3 = col.createNestedArray();
  color3.add(252);
  color3.add(28);
  color3.add(3);*/

  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}
