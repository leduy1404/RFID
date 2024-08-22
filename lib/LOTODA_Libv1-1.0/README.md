# The IoT LOTODA Arduino library

## Summary

This is an Arduino library for ESP8266/ESP32 to provide integration with [The IoT LOTODA](https://lotoda.vn) services.

- Support forum: [https://groups.google.com/g/lotodausers](https://groups.google.com/g/lotodausers)
- Facebook page: [https://www.facebook.com/lotodavn](https://www.facebook.com/lotodavn)
- Youtube page: [https://www.youtube.com/channel/UCgq4QVD5QAAoXpXEfbVfiWw](https://www.youtube.com/channel/UCgq4QVD5QAAoXpXEfbVfiWw)
- Website page: [https://lotoda.vn](https://lotoda.vn)

## Examples

- MQTT Client send and receive messages: [01_mqtt_client.ino](https://github.com/lotoda/LOTODA_Lib/blob/main/examples/01_mqtt_client/01_mqtt_client.ino)
- MQTT and relay: [02_mqtt_relay.ino](https://github.com/lotoda/LOTODA_Lib/blob/main/examples/02_mqtt_relay/02_mqtt_relay.ino)
- AM2301 sensor and DHT sensor: [03_am2301_dht.ino](https://github.com/lotoda/LOTODA_Lib/blob/main/examples/03_am2301_dht/03_am2301_dht.ino)
- And more ...

## Constructors

```LOTODA(String useridkey, String passidkey, String deviceid);```

```LOTODA(String useridkey, String passidkey, String deviceid, String nodesecretkey);```

## Methods

### Set callback function

```LOTODA& setCallback( void (*callback)(char*, uint8_t*, unsigned int) );```

```boolean mqttCallback(char* topic, unsigned char* message, unsigned int length);```

```boolean mqttCallbackControlRelay(char* topic, unsigned char* message, unsigned int length);```


### Set debug printer

```LOTODA* setDebugPrinter(HardwareSerial* debugPrinter);```


### Set network client

```LOTODA* setConnectClient(Client* client);```


### Loop

```boolean loop();```


### Publish float value/JSON format value

```boolean publishSensorValue(float value);```

```boolean publishSensorJsonValueOne(char* str, float value);```

```boolean publishSensorJsonValueTwo(char* str1, float value1, char* str2, float value2);```

### Set/Get Relay

```void setControlRelay(int id);```

```void getControlRelay(int id);```

```void off(int id);```

```void on(int id);```

