// IMPORTANT!
// Note that I have added a custom version of serial2.c to sketch-folder.
// Arduino automatically adds it to the project (no need for #include). And it seems that it succesfully replaces the default serial2.c
// The custom version has an increased RX buffersize in order to handle the big chunks sent by the ESP32.
#include "c:/dev/pdlc/mcu-code/messageStruct.h"

#include <TeensyDMX.h>

namespace teensydmx = ::qindesign::teensydmx;

// Create the DMX sender on Serial1
teensydmx::Sender dmxTx{Serial1};

SerialMessage inMsg;
//Don't set to lower than 8!!!
const int nrOfPixels = 121;

float dutyCycles[nrOfPixels];

//Non user variables
bool pixelStates[nrOfPixels];
bool isFixed[nrOfPixels];
elapsedMillis sinceSwitched[nrOfPixels];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(23, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(115200);
  // Serial3.begin(115200);
  for (int i = 0; i < nrOfPixels; i++) {
    dutyCycles[i] = 0.f;
    pixelStates[i] = false;
    isFixed[i] = false;
  }

  // switchToSerial();
  switchToDMX();

  delay(500);
  Serial.printf("SKETCH STARTED!!\n");
  digitalWrite(LED_BUILTIN, HIGH);
}

bool dmxModeActive = false;

const float frequency = 30;
const int period = 1000 / frequency;
// float duty = 0.5;

bool cycleActive = false;
elapsedMillis sinceCycleUpdate;
elapsedMillis now;
elapsedMillis sinceLoop;
uint32_t mainLoopDuration;

elapsedMillis sincePrint;
uint32_t printInterval = 150;

elapsedMillis sinceDMXPacket;
// uint32_t actualOnDuration = 123456789;
// uint32_t actualOffDuration = 987654321;
uint32_t packetInterval = 0;

elapsedMillis sinceCycleStart = 0;

// uint timeSlot = 5;

const int potPixelOffset = 0;

void loop() {
  mainLoopDuration = sinceLoop;
  sinceLoop = 0;

  //Handle serial comms!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (Serial2.available() >= sizeof(SerialMessage)) {
    int nrOfReceivedBytes = Serial2.readBytes((uint8_t *)&inMsg, sizeof(SerialMessage));
    // Check for corrupt msg!
    if (nrOfReceivedBytes != sizeof(SerialMessage) || inMsg.startChars[0] != '<'){ //} || inMsg.endChars[0] != '>') {
      // Send error byte to esp32
      Serial2.print('!');

      Serial.println("FUUUUCK!!!! CORRUPT MESSAGE ON SERIAL BUS!");
      Serial.print("startChar: ");
      Serial.println(inMsg.startChars);
      // Serial.print("endChar: ");
      // Serial.println(inMsg.endChars);
      while (Serial2.available()) {
        Serial2.read();
      }
    } else {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      memcpy(dutyCycles, inMsg.pixelStates, min(nrOfPixels, serialMessageNrOfPixelValues));
      for (int i = 0; i < min(nrOfPixels, serialMessageNrOfPixelValues); i++)
      {
        dutyCycles[i] = float(inMsg.pixelStates[i]) / 255.f;
      }
      

      Serial.print("SERIAL RECEIVED -->  ");
      printInMsg();
    }
  }
  // if (Serial2.available()) {
  //   Serial.write(Serial2.read());
  // }

  if (sincePrint >= printInterval) {
    sincePrint = 0;
    // timeSlot++;
    // if (timeSlot > 10)
    // {
    //   timeSlot = 4;
    // }
    // Serial.printf("timeSlot: %i \n", timeSlot);
    // Serial.printf("on: %u ms \t off: %u ms. MAINLOOP: %u ms\n", actualOnDuration, actualOffDuration, mainLoopDuration);

    if (dmxModeActive) {
      // Serial.printf("packetInterval %i \n", packetInterval);
      // for (int i = 0; i < 10; i++) {
      //   Serial.printf("%i: %f, ", i, dutyCycles[i]);
      // }
      // Serial.println();

      // Serial.printf("dutyCycles: %f,\t%f,\t%f\n", dutyCycles[0 + potPixelOffset], dutyCycles[1 + potPixelOffset], dutyCycles[2 + potPixelOffset]);
    }
  }

  while (Serial.available()) {
    char byte = Serial.read();
    if (byte == 'q') {
      cycleActive = !cycleActive;
    } else if (byte == 'x') {
      Serial.printf("switching to DMX mode\n");
      switchToDMX();
    } else if (byte == 's') {
      Serial.printf("switching to SERIAL mode\n");
      switchToSerial();
    } else if (!dmxModeActive) {
      Serial1.write(byte);
    }
  }

  // for (int i = 0; i < 3; i++) {
  //   int potValue = analogRead(14 + i);
  //   dutyCycles[i+potPixelOffset] = potValue / 1023.f;
  // }

  if(cycleActive && sinceCycleUpdate > 100){
    sinceCycleUpdate = 0;
    static int cycleIndex = 0;
    Serial.printf("cycleIndex: %i\n", cycleIndex);
    cycleIndex++;
    cycleIndex%=nrOfPixels;
    unSetAllPixels();
    dutyCycles[cycleIndex] = 1.f;
  }

  // for (int i = 0; i < nrOfPixels; i++) {

  //   int potValue = analogRead(16);
  //   dutyCycles[i] = potValue / 1023.f;
  // }

  if (dmxModeActive) {
    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    // setAllPixels();
    // updateAllDMXChannels();
    // delay(1000);

    // unSetAllPixels();
    // updateAllDMXChannels();
    // delay(1000);

    // DMXFadeUpdate();

    DMXFadeUpdate2();

  } else {
    while (Serial1.available()) {
      Serial.write(Serial1.read());
    }

    if (cycleActive) {
      Serial1.printf("HFFFFFFFF");
      Serial.printf("Sending: HFFFFFFFF\n");
      delay(500);
      Serial1.printf("H00000000");
      Serial.printf("Sending: H00000000\n");
      delay(500);
    }
  }
}

void printInMsg() {
  Serial.printf("msg received. expecting %i pixelvalues\n", serialMessageNrOfPixelValues);
  Serial.println();
  Serial.write((const uint8_t *)inMsg.startChars, 4);
  for (int i = 0; i < serialMessageNrOfPixelValues; i++) {
    if (!(i % 11)) {
      Serial.println();
    }
    Serial.printf("%u ", inMsg.pixelStates[i]);
  }
  // Serial.println();
  // Serial.write((const uint8_t *)inMsg.endChars, 4);
}

void switchToSerial() {
  dmxModeActive = false;
  dmxTx.end();
  delay(500);
  Serial1.begin(9600, SERIAL_8N1);
  Serial1.transmitterEnable(2);
}

void switchToDMX() {
  Serial1.clear();
  Serial1.flush();
  Serial1.end();
  dmxModeActive = true;
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  delay(500);
  dmxTx.setPacketSize(75); // Any number smaller than 75 doesn't seem to be correctly handled by the controllers
  dmxTx.begin();
  dmxTx.pause();
}

void DMXFadeUpdate2() {
  if (dmxTx.isTransmitting()) {
    return;
  }
  packetInterval = sinceDMXPacket;
  sinceDMXPacket = 0;
  // int i = 0;
  for (int i = 0; i < nrOfPixels; i++) {
    uint32_t onDuration = period * dutyCycles[i];
    uint32_t offDuration = period * (1.f - dutyCycles[i]);

    if (pixelStates[i]) {
      if (offDuration < packetInterval) {
        isFixed[i] = true;
      } else if (sinceSwitched[i] > onDuration) {
        sinceSwitched[i] = 0;
        isFixed[i] = false;
        pixelStates[i] = false;
      }
    } else {
      if (false && onDuration < packetInterval) {
        isFixed[i] = true;
      } else if (sinceSwitched[i] > offDuration) {
        sinceSwitched[i] = 0;
        isFixed[i] = false;
        pixelStates[i] = true;
      }
    }

    // Set the channel that holds that pixel
    int packetIndex = i / 8;
    uint8_t sendValues = 0;
    for (int j = 0; j < 8; j++) {
      int pixelIndex = packetIndex * 8 + j;
      sendValues |= (int)pixelStates[pixelIndex] << j;
    }
    dmxTx.set(packetIndex + 1, sendValues);
  }

  dmxTx.resumeFor(1);
}

// void DMXFadeUpdate() {
//   if (sinceCycleStart > period) {
//     sinceCycleStart = 0;
//     // Serial.printf("Staring new cycle\n\n");
//     //START A NEW CYCLE
//     setAllPixels();
//     updateAllDMXChannels();
//   }

//   bool anyPixelChanged = false;
//   for (int i = 0; i < nrOfPixels; i++) {
//     if (pixelStates[i] && sinceCycleStart >= (int)(dutyCycles[i] * period)) {
//       // Serial.printf("turning off pixel %i @ %u \n", i, (uint32_t)sinceCycleStart);
//       //Turn off that pixel
//       pixelStates[i] = false;
//       anyPixelChanged = true;

//       // Set the channel that holds that pixel
//       int packetIndex = i / 8;
//       uint8_t sendValues = 0;
//       for (int j = 0; j < 8; j++) {
//         int pixelIndex = packetIndex * sizeof(uint8_t) + j;
//         sendValues |= (int)pixelStates[pixelIndex] << j;
//       }
//       dmxTx.set(packetIndex + 1, sendValues);

//       // for (int i = 0; i < dmxTx.packetSize(); i++) {
//       //   uint8_t sendValues = 0;
//       //   for (int j = 0; j < 8; j++) {
//       //     int pixelIndex = i * 8 + j;
//       //     sendValues |= (int)pixelStates[pixelIndex] << j;
//       //   }

//       //   dmxTx.set(i + 1, sendValues);
//       // }
//       // dmxTx.resumeFor(1);
//     }
//   }

//   if (anyPixelChanged) {
//     // updateAllDMXChannels();
//     dmxTx.resumeFor(1);
//   }
// }

void setAllPixels() {
  for (int i = 0; i < nrOfPixels; i++) {
    dutyCycles[i] = 1.f;
  }
}

void unSetAllPixels() {
  for (int i = 0; i < nrOfPixels; i++) {
    dutyCycles[i] = 0.f;
  }
}

void updateAllDMXChannels() {
  for (int i = 0; i < dmxTx.packetSize(); i++) {
    uint8_t sendValues = 0;
    for (int j = 0; j < 8; j++) {
      int index = i * 8 + j;
      sendValues |= ((int)pixelStates[index] << j);
      // Serial.printf("pixelStates[%i]: %i\n", index, pixelStates[i]);
    }
    // Serial.printf("packet: %u \n", sendValues);

    dmxTx.set(i + 1, sendValues);
  }
  dmxTx.resumeFor(1);
}