#include <Arduino.h>
/**
 *  Example of controlling Relays at IoT LOTOODA
 */
#include <LOTODA.h>
#include <LOTODA-Config.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif

#ifdef ESP32
  #include <WiFi.h>
#endif

/**
 * Relay parameters: NUMBER_OF_RELAYS is from 1 to 3.
 */
#define NUMBER_OF_RELAYS 2
DEVICE _Device;
RELAY _Relay[NUMBER_OF_RELAYS];

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
     * Set the relay pin (is from 1 to 3).
     */
    _Relay[1].gpio = 5;
    pinMode(_Relay[1].gpio, OUTPUT);
    _Relay[2].gpio = 4;
    pinMode(_Relay[2].gpio, OUTPUT);
    setControlRelay(NUMBER_OF_RELAYS);

    /**
     * Set the debug printer (optional).
     */
    lotoda.setDebugPrinter(&Serial);
    /**
     * Set the connect client.
     */
    lotoda.setConnectClient(&client);
}

void loop() {
    lotoda.loop();
    /**
     * State of ControlRelay.
     */
    lotoda.stateControlRelay(NUMBER_OF_RELAYS);
}