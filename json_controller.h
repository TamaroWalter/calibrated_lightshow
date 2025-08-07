#ifndef JSON_CONTROLLER_H
#define JSON_CONTROLLER_H

char* buildEffect(int command);

char* buildStaticJson(int brightness, const std::vector<std::array<int, 2>>& r1, int* c1, const std::vector<std::array<int, 2>>& r2, int* c2, const std::vector<std::array<int, 2>>& r3, int* c3);
char* buildDynamicJson(int effectid, int brightness, int* c1, int* c2, int* c3);
#endif