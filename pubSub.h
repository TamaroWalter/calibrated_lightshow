/*
 Basic ESP32 MQTT example

 
*/

#ifndef PUBSUB_H
#define PUBSUB_H

void initMQTT(void);
void pushJson(const char* topic, const char* jsonString);
void updateMQTT(void);

#endif