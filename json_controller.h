#ifndef JSON_CONTROLLER_H
#define JSON_CONTROLLER_H

char* buildEffect(int command);
char* buildJson(int effectid, int brightness, int* c1, int* c2, int* c3);

#endif