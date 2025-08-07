#include <ArduinoJson.h>
#include <vector>
#include <array>
#include "json_controller.h"


/**
 * returns the right Effect (json) based on a commandId. 
 */
char* buildEffect(int command) {
  switch (command) {
    case 1: {
      // Case: Lage horizontal. Baue die richtigen bereiche auf.
      std::vector<std::array<int, 2>> outer = {
        {39,41}, {55,57}, {71,72}, {88,89}, {102,104}, {120,122}, {134,136}, {152,154}, {167,168},
        {184,185},{199,201}, {215, 217}
      };

      std::vector<std::array<int, 2>> off = {
        {0,39}, {41,55}, {57,71}, {89, 102}, {122,134}, {154,167}, {185,199}, {201,215}, {217,255}
      };

      std::vector<std::array<int, 2>> inner = {{72, 88}, {104,120}, {136, 152}, {168, 184}};

      int outerColor[3] = {0, 255, 0};
      int innerColor[3] = {252, 181, 3};
      int offColor[3] = {0,0,0};
      // Use the minimal memory approach
      //return buildLedRangesJson(255, outerColor, offColor, innerColor);
      //return buildStaticJsonFromTemplate(255, outerColor, offColor, innerColor);
      return buildStaticJson(255, inner, innerColor, outer, outerColor, off, offColor);
    }
    case 2:
      // Case: Lage vertikal gerade.
      return buildDynamicJson(0, 255, (int[3]){0, 0, 255}, nullptr, nullptr);
    case 3:
      // Case: Lage vertikal hoch.
      return buildDynamicJson(0, 255, (int[3]){3, 252, 69}, nullptr, nullptr);
    case 4:
      // Case: Bewegung horizontal.
      return buildDynamicJson(0, 255, (int[3]){235, 3, 252}, nullptr, nullptr);
    case 5:
      // Case: Bewegung vertikal.
      return buildDynamicJson(0, 255, (int[3]){252, 235, 3}, nullptr, nullptr);
    default:
      // Dunkel ab wenn nichts bekannt ist.
      return buildDynamicJson(0, 0, (int[3]){255, 0, 0}, nullptr, nullptr);
  }
}

char* buildStaticJson(int brightness, const std::vector<std::array<int, 2>>& r1, int* c1, const std::vector<std::array<int, 2>>& r2, int* c2, const std::vector<std::array<int, 2>>& r3, int* c3) {
  static char jsonBuffer[1024];
  StaticJsonDocument<1024> staticDoc;

  staticDoc["bri"] = brightness;
  JsonObject seg = staticDoc.createNestedObject("seg");
  JsonArray i = seg.createNestedArray("i");

  i.add(0);
  i.add(255);
  JsonArray color = i.createNestedArray();
  color.add(255);
  color.add(0);
  color.add(0);
  // Add r1 ranges with c1 colors
  for (const auto& range : r1) {
    i.add(range[0]);
    i.add(range[1]);
    JsonArray color = i.createNestedArray();
    color.add(c1[0]);
    color.add(c1[1]);
    color.add(c1[2]);
  }
  
  // Add r2 ranges with c2 colors
  for (const auto& range : r2) {
    i.add(range[0]);
    i.add(range[1]);
    JsonArray color = i.createNestedArray();
    color.add(c2[0]);
    color.add(c2[1]);
    color.add(c2[2]);
  }
  
  // Add r3 ranges with c3 colors
  for (const auto& range : r3) {
    i.add(range[0]);
    i.add(range[1]);
    JsonArray color = i.createNestedArray();
    color.add(c3[0]);
    color.add(c3[1]);
    color.add(c3[2]);
  }
  serializeJson(staticDoc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}

/**
 * Function to build the Json that controls the lights from the led-matrix.
 * - primary color is required, other colors are optional.
 * 
 */
char* buildDynamicJson(int effectid, int brightness, int* c1, int* c2, int* c3) {
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