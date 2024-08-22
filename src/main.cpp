#include <Wire.h>
#include <LOTODA.h>
#include <LOTODA-Config.h>
#include <ArduinoJson.h> 
#include <PubSubClient.h> 
#include <WiFiManager.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "FS.h"
#include <LITTLEFS.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial mySerial(16, 17);

const int RFID_LENGTH = 14;
byte buffer[RFID_LENGTH];
int bufIndex = 0;

struct UIDCount {
  byte uid[RFID_LENGTH];
  int count;
};

UIDCount uidCounts[100];
int numUIDs = 0;

AsyncWebServer server(80);
WiFiManager wifiManager;


char LotodaServer[] = "app.lotoda.vn"; 
int LotodaPort = 1883; 
char deviceId[] = "LotodaEsp8266-0001"; 
char topic[] = "Y9L4MytvWM/RFID"; 
char User_ID_Key[] = "Y9L4MytvWM"; 
char Password_ID_Key[] = "xY1SI02Y9Oi2USoD25Fb"; 
WiFiClient wifiClient; 
PubSubClient client(wifiClient); 

//long lastMsg = 0; 
//char msg[50]; 
//int value = 0;


void callback(char* topic, byte* payload, unsigned int length) { 
 
  Serial.print("Message arrived ["); 
  Serial.print(topic); 
  Serial.print("] "); 
  for (int i = 0; i < length; i++) { 
    Serial.print((char)payload[i]); 
  } 
  Serial.println(); 
} 

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(deviceId, User_ID_Key, Password_ID_Key)) {
      Serial.println("connected");
      client.subscribe("RFID"); 
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
    }
  }
}


void saveWiFiCredentials(const char* ssid, const char* password) {
  File file = LITTLEFS.open("/wificredentials.txt", "w");
  if (!file) {
    Serial.println("Không thể mở file để ghi!");
    return;
  }
  file.println(ssid);
  file.println(password);
  file.close();
}


void loadWiFiCredentials(char* ssid, char* password) {
  File file = LITTLEFS.open("/wificredentials.txt", "r");
  if (!file) {
    Serial.println("Không thể mở file để đọc, tạo file mới với giá trị mặc định.");
   
    strcpy(ssid, "");
    strcpy(password, "");

  
    file = LITTLEFS.open("/wificredentials.txt", "w");
    if (file) {
      file.println(ssid);
      file.println(password);
      file.close();
      Serial.println("Tạo file thành công.");
    } else {
      Serial.println("Không thể tạo file!");
    }
    return;
  }

  String ssidStr = file.readStringUntil('\n');
  String passStr = file.readStringUntil('\n');
  ssidStr.trim();
  passStr.trim();
  ssidStr.toCharArray(ssid, ssidStr.length() + 1);
  passStr.toCharArray(password, passStr.length() + 1);
  file.close();
}


void connectToWiFi() {
  char ssid[32];
  char password[32];
  loadWiFiCredentials(ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Kết nối thành công với SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());
}

String generateWebPage() {
  String content = "<html><head><meta charset='UTF-8'>";
  content += "<link rel='stylesheet' type='text/css' href='/style.css'>";
  content += "<script>";
  content += "function fetchData() {";
  content += "fetch('/data').then(response => response.json()).then(data => {";
  content += "console.log('Dữ liệu nhận được từ máy chủ:', data);";
  content += "let content = '<h1>Số lần quét UID</h1><ul>';";
  content += "data.forEach(item => {";
  content += "content += '<li>UID: ' + item.uid + ' - Số lần quét: ' + item.count + '</li>';";
  content += "});";
  content += "content += '</ul>';";
  content += "document.getElementById('data').innerHTML = content;";
  content += "});";
  content += "}";
  content += "setInterval(fetchData, 1000);";
  content += "</script></head><body onload='fetchData()'>";
  content += "<div id='data'><h1>Loading...</h1></div>";
  content += "<form action='/changeWiFi' method='POST'>";
  content += "  SSID: <input type='text' name='ssid'><br>";
  content += "Password: <input type='password' name='password'><br>";
  content += "<input type='submit' value='Thay đổi WiFi'>";
  content += "</form>";
  content += "<img src='https://4.imimg.com/data4/LO/FT/MY-12701339/uhf-rfid-reader-500x500.jpg' alt='RFID Reader Image'>";
  content += "</body></html>";
  return content;
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.print("Ready");
  Serial.println("Ready");

  if (!LITTLEFS.begin()) {
    Serial.println("Không thể khởi động LittleFS");
    return;
  }

  wifiManager.setConfigPortalTimeout(20); 
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    Serial.println("Kết nối thất bại, khởi động lại ESP");
    delay(3000);
    ESP.restart();
  }

  connectToWiFi();

  client.setServer(LotodaServer, LotodaPort); 
  client.setCallback(callback); 

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", generateWebPage());
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "[";
    for (int j = 0; j < numUIDs; j++) {
      if (j > 0) {
        json += ",";
      }
      json += "{\"uid\":\"";
      for (int i = 0; i < RFID_LENGTH; i++) {
        json += String(uidCounts[j].uid[i], HEX);
        if (i < RFID_LENGTH - 1) {
          json += " ";
        }
      }
      json += "\", \"count\":" + String(uidCounts[j].count) + "}";
    }
    json += "]";
    request->send(200, "application/json", json);
  });

  server.on("/changeWiFi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
    saveWiFiCredentials(ssid.c_str(), password.c_str());
    request->send(200, "text/html", "Thay đổi WiFi thành công! Đang kết nối lại...");
    ESP.restart();
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LITTLEFS, "/style.css", "text/css");
  });

  server.begin();
  Serial.println("Web server bắt đầu!");
}

const unsigned long debounceDelay = 200; 
unsigned long lastReadTime = 0;

void displayData() {

  unsigned long currentMillis = millis();
  if (currentMillis - lastReadTime < debounceDelay) {
    return; 
  }
  lastReadTime = currentMillis;

  lcd.clear();
  lcd.setCursor(0, 0);

  Serial.print("UID của thẻ: ");
  String uidString = "";

  for (int i = 0; i < RFID_LENGTH; i++) {
    lcd.print(buffer[i], HEX);
    Serial.print(buffer[i], HEX);
    Serial.print(' ');
    uidString += String(buffer[i], HEX);
    if (i == 7) {
      lcd.setCursor(0, 1);
    }
  }
  Serial.println();

  bool newUID = true;
  int uidIndex = -1;
  for (int j = 0; j < numUIDs; j++) {
    bool check = true;
    for (int i = 0; i < RFID_LENGTH; i++) {
      if (buffer[i] != uidCounts[j].uid[i]) {
        check = false;
        break;
      }
    }
    if (check) {
      uidCounts[j].count++;
      Serial.print(" Số lần quét: ");
      Serial.println(uidCounts[j].count);
      lcd.setCursor(10, 1);
      lcd.print("Lan:");
      lcd.print(uidCounts[j].count);
      newUID = false;
      uidIndex = j;
      break;
    }
  }

  if (newUID) {
    if (numUIDs < 100) {
      memcpy(uidCounts[numUIDs].uid, buffer, RFID_LENGTH);
      uidCounts[numUIDs].count = 1;
      numUIDs++;
      uidIndex = numUIDs - 1;
      Serial.print(" Số lần quét: ");
      Serial.println(1);
      lcd.setCursor(10, 1);
      lcd.print("Lan:");
      lcd.print(1);
    }
  }

  StaticJsonDocument<256> jsonDocument;
  String jsonString;

  if (uidIndex >= 0) {
    String uidValue = "";
    for (int i = 0; i < RFID_LENGTH; i++) {
      uidValue += String(uidCounts[uidIndex].uid[i], HEX);
      if (i < RFID_LENGTH - 1) {
        uidValue += " ";
      }
    }

    jsonDocument["UID"] = uidValue;
    jsonDocument["Count"] = uidCounts[uidIndex].count;

    serializeJson(jsonDocument, jsonString);

    Serial.print("Publish message: ");
    Serial.println(jsonString);

    if (client.publish(topic, jsonString.c_str())) {
      Serial.println("Dữ liệu đã được xuất bản thành công lên MQTT.");
    } else {
      Serial.println("Lỗi khi xuất bản dữ liệu lên MQTT.");
    }
  }
}

void loop() {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (mySerial.available() > 0) {
    buffer[bufIndex] = mySerial.read();
    bufIndex++;

    if (bufIndex >= RFID_LENGTH) {
      displayData();
      bufIndex = 0;
    }
  }
  delay(100);
}
