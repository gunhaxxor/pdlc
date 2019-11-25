#include "c:/dev/pdlc/mcu-code/messageStruct.h"
#include <IotWebConf.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "glasPixelThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "12341234";

#define CONFIG_VERSION "2pdlc"

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
#define STRING_LEN 128
char serverUrl[STRING_LEN] = "ws://192.168.1.210:8800/";
IotWebConfParameter serverUrlParam = IotWebConfParameter("ServerURL", "serverUrl", serverUrl, STRING_LEN);
IotWebConfSeparator separator1 = IotWebConfSeparator();

// struct SocketMessage {
//   char startChars[4] = "<<<";
//   uint8_t pixelStates[121];
// };
// const int socketMessageNrOfPixelValues =
//     sizeof(SocketMessage::pixelStates) / sizeof(pixel);

// SerialMessage socketMsg;

SerialMessage outMsg;

// https://github.com/gilmaimon/ArduinoWebsockets
// Be aware! I hade to make some edits to the library since it's using String object for binary data.
// This resulted in the data being interpreted as null terminated and parts of the message was lost.
// See this issue: https://github.com/gilmaimon/ArduinoWebsockets/issues/32
#include <ArduinoWebsockets.h>
// #include <WiFi.h>

#define LED_BUILTIN 22
#define STATUS_PIN LED_BUILTIN

// const char *ssid = "IAMAIOTDEVICE";      // Enter SSID
// const char *password = "verysafepassword"; // Enter Password

// const char *url = "ws://echo.websocket.org:80/";
// const char *url = "ws://fredmill.herokuapp.com:80/";
// const char *url = "ws://192.168.1.210:8800/";

bool webSocketConnected = false;

using namespace websockets;

WebsocketsClient client;
void onEventsCallback(WebsocketsEvent event, std::string data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Websocket Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Websocket Connnection Closed");
    delay(3000);
    // iotWebConf.delay(3000);
    Serial.println("Trying to reconnect to websocket");
    client.connect(serverUrl);
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void handleRoot() {
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal()) {
    // -- Captive portal request were already served.
    return;
  }
  // iotWebConf.handleConfig();
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>Glaspixel</title></head><body>Glaspixel!";
  s += "<ul>";
  s += "<li>serverUrl: ";
  s += serverUrl;
  s += "</ul>";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial2.begin(115200);
  Serial.begin(115200);

  // float duty = 0.f;
  for (int i = 0; i < serialMessageNrOfPixelValues; i++) {
    float duty = float(i) / float(serialMessageNrOfPixelValues);
    // duty += 0.1;
    // if(duty > 1.0f){
    //   duty = 0.f;
    // }
    outMsg.pixelStates[i] = duty;
  }

  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.addParameter(&separator1);
  iotWebConf.addParameter(&serverUrlParam);
  iotWebConf.getApTimeoutParameter()->visible = true;
  // -- Initializing the configuration.
  iotWebConf.init();
  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", [] { iotWebConf.handleConfig(); });
  server.onNotFound([]() { iotWebConf.handleNotFound(); });

  // Connect to wifi
  // WiFi.begin(ssid, password);

  // // Wait some time to connect to wifi
  // for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
  // {
  //   Serial.print(".");
  //   delay(1000);
  // }

  // // Check if connected to wifi
  // if (WiFi.status() != WL_CONNECTED)
  // {
  //   Serial.println("No Wifi!");
  //   return;
  // }

  iotWebConf.setWifiConnectionCallback(&wifiConnected);

  // sendSerialRequest();
}

unsigned long printStamp = 0;
unsigned long printInterval = 500;

uint8_t requestId = 0;

unsigned long socketRequestStamp = 0;
unsigned long socketRequestTimeOut = 1000;

unsigned long receivedSocketDataStamp = 0;
unsigned long socketDataInterval = 0;

unsigned long serialRequestStamp = 0;
unsigned long serialRequestTimeOut = 500;

unsigned long now = 0;
void loop() {
  now = millis();

  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
  if (iotWebConf.getState() != 4) {
    return;
  }

  // Receive status byte from teensy
  if (Serial2.available()) {
    uint8_t inByte = Serial2.read();
    // if (inByte == '.')
    // {
    //   Serial2.write((const char *)&outMsg, sizeof(SerialMessage));
    // }
    // else
    if (inByte == '!') {
      Serial.println("----------- Some error with serial comms!");
    }
    // else
    // {
    //   Serial.printf("received: %c", inByte);
    // }
  }

  // Serial2.write((const uint8_t *)&outMsg, sizeof(SerialMessage));
  // Serial.write((const uint8_t *)&outMsg, sizeof(SerialMessage));
  // Serial.println();
  // delay(500);
  // return;

  // if (Serial2.available() >= sizeof(SerialMessage)) {
  //   int nrOfReceivedBytes =
  //       Serial2.readBytes((uint8_t *)&inMsg, sizeof(SerialMessage));
  //   // Check for corrupt msg!
  //   if (nrOfReceivedBytes != sizeof(SerialMessage) ||
  //       inMsg.startChar[0] != '<' || inMsg.endChar[0] != '>') {
  //     Serial2.print('!');
  //     Serial.println("FUUUUCK!!!! CORRUPT MESSAGE ON SERIAL BUS!");
  //     Serial.print("startChar: ");
  //     Serial.println(inMsg.startChar);
  //     Serial.print("endChar: ");
  //     Serial.println(inMsg.endChar);
  //     while (Serial2.available()) {
  //       Serial2.read();
  //     }
  //   } else {
  //     // Serial.print("SERIAL RECEIVED -->  ");
  //     // printInMsg();
  //     if (webSocketConnected && shouldRequestSocketData) {
  //       // Serial.printf("Sending to socket: \n");
  //       // printSocketData();
  //       client.sendBinary((const char *)&inMsg, sizeof(SerialMessage));
  //       socketRequestStamp = now;
  //       shouldRequestSocketData = false;
  //       digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //     }
  //     sendSerialRequest();
  //   }
  // } else if (now - serialRequestStamp > serialRequestTimeOut) {
  //   sendSerialRequest();
  // }

  if (now - socketRequestStamp > socketRequestTimeOut) {
    requestSocketData();
  }

  if (webSocketConnected && now - printStamp > printInterval) {
    printStamp = now;
    // printSocketData();
    // printOutMsg();

    Serial.printf("socketDataInterval: %i, requestId: %i \n", socketDataInterval, requestId);
  }

  // let the websockets client check for incoming messages
  if (webSocketConnected) {
    if (client.available()) {
      client.poll();
    } else {
      Serial.println("client not available. Trying to reconnect");
      webSocketConnected = false;
      webSocketConnected = client.connect(serverUrl);

      // Serial.println("client not available. RESTARTING");
      // ESP.restart();
    }
  } else {
    Serial.println("socket not connected. Trying to reconnect");
    webSocketConnected = client.connect(serverUrl);
    if (webSocketConnected)
      client.send("Hello Server");
  }
}

void printSocketData() {
  printOutMsg();
  Serial.write((const uint8_t *)&outMsg, sizeof(SerialMessage));
  Serial.println();
}

void printOutMsg() {
  Serial.printf("msg received. expecting %i pixelvalues\n", serialMessageNrOfPixelValues);
  Serial.println();
  Serial.write((const uint8_t *)outMsg.startChars, 4);
  for (int i = 0; i < 11; i++) {
    if (!(i % 11)) {
      Serial.println();
    }
    Serial.printf("%f ", outMsg.pixelStates[i]);
  }
  // Serial.println();
  // Serial.write((const uint8_t *)outMsg.endChars, 4);
}

// void sendSerialRequest() {
//   serialRequestStamp = millis();
//   // Serial.printf("requesting serial\n");
//   Serial2.print('.');
// }

void requestSocketData() {
  static uint8_t requestCounter = 0;
  static uint8_t msg[] = "esp32?";
  requestCounter++;
  requestCounter %= 127; // Handle weird UTF16 behaviour!!

  requestId = requestCounter + 1;
  msg[5] = requestId;

  // Serial.printf("requesting socketData id: %i\n", requestId);
  socketRequestStamp = now;

  client.send((const char *)msg);
}

void wifiConnected() {
  Serial.println("Connected to Wifi, Connecting to server.");

  // digitalWrite(LED_BUILTIN, LOW);
  // return;
  // try to connect to Websockets server
  webSocketConnected = client.connect(serverUrl);

  if (webSocketConnected) {
    Serial.println("Connected!");
    client.send("esp32");
  } else {
    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message) {
    socketDataInterval = now - receivedSocketDataStamp;
    receivedSocketDataStamp = now;
    static int msgCounter = 0;
    msgCounter++;
    // digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN));
    // if(msgCounter%2){
    //   iotWebConf.fineBlink(0, 100);
    // }else{
    //   iotWebConf.fineBlink(100, 0);
    // }
    // Serial.print("Got Message: ");
    // Serial.println(message.data());
    if (message.isBinary()) {
      // Serial.printf("trying to extract %i bytes from received msg", message.length());
      // String data = message.data();
      // Serial.printf("string object is %i chars\n", data.length());
      memcpy(&outMsg, (message.data().c_str()), sizeof(SerialMessage));
      // // printOutMsg();
      Serial2.write((const uint8_t *)&outMsg, sizeof(SerialMessage));
      // Serial.println(msgCounter);

      if (uint8_t(outMsg.startChars[2]) == requestId) {
        requestSocketData();
      } else {
        Serial.printf("----------outdated request answered");
      }
    } else {
      Serial.printf("msg: %s\n", message.data().c_str());
    }
  });
  client.onEvent(onEventsCallback);
}