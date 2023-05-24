////////////////////////////////////////////////////////////////////////////////////////////////////
// Chương trình ESP gửi giá trị cảm biến Light, Temp, Humi về ESP Master
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <Arduino_JSON.h>
#include "DHT.h"

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Khong Biet";              // Tên Wifi
const char* password = "0964231520";       // Mật khẩu Wifi
String mqtt_server = "broker.hivemq.com";  // Server MQTT
int mqtt_port = 1883;                      // Port MQTT
String client_name = "esp8266-light-sensor";      // Mỗi ESP một ClientName tránh trùng lặp
String topic_sub = "esp8266-light-sensor/pub";
String topic_pub = "esp8266-light-sensor/sub";
WiFiClient espClient;
PubSubClient mqtt(espClient);
unsigned long lastime_reconnect_mqtt = 0;
unsigned long lastime_response_server = 0;
float temp = 0;
float humi = 0;
float light_sensor = 0;

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

//Hàm nhận dữ liệu khi có Message được gửi đến ESP
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
}

String createMessage() {
  JSONVar myObject;
  myObject["light_sensor"] = light_sensor;
  myObject["temp"] = temp;
  myObject["humi"] = humi;
  String jsonString = JSON.stringify(myObject);
  return jsonString;
}

//Hàm gửi dữ liệu đi
void sendData() {
  if (millis() - lastime_response_server > 200) {  // Timer cho sau khoảng 1000ms gửi dữ liệu 1 lần
    lastime_response_server = millis();
//    temp = random(20, 40);
//    humi = random(80, 100);
//    light_sensor = random(0, 100);
    humi = dht.readHumidity();
    temp = dht.readTemperature();
   int value = analogRead(A0);
  light_sensor =100- map(value, 0, 1023, 0, 100);
    mqtt.publish(topic_pub.c_str(), createMessage().c_str());  // Gửi dữ liệu lên Server MQTT
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);         // Chọn chế độ wifi là Station
  WiFi.begin(ssid, password);  // Bắt đầu kết nối wifi với tên và password đã khai báo
  Serial.println("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {  // Kiểm tra kết nối và đợi đến khi wifi kết nối thành công
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi...");
  Serial.println(WiFi.localIP());  // In ra địa chỉ IP của ESP

  mqtt.setServer(mqtt_server.c_str(), mqtt_port);  // Set địa chỉ server cho mqtt
  mqtt.setCallback(callback);                      // set hàm nhận tin nhắn từ mqtt server
}

void loop() {
  if (mqtt.connected()) {
    sendData();
  } else {
    reconnectMQTT();
  }
  mqtt.loop();
}
