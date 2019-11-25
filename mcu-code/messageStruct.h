struct SerialMessage {
  char startChars[4] = "<<<";
  uint8_t pixelStates[121];
  // char endChars[4] = ">>>";
};

const int serialMessageNrOfPixelValues =
    sizeof(SerialMessage::pixelStates) / sizeof(SerialMessage::pixelStates[0]);
