////////////////////////////////////////////////////////////////////////////////////////////////////
// Chương trình ESP đọc Soil Humidity Sensor và gửi dữ liệu lên ESP Master
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include "PubSubClient.h"

const char* ssid = "Khong Biet";              // Tên Wifi
const char* password = "0964231520";       // Mật khẩu Wifi
String mqtt_server = "broker.hivemq.com";  // Server MQTT
int mqtt_port = 1883;                      // Port MQTT
String client_name = "esp8266-soil-moisture-sensor";      // Mỗi ESP một ClientName tránh trùng lặp
String topic_sub = "esp8266-soil-moisture-sensor/pub";
String topic_pub = "esp8266-soil-moisture-sensor/sub";
WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastime_reconnect_mqtt = 0;
unsigned long lastime_response_server = 0;

float soil_moisture = 0;

String createMessage() {
  JSONVar myObject;
  myObject["soil_moisture"] = soil_moisture;
  String jsonString = JSON.stringify(myObject);
  return jsonString;
}

// Reconnect tới Server MQTT khi mất kết nối
void reconnectMQTT() {
  if (!mqtt.connected() && (millis() - lastime_reconnect_mqtt > 2000)) {  // Timer sau 2000ms reconnect MQTT
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect(client_name.c_str())) {  // Thực hiện kết nối tới Server MQTT và kiểm tra đã kết nối hay chưa
      Serial.println("connected");
      mqtt.subscribe(topic_sub.c_str());
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

//Hàm gửi dữ liệu đi
void sendData() {
  if (millis() - lastime_response_server > 200) {  // Timer cho sau khoảng 1000ms gửi dữ liệu 1 lần
    int value = analogRead(A0);
    soil_moisture =100- map(value, 0, 1023, 0, 100);
    lastime_response_server = millis();
    mqtt.publish(topic_pub.c_str(), createMessage().c_str());  // Gửi dữ liệu lên Server MQTT
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  mqtt.setServer(mqtt_server.c_str(), mqtt_port);  // Set địa chỉ server cho mqtt
}

void loop() {
  
  if (mqtt.connected()) {
    sendData();
  } else {
    reconnectMQTT();
  }
  mqtt.loop();
}
