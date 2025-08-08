#include <ArduinoJson.h>
#include <vector>
#include <array>
#include <string>
#include "json_controller.h"


/**
 * returns the right Effect (json) based on a commandId. 
 */
char* buildEffect(int command) {
  switch (command) {
    case 1: {
      // Case: Lage horizontal. Baue die richtigen bereiche auf.
      int textColor[3] = {245, 143, 0};
      return buildStringJson(1, textColor);
      //return buildStaticJson(255, inner, innerColor, outer, outerColor, off, offColor);
      //return buildDynamicJson(0, 255, (int[3]){0,0,255}, nullptr, nullptr);
    }
    case 2: {
      // Case: Lage vertikal gerade.
      int textColor[3] = {245, 49, 0};
      return buildStringJson(2, textColor);
      //return buildStaticJson(255, inner, innerColor, outer, outerColor, off, offColor);
      //return buildDynamicJson(0, 255, (int[3]){255,0,0}, nullptr, nullptr);
    }
    case 3: {
      // Case: Lage vertikal hoch.
      int textColor[3] = {18, 29, 150};
      return buildStringJson(3, textColor);
    }
    case 4:
      // Case: Bewegung horizontal.
      return buildDynamicJson(30, 255, 150, (int[3]){255, 111, 0}, (int[3]){21, 5, 166}, (int[3]){0, 255, 0});
    case 5:
      // Case: Bewegung vertikal.
      return buildDynamicJson(153, 255, 200, (int[3]){252, 235, 3}, (int[3]){0,0,0}, nullptr);
    default:
      // Dunkel ab wenn nichts bekannt ist.
      return buildDynamicJson(0, 0, 0, (int[3]){0, 0, 0}, nullptr, nullptr);
  }
}

char* buildStaticJson(int brightness, const std::vector<std::array<int, 2>>& r1, int* c1, const std::vector<std::array<int, 2>>& r2, int* c2, const std::vector<std::array<int, 2>>& r3, int* c3) {
  static char jsonBuffer[1024];
  StaticJsonDocument<1024> doc;

  doc["bri"] = brightness;
  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();
  segment["sx"] = 126;
  segment["id"] = 0;
  segment["freeze"] = false;
  JsonArray i = segment.createNestedArray("i");
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
  for (const auto& range : r3) {
    i.add(range[0]);
    i.add(range[1]);
    JsonArray color = i.createNestedArray();
    color.add(c3[0]);
    color.add(c3[1]);
    color.add(c3[2]);
  }
  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}

char* buildStringJson(int stringId, int* textColor) {
  static char jsonBuffer[1024];
  StaticJsonDocument<1024> doc;
  doc["on"] = true;
  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();
  segment["bri"] = 255;
  segment["set"] = 0;
  const char* text = ""; // pointer to a string literal

  switch (stringId) {
      case 1:
          text = "-- -- -- -- -- -- -- -- -- --";
          break;
      case 2:
          text = "| | | | | | | | | |";
          break;
      case 3:
          text = "+ + + + + + + + + +";
          break;
      default:
          text = ".. .. .. .. .. .. ..";
          break;
  }
  segment["n"] = text;
  JsonArray color = segment.createNestedArray("col");
  JsonArray c1 = color.createNestedArray();
  c1.add(textColor[0]);
  c1.add(textColor[1]);
  c1.add(textColor[2]);
  JsonArray c2 = color.createNestedArray();
  c2.add(0);
  c2.add(0);
  c2.add(0);
  JsonArray c3 = color.createNestedArray();
  c3.add(0);
  c3.add(0);
  c3.add(0);
  segment["fx"] = 122;
  segment["sx"] = 255;
  segment["ix"] = 127;
  segment["pal"] = 2;
  segment["c1"] = 0;
  segment["c2"] = 127;
  segment["c3"] = 16;
  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}

/**
 * Function to build the Json that controls the lights from the led-matrix.
 * - primary color is required, other colors are optional.
 * 
 */
char* buildDynamicJson(int effectid, int brightness, int speed, int* c1, int* c2, int* c3) {
  static char jsonBuffer[256];
  StaticJsonDocument<256> doc;
  doc["on"] = true;
  doc["bri"] = brightness;
  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();
  //segment.createNestedArray("i");
  segment["id"] = 0;
  segment["freeze"] = false;
  segment["fx"] = effectid;
  segment["sx"] = speed;
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

char* buildResetJson() {
  static char jsonBuffer[256];
  StaticJsonDocument<256> doc;
  doc["on"] = true;
  serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));
  return jsonBuffer;
}