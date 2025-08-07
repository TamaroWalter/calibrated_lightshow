#include <ArduinoJson.h>
#include "json_controller.h"


/**
 * returns the right Effect (json) based on a commandId. 
 */
char* buildEffect(int command) {
  switch (command) {
    case 1:
      // Case: Lage horizontal.
      return buildJson(38, 255, (int[3]){252, 181, 3}, (int[3]){0, 255, 0}, (int[3]){0, 0, 255});
    case 2:
      // Case: Lage vertikal gerade.
      return buildJson(0, 255, (int[3]){0, 0, 255}, nullptr, nullptr);
    case 3:
      // Case: Lage vertikal hoch.
      return buildJson(0, 255, (int[3]){3, 252, 69}, nullptr, nullptr);
    case 4:
      // Case: Bewegung horizontal.
      return buildJson(0, 255, (int[3]){235, 3, 252}, nullptr, nullptr);
    case 5:
      // Case: Bewegung vertikal gerade.
      return buildJson(0, 255, (int[3]){252, 235, 3}, nullptr, nullptr);
    case 6:
      // Case: Bewegung vertikal hoch.
      return buildJson(0, 255, (int[3]){255, 0, 0}, nullptr, nullptr);
    default:
      // Dunkel ab wenn nichts bekannt ist.
      return buildJson(0, 0, (int[3]){255, 0, 0}, nullptr, nullptr);
  }
}


/**
 * Function to build the Json that controls the lights from the led-matrix.
 * - primary color is required, other colors are optional.
 */
char* buildJson(int effectid, int brightness, int* c1, int* c2, int* c3) {
  static char jsonBuffer[256];
  StaticJsonDocument<256> doc;

  doc["bri"] = brightness;
  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();
  segment["fx"] = effectid;

  JsonArray col = segment.createNestedArray("col");

  JsonArray color1 = col.createNestedArray();
  for (int i = 0; i < 3; i++) {
    color1.add(c1[i]);
  }

  if (c2 != nullptr) {
    JsonArray color2 = col.createNestedArray();
    for (int i = 0; i < 3; i++) {
      color2.add(c2[i]);
    }
  }
  if (c3 != nullptr) {
    JsonArray color3 = col.createNestedArray();
    for (int i = 0; i < 3; i++) {
      color3.add(c3[i]);
    }
  }

  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}