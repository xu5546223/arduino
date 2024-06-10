#include <ArduinoJson.h>
#include <Servo.h>
#include "HX711.h"
#include <LiquidCrystal_PCF8574.h>
#include <ArduinoMqttClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA) || defined(ARDUINO_OPTA)
  #include <WiFi.h>
#elif defined(ARDUINO_PORTENTA_C33)
  #include <WiFiC3.h>
#elif defined(ARDUINO_UNOR4_WIFI)
  #include <WiFiS3.h>
#endif

#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
Servo myservo;
LiquidCrystal_PCF8574 lcd(0x27);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.stdtime.gov.tw", 8 * 3600); // UTC time, update every 60 seconds

const int DT_PIN = 7;
const int SCK_PIN = 6;
const int scale_factor = 417;
HX711 scale;
const char broker[] = "test.mosquitto.org";
int port = 1883;
const char topic1[] = "pet/feed/weight";
const char topic2[] = "pet/feed/state";
float weight;
float feedingweight;
unsigned long feedingTime; // Time in minutes (hhmm format)
unsigned long currentTime;
bool feedScheduled = false;
int emptycount = 0;

void setup() {
  myservo.attach(4);
  myservo.write(80);
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Initializing the scale");

  scale.begin(DT_PIN, SCK_PIN);
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.clear();

  // 在 LCD 上顯示初始化訊息
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");

  Serial.println("Before setting up the scale:");
  Serial.println(scale.get_units(5), 0);
  scale.set_scale(scale_factor);
  scale.tare();
  Serial.println("After setting up the scale:");
  Serial.println(scale.get_units(5), 0);
  Serial.println("Readings:");
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting conncet");
    lcd.setCursor(0, 1);
    lcd.print("Please restart");

    delay(1000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // 顯示 WiFi 連接資訊
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MQTT Error");
    lcd.setCursor(0, 1);
    lcd.print("Please restart");

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("MQTT:");
  lcd.setCursor(4, 1);
  lcd.print("Connected");

  Serial.print("Subscribing to topic: ");
  Serial.println(topic1);
  Serial.println();
  mqttClient.subscribe(topic1);
  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic1);
  Serial.println();

  timeClient.begin();
}

void loop() {
  
  mqttClient.poll();
  timeClient.update();
  currentTime = timeClient.getHours() * 100 + timeClient.getMinutes();
  
  emptycount  = 0;
  if (feedScheduled && currentTime == feedingTime) {
    feed();
    feedScheduled = false;//restart
   
  }

  mesureweight();
  parseMessage();
}

void parseMessage() {
  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, mqttClient);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    feedingTime = doc["time"];
    feedingweight = doc["weight"];
    feedScheduled = true;

    Serial.print("Scheduled feeding time: ");
    Serial.println(feedingTime);
    Serial.print("Scheduled feeding weight: ");
    Serial.println(feedingweight);
  }
}

void mesureweight() {
  static unsigned long lastLcdUpdate = 0;
  unsigned long currentMillis = millis();

  Serial.println(scale.get_units(10), 0);

  weight = scale.get_units(10);
  if (weight <= 0) {
    weight = 0;
  }

  scale.power_down();
  delay(1);
  scale.power_up();

  // 更新LCD顯示，每秒更新一次
  if (currentMillis - lastLcdUpdate >= 1000) {
    lastLcdUpdate = currentMillis;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Weight: ");
    lcd.setCursor(9, 0);
    lcd.print(weight, 0);
    lcd.setCursor(13, 0);
    lcd.print("g");
    lcd.setCursor(1, 1);
    lcd.print("State: ");
  }
}

void feed() {
  
  while (feedingweight > weight /*&& emptycount < 4*/) {
    myservo.write(15);  // 打開馬達
    delay(400);  // 適當的延遲時間，以便餵食
    myservo.write(80);  // 關閉馬達

    mesureweight();  // 測量重量
    sendstate();  // 送出狀態
    
  }
  
  feedingweight = 0;
}

void sendstate() {
  const char* state = (feedingweight <= weight) ? "Success" : "Failed";

  if (state == "Failed") {
    emptycount++;
  }

  StaticJsonDocument<256> doc;
  doc["state"] = state;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  mqttClient.beginMessage(topic2);
  mqttClient.print(jsonBuffer);
  mqttClient.endMessage();

  // 更新LCD狀態
  lcd.setCursor(8, 1);
  lcd.print(state);
  if (state == "Failed") {
    lcd.setCursor(15, 1);
    lcd.print(emptycount);
  }

  Serial.print("Sent state: ");
  Serial.println(state);
  Serial.print("feedingweight: ");
  Serial.println(feedingweight);
  Serial.print("weight: ");
  Serial.println(weight);
}