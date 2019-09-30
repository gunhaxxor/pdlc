#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid     = "Interactive";
const char* password = "wecanbuildit";

//glass matrix size
const int matrixSizeX = 5;
const int matrixSizeY = 5;

const int serial1Id = 0;
const int serial2Id = 1;

char path[] = "/";
char host[] = "192.168.1.135";

WebSocketClient webSocketClient;
WiFiClient client;

//for json parsing
StaticJsonDocument<64> doc;

void setup() {
  Serial.begin(115200);
  //Serial1.begin(9600);
  Serial2.begin(9600);

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
      //parse data and send over com port
      parseData(data);
    }

  } else {
    Serial.println("Client disconnected.");
    delay(10000);
  }
  delay(10);
}

void parseData(String json) {
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  int id = doc["id"];
  unsigned int values = doc["values"];

  if (id == serial1Id) {
    char str[20];
    sprintf(str, "%08x", values);
    //Serial1.println((String)"H" + str);
    Serial.println(str);
  } else if(id == serial2Id) {
    char str[32];
    sprintf(str, "%08x", values);
    //printf("H%04x%04x", valuepart1, valuesPart2);
    Serial2.println((String)"H" + str);
    Serial.println((String)"H" + str);
  }

}
