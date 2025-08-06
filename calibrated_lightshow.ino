// default I2C-address for MPU6050: 0x68

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <MadgwickAHRS.h>

Adafruit_MPU6050 mpu;
Madgwick filter;

// IoT
// #include <ArduinoJson.h>
#include "pubSub.h"
String topic = "gruppe2/imu";

void setup(void) {
  // IoT Stuff
  initMQTT();

  // IMU Stuff
  Serial.begin(115200);
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) { // fails every time
    Serial.println("Failed to find MPU6050 chip");
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  filter.begin(100); // 100Hz loop frequency

  Serial.println("");
  delay(100);
}






void loop() {

  unsigned long tic = micros();

  updateMQTT();

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  filter.updateIMU(g.gyro.x, g.gyro.y, g.gyro.z, a.acceleration.x, a.acceleration.y, a.acceleration.z);

  // StaticJsonDocument<200> doc; // create JSON with arduino library

  // doc["t"] = temp.temperature;

  // JsonArray acc = doc.createNestedArray("a");
  // acc.add(a.acceleration.x);
  // acc.add(a.acceleration.y);
  // acc.add(a.acceleration.z);

  // JsonArray gyr = doc.createNestedArray("g");
  // gyr.add(g.gyro.x);
  // gyr.add(g.gyro.y);
  // gyr.add(g.gyro.z);

  // String jsonString;
  // serializeJson(doc, jsonString);
  // pushJson(topic.c_str(), jsonString.c_str());


  char buffer[12];         // Buffer to store the converted string
  dtostrf(filter.getPitch(), 6, 6, buffer); // (value, width, precision, buffer)
  pushJson("gruppe2/pitch", buffer);
  dtostrf(filter.getRoll(), 6, 6, buffer); // (value, width, precision, buffer)
  pushJson("gruppe2/roll", buffer);
  dtostrf(filter.getYaw(), 6, 6, buffer); // (value, width, precision, buffer)
  pushJson("gruppe2/yaw", buffer);


  unsigned long toc = micros();

  // Serial.println(toc - tic);
  
  if((toc-tic) <= 10000) {
    delayMicroseconds(10000 - (toc-tic));
  }
  // Serial.println("done.");
}