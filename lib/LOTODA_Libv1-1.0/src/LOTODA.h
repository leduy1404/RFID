/**
 * LOTODA.h - Arduino Library of IoT LOTODA https://lotoda.vn
 */
#include <Arduino.h>

#define LOTODA_URL_HOST           "https://lotoda.vn/"
#define LOTODA_API_HOST           "https://api.lotoda.vn"
#define LOTODA_MQTT_HOST          "app.lotoda.vn"
#define LOTODA_VERSION            "1.0.0"

#define SENSOR_FIELD              "SENSOR"

#include "MqttClient.h"

#ifdef ESP8266
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
#endif
#ifdef ESP32
    #include <HTTPClient.h>
    #include <Update.h>
#endif

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define LOTODA_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define LOTODA_CALLBACK_SIGNATURE void void (*callback)(char*, uint8_t*, unsigned int)
#endif

class LOTODA {
    private:
        String useridkey;
        String passidkey;
        String deviceid;
        String nodesecretkey;

        HardwareSerial* debugPrinter;
        Client* _client;

#if defined(ESP8266) || defined(ESP32)
        WiFiClient wiFiClient;
#endif
        MqttClient mqttClient;
        volatile unsigned long nextPublishValue = 0;
        volatile unsigned long nextpublishUpdate = 0;
        void debugPrint(String function, int line, String msg);
        LOTODA_CALLBACK_SIGNATURE;
        bool mqttConnect();
        
    public:
        LOTODA(String useridkey, String passidkey, String deviceid);
        LOTODA(String useridkey, String passidkey, String deviceid, String nodesecretkey);

        LOTODA* setCheckDuration(unsigned long checkDuration);
        LOTODA* setDebugPrinter(HardwareSerial* debugPrinter);
        LOTODA* setConnectClient(Client* client);

        bool loop();
        bool publishSensorValue(float value);
        bool publishSensorJsonValueOne(char* str, float value);
        bool publishSensorJsonValueTwo(char* str1, float value1, char* str2, float value2);
        bool getControlRelay();
        LOTODA& setCallback(LOTODA_CALLBACK_SIGNATURE);
        bool mqttCallback(char* topic, unsigned char* message, unsigned int length);

        void stateControlRelay(int NUMBER_OF_RELAYS);
        bool mqttCallbackControlRelay(char* topic, unsigned char* message, unsigned int length);
        void stat(String relaytopic, String relaystate);
};