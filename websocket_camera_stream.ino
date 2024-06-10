#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int vFlip = 0; // 1=上下翻轉
int hMirror = 0; // 1=左右翻轉
char* ssid = "xu5546223";
char* password = "12345678";
String websocket_server_host = "20.205.139.170";
String websock_name = "/cam1"; 
int websocket_server_port = 1880;
long wsLastRecvTime;
long wsLastRecvInterval = 10000;
const unsigned long imageSendInterval = 50; // 100毫秒间隔（10 FPS）

using namespace websockets;
WebsocketsClient client;
bool isWebSocketConnected = false;
unsigned long lastImageSendTime = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
  Serial.begin(115200);
  setupCam();

  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);

  WiFiConnect();
  webSocketConnect();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFiConnect();
  }
  
  if ((millis() - wsLastRecvTime) >= wsLastRecvInterval) {
    isWebSocketConnected = false;
  }

  if (!isWebSocketConnected) {
    webSocketConnect();
  }

  client.poll();
  if (millis() - lastImageSendTime >= imageSendInterval) {
    SendImageWS();
    lastImageSendTime = millis();
  }
}

void WiFiConnect() {
  Serial.print("開始連線到: "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  int WifiTryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (WifiTryCount++ >= 20) ESP.restart();
  }
  Serial.println("");
  Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connection Opened");
    isWebSocketConnected = true;
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connection Closed");
    isWebSocketConnected = false;
    webSocketConnect();
  }
}

void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
  wsLastRecvTime = millis();
}

void webSocketConnect() {
  Serial.println("Websocket Connecting!");
  while (!client.connect(websocket_server_host, websocket_server_port, websock_name)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");
  isWebSocketConnected = true;
  wsLastRecvTime = millis();
}

void SendImageWS() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  client.sendBinary((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void setupCam() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000; // 增加到20MHz
  config.pixel_format = PIXFORMAT_JPEG; // 使用更高效的GRAYSCALE格式
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 4; // 调整缓冲区数量

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  Serial.println("Camera Setup OK");
}
