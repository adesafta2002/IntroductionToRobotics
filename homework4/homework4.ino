const int baudRate = 9600;
const long blinkInterval = 300;
const int longPressThreshold = 2000;
int state = 1;
int currentDisplay = 3;
// declaring consts for encodings
const int encodingsNumber = 16;
int byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};
// declaring display pins and values
const int numberOfDisplays = 4;
const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;
byte blinkingDpState = 0;
const byte regSize = 8;  // 1 byte aka 8 bits

int displayDigits[numberOfDisplays] = {
  segD1, segD2, segD3, segD4
};

int displayValues[numberOfDisplays] = {
  0, 0, 0, 0
};
// declaring pins and values for joystick
const int joystickPinSw = 2;  // digital pin connected to switch output
const int joystickPinX = A0;  // A0 - analog pin connected to X output
const int joystickPinY = A1;  // A1 - analog pin connected to Y output
const int upperLimit = 800;
const int lowerLimit = 300;
const int left = 0;
const int right = 1;
const int up = 2;
const int down = 3;
byte previousSwReading = 1;
byte swState = LOW;
int xValueJoystick = 0;
int yValueJoystick = 0;
int lastXState = 0;
int lastYState = 0;
// declaring shift register pins
const int latchPin = 11;  // STCP to 12 on Shift Register
const int clockPin = 10;  // SHCP to 11 on Shift Register
const int dataPin = 12;   // DS to 14 on Shift Register
// declaring time values
const unsigned long debounceTime = 50;
unsigned long lastButtonInputChange = 0;
unsigned long lastBlinkStateChange = 0;
unsigned long upTime = 0;

void initializePinModes() {
  pinMode(joystickPinSw, INPUT_PULLUP);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  for (int i = 0; i < numberOfDisplays; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
}

void blinkSegment() {
  if (state != 1) {
    return;
  }
  upTime = millis();
  if (upTime - lastBlinkStateChange >= blinkInterval) {
    lastBlinkStateChange = upTime;
    blinkingDpState = !blinkingDpState;
  }
}

void deactivateDisplays() {
  for (int i = 0; i < numberOfDisplays; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
}

void writeReg(int encoding, int displayIndex) {
  digitalWrite(latchPin, LOW);
  byte msb = B0;
  for (int i = 0; i < regSize - 1; i++) {
    msb = bitRead(encoding, regSize - i - 1);
    insertBitIntoShiftReg(msb);
  }
  if (displayIndex == currentDisplay) {
    insertBitIntoShiftReg(blinkingDpState);
  } else {
    insertBitIntoShiftReg(0);
  }
  digitalWrite(latchPin, HIGH);
}

void insertBitIntoShiftReg(byte bitToInsert) {
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, bitToInsert);
  digitalWrite(clockPin, HIGH);
}

void refreshDisplay() {
  for (int i = 0; i < numberOfDisplays; i++) {
    deactivateDisplays();
    writeReg(byteEncodings[displayValues[i]], i);
    // activating I'th display 
    digitalWrite(displayDigits[numberOfDisplays - i - 1], LOW);
    delay(5);
  }
}

void reset() {
  currentDisplay = 3;
  for (int i = 0; i < numberOfDisplays; i++) {
    displayValues[i] = 0;
  }
}

void valueLeftDefaultRange(int movement) {
  switch (movement) {
    case right:
      {
        currentDisplay++;
        if (currentDisplay > 3) {
          currentDisplay = 0;
        }
        return;
      }
    case left:
      {
        currentDisplay--;
        if (currentDisplay < 0) {
          currentDisplay = 3;
        }
        return;
      }
    case up:
      {
        if (state == 1) {
          return;
        }
        displayValues[currentDisplay]++;
        if (displayValues[currentDisplay] > 15) {
          displayValues[currentDisplay] = 0;
        }
        return;
      }
    case down:
      {
        if (state == 1) {
          return;
        }
        displayValues[currentDisplay]--;
        if (displayValues[currentDisplay] < 0) {
          displayValues[currentDisplay] = 15;
        }
        return;
      }
  }
}

void implementXMovement() {
  if (xValueJoystick > upperLimit) {
    if (lastXState != 1) {
      valueLeftDefaultRange(right);
    }
    lastXState = 1;
    return;
  }
  if (xValueJoystick < lowerLimit) {
    if (lastXState != -1) {
      valueLeftDefaultRange(left);
    }
    lastXState = -1;
    return;
  }
  lastXState = 0;
}

void implementYMovement() {
  if (yValueJoystick > upperLimit) {
    if (lastYState != 1) {
      valueLeftDefaultRange(down);
    }
    lastYState = 1;
    return;
  }
  if (yValueJoystick < lowerLimit) {
    if (lastYState != -1) {
      valueLeftDefaultRange(up);
    }
    lastYState = -1;
    return;
  }
  lastYState = 0;
}

void implementButtonPress() {
  if (previousSwReading != swState && swState == LOW) {
    lastButtonInputChange = millis();
  }

  if (lastButtonInputChange && previousSwReading != swState && swState == HIGH) {
    if (millis() - lastButtonInputChange < debounceTime) {
      return;
    }
    lastButtonInputChange = 0;
    handleShortButtonPress();
  }

  if (lastButtonInputChange && millis() - lastButtonInputChange > longPressThreshold) {
    lastButtonInputChange = 0;
    if (state == 1)
      reset();
  }

  previousSwReading = swState;
}

void takeInputBasedOnState() {
  if (state == 1) {
    implementXMovement();
    return;
  } else {
    implementYMovement();
    return;
  }
}

void readJoystickInput() {
  swState = digitalRead(joystickPinSw);
  xValueJoystick = analogRead(joystickPinX);
  yValueJoystick = analogRead(joystickPinY);
  takeInputBasedOnState();
  implementButtonPress();
}

void handleShortButtonPress() {
  if (state == 1) {
    state++;
    blinkingDpState = 1;
  } else
    state--;
}

void setup() {
  initializePinModes();
  Serial.begin(baudRate);
}

void loop() {
  refreshDisplay();
  readJoystickInput();
  blinkSegment();
}