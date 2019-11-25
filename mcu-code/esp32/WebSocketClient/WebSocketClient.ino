#include <ArduinoJson.h>

#include <LXESP32DMX.h>
#include "esp_task_wdt.h"

#define DMX_DIRECTION_PIN 21
#define DMX_SERIAL_OUTPUT_PIN 17

#include <WiFi.h>
#include <WebSocketClient.h>
 
const char* ssid     = "Interactive";
const char* password = "wecanbuildit";

//glass matrix size
const int matrixSizeX = 2;
const int matrixSizeY = 2;
 
char path[] = "/";
char host[] = "192.168.1.135";
 
WebSocketClient webSocketClient;
WiFiClient client;

uint8_t dmxbuffer[DMX_MAX_FRAME];

//for json parsing
StaticJsonDocument<58> doc;
 
void setup() {
  Serial.begin(115200);

  pinMode(DMX_DIRECTION_PIN, OUTPUT);
  digitalWrite(DMX_DIRECTION_PIN, HIGH);

  pinMode(DMX_SERIAL_OUTPUT_PIN, OUTPUT);
  ESP32DMX.startOutput(DMX_SERIAL_OUTPUT_PIN);
  Serial.println("dmx setup complete");
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  delay(5000);
 
  if (client.connect(host, 80)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
 
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
 
}
 
void loop() {
  String data;
 
  if (client.connected()) {
    webSocketClient.getData(data);
    if (data.length() > 0) {
      Serial.print("Received data: ");
      Serial.println(data);
      //parse data and send over dmx
      parseData(data);
    }
 
  } else {
    Serial.println("Client disconnected.");
    delay(10000);
  }
  
  delay(100);
}

void parseData(String json){
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  int x = doc["x"];
  int y = doc["y"];
  x++;
  y++;
  bool state = doc["state"];

  dmxbuffer[x * matrixSizeX + y] = (state ? 1 : 0);

  Serial.print("dmx: ");
  Serial.print(x * matrixSizeX + y);
  Serial.print(" = ");
  Serial.println(state ? 255 : 0);

  copyDMXToOutput();

  delay(100);
}

void copyDMXToOutput(void) {
  xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
  for (int i=1; i<DMX_MAX_FRAME; i++) {
      ESP32DMX.setSlot(i , dmxbuffer[i]);
   }
   xSemaphoreGive( ESP32DMX.lxDataLock );
}
