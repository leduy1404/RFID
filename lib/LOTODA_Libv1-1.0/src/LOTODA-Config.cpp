/**
 * LOTODA-Config.h - Arduino Library of IoT LOTODA https://lotoda.vn
 */
#include <LOTODA-Config.h>

void on(int id) {
    digitalWrite(_Relay[id].gpio, _Relay[id].triggerSignal == RELAY_SIGNAL_TRIGGER_HIGH ? HIGH : LOW);
    _Relay[id].state.powerOnStr = "0";
}

void off(int id) {
    digitalWrite(_Relay[id].gpio, _Relay[id].triggerSignal == RELAY_SIGNAL_TRIGGER_HIGH ? LOW : HIGH);
    _Relay[id].state.powerOnStr = "1";
}

void setControlRelay(int id) {
    _Device.noOfRelays = 0;
    for (uint8_t i = 0; i < id; i++) {
        String j = String(i + 1);
        char mqttStateTopic[10];
        strcpy(mqttStateTopic, "POWER");
        strcat(mqttStateTopic, j.c_str());
        _Relay[i].name = String(mqttStateTopic);
        _Relay[i].state.powerOn = LOW;
        _Relay[i].state.powerOnStr = "OFF";
        _Device.noOfRelays ++;
    }
}