/****************************************************
 * Project: ESP32 REST API Sensor Server
 * Author: Sumith D
 * Year: 2025
 *
 * Licensed under the MIT License.
 * You may use, modify, and distribute this code freely,
 * but you must give credit to Author (Sumith D).
 ****************************************************/
// 
// ESP32 Board Connections
// 
//           +3.3V
//            │
//            │
//            ├─────> DHT11 VCC
//            │
//            ├─────> LDR ──┐
//            │             │
//            │            R 10kΩ
//            │             │
//            │            GND
//            │
//            ├─────> PIR VCC
//            │
//            └─────> OLED VCC
// 
// GND ──────────────> DHT11 GND
// GND ──────────────> PIR GND
// GND ──────────────> OLED GND
// 
// DATA Lines:
// DHT11 DATA ──> GPIO5
// PIR OUT   ──> GPIO2
// LDR middle point ──> GPIO32
// OLED SDA line ──> GPIO21
// OLED SCL line ──> GPIO22
// 


// OLED modules :)
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// wifi module :)
#include <WiFi.h>

// Other Sensors module
#include <DHT.h>


// global variables
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 

#define MOTION_PIN 2
#define PHOTO_PIN 32
#define DHT_PIN 5

#define DHT_TYPE DHT11


// wifi creds
const char* ssid = "wifi_name";
const char* password = "wifi_password";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHT_PIN, DHT_TYPE);
WiFiServer server(80);


// Helper functions

float readTemperature() {
  // in degree
  float t = dht.readTemperature();

  if (isnan(t)) {
    Serial.println("DHT temp read error");
    return -999;
  }
  return t;
}

float readHumidity() {

  float h = dht.readHumidity();

  if (isnan(h)) {
    Serial.println("DHT humidity read error");
    return -999;
  }
  return h;
}

int readPhotoSensor() {
  return analogRead(PHOTO_PIN);
}

bool readMotionSensor() {
  return digitalRead(MOTION_PIN) == HIGH;
}


void setup() {
  Serial.begin(115200);

  // pins initialization
  dht.begin();
  pinMode(MOTION_PIN, INPUT);
  pinMode(PHOTO_PIN, INPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  // display configs
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,10);
  display.setTextWrap(true);

  // console and OLED message logs
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  display.print("Connecting to wifi");
  display.display();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }

  display.clearDisplay();
  display.println("Wifi Connected!!");
  display.display();
  delay(2000);
  
  display.clearDisplay();
  display.println("Server Online on IP: ");
  display.println(WiFi.localIP());
  display.display();


  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Start core functions
  server.begin();
}


void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  while (!client.available()) delay(1);

  String req = client.readStringUntil('\r');
  Serial.println(req);

  while (client.available()) client.read();

  String json = "{}";
  int statusCode = 200;


  // Endpoints
  
  if (req.startsWith("GET / ")) {
    json = "{\"status\":\"online\"}";
  }
  else if (req.startsWith("GET /temp")) {
    float temperature = readTemperature();
    float humidity    = readHumidity();

    json = "{";
    json += "\"temperature\":" + String(temperature, 2) + ",";
    json += "\"humidity\":" + String(humidity, 2);
    json += "}";
  }
  else if (req.startsWith("GET /photo")) {
    int lightValue = readPhotoSensor();

    json = "{";
    json += "\"light\":" + String(map(readPhotoSensor(), 0, 4095, 0, 100));
    json += "}";
  }
  else if (req.startsWith("GET /motion")) {
    bool motion = readMotionSensor();

    json = "{";
    json += "\"motion\":" + String(motion ? "true" : "false");
    json += "}";
  }
  else {
    statusCode = 404;
    json = "{\"error\":\"unknown endpoint\"}";
  }

  // ---- SEND JSON RESPONSE ----
  client.printf("HTTP/1.1 %d OK\r\n", statusCode);
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *"); // allow browser access
  client.println("Connection: close");
  client.println();
  client.println(json);

  delay(1);
  client.stop();
}

