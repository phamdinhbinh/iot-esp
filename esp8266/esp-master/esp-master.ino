////////////////////////////////////////////////////////////////////////////////////////////////////
// Chương trình ESP Master điều khiển Relay và thu thập dữ liệu cảm biến từ ESP Soil Humidity
// và gửi dữ liệu lên web
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "PubSubClient.h"

#define RELAY_1 2
#define RELAY_2 16

String serverName = "http://172.20.10.6:4000/api/history-data/add";
// String serverName = "http://192.168.1.2:1880/test";

unsigned long lastTimeCallApi = 0;
unsigned long timerDelayCallApi = 60000;
unsigned long lastTimeUpdateRTC = 0;
unsigned long timerDelayUpdateRTC = 1000;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
String currentDateTime = "2000-01-01 00:00:00";

const char* ssid = "Khong Biet";              // Tên Wifi
const char* password = "0964231520";       // Mật khẩu Wifi 
String mqtt_server = "broker.hivemq.com";  // Server MQTT
int mqtt_port = 1883;                      // Port MQTT
String client_name = "esp8266-master";      // Mỗi ESP một ClientName tránh trùng lặp
String topic_light_sensor_sub = "esp8266-light-sensor/sub";
String topic_soil_moisture_sub = "esp8266-soil-moisture-sensor/sub";
String topic_master_sub = "esp8266-master/sub";
String topic_master_pub = "esp8266-master/pub";
WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastime_reconnect_mqtt = 0;
unsigned long lastime_response_server = 0;
unsigned long lastime_send_to_esp_relay = 0;

float temp = 0;
float humi = 0;
float soil_moisture = 0;
float light_sensor = 0;
int relay_1 = 0;
int relay_2 = 0;

float temp_setting_value = 0;
float soil_moisture_setting_value = 0;
int mode_control = 0;  //0: is manual | 1: is auto

String createMessage() {
  JSONVar myObject;
  myObject["temp"] = temp;
  myObject["humi"] = humi;
  myObject["soil_moisture"] = soil_moisture;
  myObject["light"] = light_sensor;
  myObject["date_time"] = currentDateTime;
  myObject["relay_1"] = relay_1;
  myObject["relay_2"] = relay_2;
  myObject["mode_control"] = mode_control;
  myObject["temp_setting_value"] = temp_setting_value;
  myObject["soil_moisture_setting_value"] = soil_moisture_setting_value;
  String jsonString = JSON.stringify(myObject);
  return jsonString;
}

String creatPayloadLog() {
  JSONVar myObject;
  myObject["temp"] = temp;
  myObject["humi"] = humi;
  myObject["soil_moisture"] = soil_moisture;
  myObject["light"] = light_sensor;
  myObject["date_time"] = currentDateTime;
  String jsonString = JSON.stringify(myObject);
  return jsonString;
}

void logDataSensor() {
  if ((millis() - lastTimeCallApi) > timerDelayCallApi) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverName);
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // String httpRequestData = createMessage();
      // int httpResponseCode = http.POST(httpRequestData);
      String httpRequestData = creatPayloadLog();
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(httpRequestData);

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTimeCallApi = millis();
  }
}

void updateTime() {
  if (millis() - lastTimeUpdateRTC > timerDelayUpdateRTC) {
    lastTimeUpdateRTC = millis();
    timeClient.update();
    time_t epochTime = timeClient.getEpochTime();
    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();
    int currentSecond = timeClient.getSeconds();
    //Get a time structure
    struct tm* ptm = gmtime((time_t*)&epochTime);
    int monthDay = ptm->tm_mday;
    int currentMonth = ptm->tm_mon + 1;
    int currentYear = ptm->tm_year + 1900;

    currentDateTime = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay) + " " + String(currentHour) + ":" + String(currentMinute) + ":" + String(currentSecond);
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";
  if (strstr(topic, topic_soil_moisture_sub.c_str()) != null) {
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    JSONVar myObject = JSON.parse(message);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    if (myObject.hasOwnProperty("soil_moisture")) {
      soil_moisture = (double)myObject["soil_moisture"];
    }
  }

  if (strstr(topic, topic_light_sensor_sub.c_str()) != null) {
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    JSONVar myObject = JSON.parse(message);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    if (myObject.hasOwnProperty("light_sensor")) {
      light_sensor = (double)myObject["light_sensor"];
    }
    if (myObject.hasOwnProperty("temp")) {
      temp = (double)myObject["temp"];
    }
    if (myObject.hasOwnProperty("humi")) {
      humi = (double)myObject["humi"];
    }
  }

  if (strstr(topic, topic_master_sub.c_str()) != null) {
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    Serial.println(message);
    JSONVar myObject = JSON.parse(message);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    if (myObject.hasOwnProperty("mode-control")) {
      mode_control = (int)myObject["mode-control"];
    }
    if (myObject.hasOwnProperty("temp_setting_value")) {
      temp_setting_value = (double)myObject["temp_setting_value"];
    }
    if (myObject.hasOwnProperty("soil_moisture_setting_value")) {
      soil_moisture_setting_value = (double)myObject["soil_moisture_setting_value"];
    }
    if (mode_control == 0) {
      if (myObject.hasOwnProperty("relay_1")) {
        relay_1 = (int)myObject["relay_1"]  ;
      }
      if (myObject.hasOwnProperty("relay_2")) {
        relay_2 = (int)myObject["relay_2"]  ;
      }
    }
    
    if (mode_control == 2) {
      if (myObject.hasOwnProperty("relay_1_stage")) {
        relay_1 = (int)myObject["relay_1_stage"]  ;
      }
      if (myObject.hasOwnProperty("relay_2_stage")) {
        relay_2 = (int)myObject["relay_2_stage"]  ;
      }
    }
    sendData(0);
  }
}

// Reconnect tới Server MQTT khi mất kết nối
void reconnectMQTT() {
  if (!mqtt.connected() && (millis() - lastime_reconnect_mqtt > 2000)) {  // Timer sau 2000ms reconnect MQTT
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect(client_name.c_str())) {  // Thực hiện kết nối tới Server MQTT và kiểm tra đã kết nối hay chưa
      Serial.println("connected");
      mqtt.subscribe(topic_light_sensor_sub.c_str());
      mqtt.subscribe(topic_soil_moisture_sub.c_str());
      mqtt.subscribe(topic_master_sub.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
    }
    lastime_reconnect_mqtt = millis();
  }

  if (mqtt.connected()) {
    lastime_reconnect_mqtt = millis();
  }
}

//Hàm gửi dữ liệu lên server
void sendData(int interval) {
  if (millis() - lastime_response_server > interval) {  // Timer cho sau khoảng 1000ms gửi dữ liệu 1 lần
    lastime_response_server = millis();
    mqtt.publish_P(topic_master_pub.c_str(), createMessage().c_str(), false);  // Gửi dữ liệu lên Server MQTT
  }
}

void autoMode() {
  if (temp < temp_setting_value + 2) {
  relay_2 = 0;
    }
  if (temp > temp_setting_value) {
    relay_2 = 1;
  } 


  if (soil_moisture > soil_moisture_setting_value +3 ) {
    relay_1 = 0;
  }
   if (soil_moisture < soil_moisture_setting_value) {
    relay_1 = 1;
  }
}


void logicControl() {
  if (mode_control == 1) {
    autoMode();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.setTimeOffset(7 * 3600);

  mqtt.setServer(mqtt_server.c_str(), mqtt_port);  // Set địa chỉ server cho mqtt
  mqtt.setCallback(callback);
}

void loop() {
  digitalWrite(RELAY_1, relay_1);
  digitalWrite(RELAY_2, relay_2);
  updateTime();
  logDataSensor();

  if (mqtt.connected()) {
    sendData(500);
  } else {
    reconnectMQTT();
  }
  mqtt.loop();
  logicControl();
}
