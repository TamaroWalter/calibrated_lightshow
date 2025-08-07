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
        {39,41}, {55,57}, {71,72}, {88,89}, {102,104}, {120,122}, {134,136}, {152,154}, {167,168}, {184,185}, {199,201},{215, 217}
      };

      std::vector<std::array<int, 2>> inner = {{72, 88}, {104,120}, {136, 152}, {168, 184}};
      std::vector<std::array<int, 2>> off = {{0,39}, {41,55}, {57,71}, {89, 102}, {122, 134}, {154,167}, {185,199}, {201, 215}, {217,256}};
      int outerColor[3] = {255, 255, 0};
      int innerColor[3] = {0, 0, 255};
      int offColor[3] = {0,0,0};
      //return buildStaticJson(255, inner, innerColor, outer, outerColor, off, offColor);
      return buildDynamicJson(0, 255, (int[3]){0,0,255}, nullptr, nullptr);
    }
    case 2: {
      // Case: Lage vertikal gerade.
      std::vector<std::array<int, 2>> outer = {
        {15,17}, {46,50}, {77,78}, {82,83}, {173,174}, {178,179}, {206,210}, {239,241}
      };

      std::vector<std::array<int, 2>> inner = {{79,81}, {111,113}, {143,145}, {175,177}};
      std::vector<std::array<int, 2>> off= {{0,15}, {17,46}, {50,77}, {78,79},{81,82}, {83,111}, {113,143}, {145,173},{174,175}, {177,178}, {179,206}, {210,239}, {241,256}};
      int outerColor[3] = {255, 0, 0};
      int innerColor[3] = {0, 0, 255};
      int offColor[3] = {0,0,0};
      //return buildStaticJson(255, inner, innerColor, outer, outerColor, off, offColor);
      return buildDynamicJson(0, 255, (int[3]){255,0,0}, nullptr, nullptr);
    }
    case 3:
      // Case: Lage vertikal hoch.
      return buildDynamicJson(153, 255, (int[3]){0, 255, 0}, nullptr, nullptr);
    case 4:
      // Case: Bewegung horizontal.
      return buildDynamicJson(38, 255, (int[3]){255, 0, 0}, (int[3]){0, 0, 255}, (int[3]){0, 255, 0});
    case 5:
      // Case: Bewegung vertikal.
      return buildDynamicJson(130, 255, (int[3]){252, 235, 3}, (int[3]){0,0,0}, nullptr);
    default:
      // Dunkel ab wenn nichts bekannt ist.
      return buildDynamicJson(0, 0, (int[3]){255, 0, 0}, nullptr, nullptr);
  }
}

char* buildStaticJson(int brightness, const std::vector<std::array<int, 2>>& r1, int* c1, const std::vector<std::array<int, 2>>& r2, int* c2, const std::vector<std::array<int, 2>>& r3, int* c3) {
  static char jsonBuffer[1024];
  StaticJsonDocument<1024> doc;
  doc["bri"] = brightness;
  JsonArray seg = doc.createNestedArray("seg");
  JsonObject segment = seg.createNestedObject();
  segment["sx"] = 126;
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
  segment["on"] = true;
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