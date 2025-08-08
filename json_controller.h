#ifndef JSON_CONTROLLER_H
#define JSON_CONTROLLER_H

char* buildEffect(int command);

char* buildStaticJson(int brightness, const std::vector<std::array<int, 2>>& r1, int* c1, const std::vector<std::array<int, 2>>& r2, int* c2, const std::vector<std::array<int, 2>>& r3, int* c3);
char* buildStringJson(int stringId,  int* textColor);
char* buildDynamicJson(int effectid, int brightness, int speed, int* c1, int* c2, int* c3);
char* buildResetJson();
#endif