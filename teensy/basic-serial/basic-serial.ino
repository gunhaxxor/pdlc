#include <TeensyDMX.h>

namespace teensydmx = ::qindesign::teensydmx;

// Create the DMX sender on Serial1

teensydmx::Sender dmxTx{Serial1};

const int frequency = 10;
const int period = 1000 / frequency;
float duty = 0.5;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(23, OUTPUT);

  Serial.begin(115200);

  // switchToSerial();
  switchToDMX();

  delay(500);
  Serial.printf("SKETCH STARTED!!\n");
}

bool dmxModeActive = false;

void switchToSerial()
{
  dmxModeActive = false;
  dmxTx.end();
  delay(500);
  Serial1.begin(9600, SERIAL_8N1);
  Serial1.transmitterEnable(2);
}

void switchToDMX()
{
  dmxModeActive = true;
  Serial1.clear();
  Serial1.flush();
  Serial1.end();
  delay(500);
  dmxTx.begin();
}

bool cycleActive = false;

void loop()
{
  int potValue = analogRead(15);
  duty = potValue / 1023.f;

  int onPeriod = period * duty;
  int offPeriod = period * (1.0f - duty);

  Serial.printf("duty: %f\n", duty);
  // analogWrite(23, potValue / 4);
  // digitalWrite(23, HIGH);
  // delay(onPeriod);

  // digitalWrite(23, LOW);
  // delay(offPeriod);

  // return;

  while (Serial.available())
  {
    char byte = Serial.read();
    if (byte == 'q')
    {
      cycleActive = !cycleActive;
    }
    else if (byte == 'x')
    {
      Serial.printf("switching to DMX mode\n");
      switchToDMX();
    }
    else if (byte == 's')
    {
      Serial.printf("switching to SERIAL mode\n");
      switchToSerial();
    }
    else if (!dmxModeActive)
    {
      Serial1.write(byte);
    }
  }

  if (dmxModeActive)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    dmxTx.set(1, 0xFF);
    delay(onPeriod);

    digitalWrite(LED_BUILTIN, LOW);
    dmxTx.set(1, 0x00);
    delay(offPeriod);
  }
  else
  {
    while (Serial1.available())
    {
      Serial.write(Serial1.read());
    }

    if (cycleActive)
    {
      Serial1.printf("HFFFFFFFF");
      Serial.printf("Sending: HFFFFFFFF\n");
      delay(500);
      Serial1.printf("H00000000");
      Serial.printf("Sending: H00000000\n");
      delay(500);
    }
  }
}