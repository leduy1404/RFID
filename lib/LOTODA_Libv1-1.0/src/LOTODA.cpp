/**
 * LOTODA.cpp - Arduino Library of IoT LOTODA https://lotoda.vn
 */
#include "LOTODA.h"
#include "LOTODA-Config.h"

LOTODA::LOTODA(String useridkey, String passidkey, String deviceid) {
    this->useridkey = useridkey;
    this->passidkey = passidkey;
    this->deviceid = deviceid;
    this->callback = NULL;
}

LOTODA::LOTODA(String useridkey, String passidkey, String deviceid, String nodesecretkey) {
    this->useridkey = useridkey;
    this->passidkey = passidkey;
    this->deviceid = deviceid;
    this->nodesecretkey = nodesecretkey;
    this->callback = NULL;
}

LOTODA& LOTODA::setCallback(LOTODA_CALLBACK_SIGNATURE) {
    this->callback = callback;
    return *this;
}

LOTODA* LOTODA::setDebugPrinter(HardwareSerial* debugPrinter) {
    this->debugPrinter = debugPrinter;
    return this;
}

LOTODA* LOTODA::setConnectClient(Client* client) {
    this->_client = client;
    this->mqttClient = MqttClient(client);
    return this;
}

bool LOTODA::loop() {
    this->mqttClient.loop();
    this->mqttConnect();
    return true;
}

bool LOTODA::publishSensorValue(float value) {
    if (nextPublishValue < millis()) {
        debugPrint("publishSensorValue()", __LINE__, "Publish sensor's value to the cloud");
        nextPublishValue = millis() + 10000;
        String topic = String(this->useridkey + "/tele/" + this->deviceid + "/" + SENSOR_FIELD);
        bool result = this->mqttClient.publish(topic.c_str(), String(value).c_str());

        debugPrint("publishSensorValue()", __LINE__, "Response received from the cloud (status " + String(result) + ")");
        return result;
    }
}

bool LOTODA::publishSensorJsonValueOne(char* str1, float value1) {
    if (nextPublishValue < millis()) {
        debugPrint("publishSensorValue()", __LINE__, "Publish sensor's value to the cloud");
        nextPublishValue = millis() + 10000;
        String topic = String(this->useridkey + "/tele/" + this->deviceid + "/" + SENSOR_FIELD);
        char tmpString1[15];
        // float to string
        dtostrf(value1, 1, 2, tmpString1);
        char tmp1[100];
        char key1[100] = "{";
        sprintf(tmp1,"\"%s\":", str1);
        strcat(key1, tmp1);
        char endkey[2] =  "}";
        char str[250];
        snprintf(str, sizeof(str), "%s%s%s", key1, tmpString1, endkey); //json message
        // Serial.println(str);
        bool result = this->mqttClient.publish(topic.c_str(), String(str).c_str());
        debugPrint("publishSensorValue()", __LINE__, "Response received from the cloud (status " + String(result) + ")");
        return result;
    }
}

bool LOTODA::publishSensorJsonValueTwo(char* str1, float value1, char* str2, float value2) {
    if (nextPublishValue < millis()) {
        debugPrint("publishSensorValue()", __LINE__, "Publish sensor's value to the cloud");
        nextPublishValue = millis() + 10000;
        String topic = String(this->useridkey + "/tele/" + this->deviceid + "/" + SENSOR_FIELD);
        char tmpString1[15];
        char tmpString2[15];
        // float to string
        dtostrf(value1, 1, 2, tmpString1);
        dtostrf(value2, 1, 2, tmpString2);
        char tmp1[100];
        char tmp2[100];
        char key1[100] = "{";
        sprintf(tmp1,"\"%s\":", str1);
        strcat(key1, tmp1);
        char key2[100] = ",";
        sprintf(tmp2,"\"%s\":", str2);
        strcat(key2, tmp2);
        char endkey[2] =  "}";
        char str[250];
        snprintf(str, sizeof(str), "%s%s%s%s%s", key1, tmpString1, key2, tmpString2, endkey); //json message
        // Serial.println(str);
        bool result = this->mqttClient.publish(topic.c_str(), String(str).c_str());
        debugPrint("publishSensorValue()", __LINE__, "Response received from the cloud (status " + String(result) + ")");
        return result;
    }
}

bool LOTODA::getControlRelay()
{
    mqttClient.setCallback([this] (char* topic, unsigned char* payload, unsigned int length) {
        this->mqttCallbackControlRelay(topic, payload, length);
        });
    if (mqttClient.connect(this->deviceid.c_str(), this->useridkey.c_str(), this->passidkey.c_str())) {
        return true;
    }else
    {
        debugPrint("mqttConnect()", __LINE__, "Connection failed, rc=" + String(mqttClient.state()));
        return false;
    }
}
void LOTODA::debugPrint(String function, int line, String msg) {
    if (this->debugPrinter) {
        debugPrinter->print(millis());
        debugPrinter->print(": {");
        debugPrinter->print(function);
        debugPrinter->print(":");
        debugPrinter->print(line);
        debugPrinter->print("} - ");
        debugPrinter->println(msg);
    }
}

bool LOTODA::mqttConnect() {
    if (mqttClient.connected()) {
       return true;
    }
    debugPrint("mqttConnect()", __LINE__, "Send MQTT connection request to the cloud");
    debugPrint("mqttConnect()", __LINE__, "MQTT clientId: " + this->deviceid);
    mqttClient.setServer(LOTODA_MQTT_HOST, 1883);
    mqttClient.setCallback([this] (char* topic, unsigned char* payload, unsigned int length) {
        this->mqttCallback(topic, payload, length);
        });
    if (mqttClient.connect(this->deviceid.c_str(), this->useridkey.c_str(), this->passidkey.c_str())) {
        String topic = String(this->useridkey + "/cmnd/" + this->deviceid + "/#");
        debugPrint("mqttConnect()", __LINE__, "Connected and subscribed to the '" + topic + "' topic.");
        mqttClient.subscribe(topic.c_str());
        return true;
    }

    debugPrint("mqttConnect()", __LINE__, "Connection failed, rc=" + String(mqttClient.state()));
    return false;
}

bool LOTODA::mqttCallback(char* topic, unsigned char* message, unsigned int length)
{
    char payloadChars[length + 1];
    for (int i = 0; i < length; i++) {
        payloadChars[i] = (char)message[i];
    }
    payloadChars[length] = '\0';
    String payload = String(payloadChars);

    debugPrint("mqttCallback()", __LINE__, "MQTT payload [" + payload + "] arrived on the [" + topic + "] topic");
    return true;
}

bool LOTODA::mqttCallbackControlRelay(char* topic, unsigned char* message, unsigned int length)
{
    bool publishState = true;
    char payloadChars[length + 1];
    char pos = 0;
    char mod_topic[100] = {0};
    char count = 0;
    for (int i = 0; i < length; i++) {
        payloadChars[i] = (char)message[i];
    }
    payloadChars[length] = '\0';
    String payload = String(payloadChars);
    debugPrint("mqttCallbackControlRelay()", __LINE__, "MQTT payload [" + payload + "] arrived on the [" + topic + "] topic");
    for (int i=0; i< strlen(topic); i++)
    {
        if(topic[i]=='/')
        {
            pos=i;
        }
    }
    for (int i = pos + 1; i < strlen(topic); i++)
    {
        mod_topic[i - pos - 1] = topic[i];
    }
    for (uint8_t i = 0; i < _Device.noOfRelays; i++) {
        if (_Relay[i].name ==  String(mod_topic)){
            String j = String(i + 1);
            if (payload == "1") {
                _Relay[i].state.powerOnStr = "1";
                off(i);
                _Relay[i].triggerSignal == RELAY_SIGNAL_TRIGGER_HIGH;
                debugPrint("mqttCallbackControlRelay()", __LINE__, "RELAY" + j + " = ON");
            } else if (payload == "0") {
                _Relay[i].state.powerOnStr = "0";
                on(i);
                _Relay[i].triggerSignal == RELAY_SIGNAL_TRIGGER_HIGH;
                debugPrint("mqttCallbackControlRelay()", __LINE__, "RELAY" + j + " = OFF");
            } else if (payload == "g" && length == 3) {
            } else {
                publishState = false;
            }
            if (publishState) {
                String relaytopic = String(this->useridkey + "/" + STAT + "/" + this->deviceid + "/" + _Relay[i].name);
                mqttClient.publish(relaytopic.c_str(), _Relay[i].state.powerOnStr.c_str());
                debugPrint("stateControlRelay()", __LINE__, "Publish state of RELAYs on the [" + relaytopic + "] topic to the cloud");
            }
        }
    }
    return true;
}

void LOTODA::stateControlRelay(int id) {
    this->getControlRelay();
    if (nextpublishUpdate < millis()) {
        for (uint8_t i = 0; i < _Device.noOfRelays; i++) {
            nextpublishUpdate = millis() + 10000;
            String relaytopic = String(this->useridkey + "/" + TELE + "/" + this->deviceid + "/" + _Relay[i].name);
            mqttClient.publish(relaytopic.c_str(), _Relay[i].state.powerOnStr.c_str());
            debugPrint("stateControlRelay()", __LINE__, "Publish state of RELAYs on the [" + relaytopic + "] topic to the cloud");
        }
    }
}

void LOTODA::stat(String relaytopic, String relaystate) {
    mqttClient.publish(relaytopic.c_str(), relaystate.c_str());
}