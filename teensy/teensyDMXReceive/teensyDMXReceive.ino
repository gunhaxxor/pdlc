#include <cstring>

#include <TeensyDMX.h>

namespace teensydmx = ::qindesign::teensydmx;

// Create the DMX receiver on Serial1.
teensydmx::Receiver dmxRx{Serial1};

// The last value on the channel, for knowing when to print a change
// (Example 1).
uint8_t lastValue = 0;

// Buffer in which to store packet data (Example 2).
uint8_t packetBuf[3]{0};

// The last values received on channels 10-12, initialized to zero.
uint8_t rgb[3]{0};

void setup() {
  // Serial initialization, for printing things
  Serial.begin(115200);
  while (!Serial && millis() < 4000) {
    // Wait for initialization to complete or a time limit
  }
  Serial.println("Starting BasicReceive.");

  // Turn on the LED, for indicating activity
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWriteFast(LED_BUILTIN, HIGH);

  // Start the receiver
  dmxRx.begin();

  // Print the first values
  lastValue = dmxRx.get(2);
  Serial.printf("Channel 1: %d\n", lastValue);

}

void loop() {
  //Print dxm values from channel 3,4,5,6
  Serial.printf("Channel 3: %d\n", dmxRx.get(3));
  Serial.printf("Channel 4: %d\n", dmxRx.get(4));
  Serial.printf("Channel 5: %d\n", dmxRx.get(5));
  Serial.printf("Channel 6: %d\n", dmxRx.get(6));

}
