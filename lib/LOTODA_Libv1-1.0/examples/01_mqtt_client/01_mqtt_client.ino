#include <Arduino.h>
/**
 *  Example of controlling MQTT client connect to IoT LOTOODA
 */
#include <LOTODA.h>

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#endif

#ifdef ESP32
  #include <WiFi.h>
#endif

/**
 * WiFi parameters.
 */
const char* ssid      = "ssid";
const char* password  = "password";
WiFiClient client;

/**
 * Initialize the connection with the cloud.
 */
String useridkey    = "xxxxxxxxxxx";
String passidkey  = "yyyyyyyyyyyyyyyyyyyyyyy";
String deviceid      = "idzzzzzzz";
LOTODA lotoda = LOTODA(useridkey, passidkey, deviceid);

void setup() {
    Serial.begin(115200);
    delay(10);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(50);
        Serial.print(".");
    }
    Serial.println("");

    /**
     * Set the debug printer (optional).
     */
    lotoda.setDebugPrinter(&Serial);
    /**
     * Set LOTODA client.
     */
    lotoda.setConnectClient(&client);
}

void loop() {
    lotoda.loop();

    float temperatureValue = 32.10;
    lotoda.publishSensorValue(temperatureValue);
}