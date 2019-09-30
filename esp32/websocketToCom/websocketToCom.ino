#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketClient.h>

const char* ssid     = "Interactive";
const char* password = "wecanbuildit";

//glass matrix size
const int matrixSizeX = 5;
const int matrixSizeY = 5;

char path[] = "/";
char host[] = "192.168.1.135";

WebSocketClient webSocketClient;
WiFiClient client;

//for json parsing
StaticJsonDocument<58> doc;

unsigned long output;

void setup() {
  Serial.begin(115200);
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
  if(millis()%500 < 2){
    printHex(output);
  }
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

  int x = doc["x"];
  int y = doc["y"];
  bool state = doc["state"];

  if (state) {
    output |= (1 << (y * matrixSizeX + x));
  } else {
    output &= ~(1 << (y * matrixSizeX + x ));
  }

}

void printHex(int val) {
  char str[20];
  sprintf(str, "%08x", val);
  Serial2.println((String)"H" + str);
  Serial.println(str);
}
