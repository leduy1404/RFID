/**
 * LOTODA-Config.cpp - Arduino Library of IoT LOTODA https://lotoda.vn
 */
#ifndef _LOTODA_CONFIG_h
#define _LOTODA_CONFIG_h

#include <arduino.h>

#define RELAY_ON 1
#define RELAY_OFF 0
#define RELAY_SIGNAL_TRIGGER_HIGH 1
#define RELAY_SIGNAL_TRIGGER_LOW 0

#define CMND "cmnd"
#define STAT "stat"
#define TELE "tele"

#define _NUMBER_OF_RELAYS

struct MQTT_BASIC_CONFIG {
  char topic[65];
};


struct RELAY_DEFAULTS {
  uint8_t powerOn;
  String powerOnStr;
  uint8_t MQTTConnected;
};

struct RELAY {
  uint8_t gpio;
  String name;
  uint8_t triggerSignal;
  MQTT_BASIC_CONFIG mqtt;
  RELAY_DEFAULTS state;
};
  extern struct RELAY _Relay[_NUMBER_OF_RELAYS];

struct DEVICE {
  char name[33];
  uint8_t noOfButtons;
  uint8_t noOfRelays;
  uint8_t noOfLEDs;
  boolean isAnalogInput;
};
extern struct DEVICE _Device;


void setControlRelay(int id);
void on(int id);
void off(int id);
#endif // _LOTODA_CONFIG_h