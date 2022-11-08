const int possibleMovements = 4;  // LEFT-RIGHT-UP-DOWN
const int baudRate = 9600;
const long blinkInterval = 200;
const int longPressThreshold = 2000;
const int maxJoystickValue = 1023;
const int averageDefaultJoystickPosition = 520;
int state = 1;
int currentPosition = 7;
int debounceJoystick = 15;
// declaring segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;
const int segSize = 8;
bool commonAnode = false;
const int noOfDigits = 10;
byte blinkingSegmentState = 0;
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};
byte segmentsValues[segSize] = { 0, 0, 0, 0, 0, 0, 0, 0 };  // a  b  c  d  e  f  g dp
const byte defaultSegmentsValues[segSize] = { 0, 0, 0, 0, 0, 0, 0, 0 };
const int nextSegmentBasedOnCurrentPosition[segSize][possibleMovements] = {
  // LEFT  RIGHT  UP  DOWN
  { 5, 1, 0, 6 },  // a
  { 5, 1, 0, 6 },  // b
  { 4, 7, 6, 3 },  // c
  { 4, 2, 6, 3 },  // d
  { 4, 2, 6, 3 },  // e
  { 5, 1, 0, 6 },  // f
  { 6, 6, 0, 3 },  // g
  { 2, 7, 7, 7 },  // dp
};

// declaring pins and values for joystick
const int joystickPinSw = 2;  // digital pin connected to switch output
const int joystickPinX = A0;  // A0 - analog pin connected to X output
const int joystickPinY = A1;  // A1 - analog pin connected to Y output
const int upperLimit = 800;
const int lowerLimit = 300;
int left = 0;
int right = 1;
int up = 2;
int down = 3;
byte previousSwReading = 1;
byte swState = LOW;
int xValueJoystick = 0;
int yValueJoystick = 0;
int lastXState = 0;
int lastYState = 0;
// declaring time values
const unsigned long debounceTime = 50;
unsigned long lastButtonInputChange = 0;
unsigned long lastBlinkStateChange = 0;
unsigned long upTime = 0;

void initializePinModes() {
  pinMode(joystickPinSw, INPUT_PULLUP);
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
}

void digitalWriteForCurrentPosition() {
  digitalWrite(segments[currentPosition], segmentsValues[currentPosition]);
}

void blinkSegment() {
  if (state != 1) {
    return;
  }
  upTime = millis();
  if (upTime - lastBlinkStateChange >= blinkInterval) {
    lastBlinkStateChange = upTime;
    blinkingSegmentState = !blinkingSegmentState;
    digitalWrite(segments[currentPosition], blinkingSegmentState);
  }
}

void reset() {
  for (int i = 0; i < segSize; i++) {
    segmentsValues[i] = 0;
    digitalWrite(segments[i], 0);
    currentPosition = 7;
  }
}

void valueLeftDefaultRange(int movement, boolean toggleSegment = false) {
  if (state != 1) {
    if (toggleSegment) {
      segmentsValues[currentPosition] = !segmentsValues[currentPosition];
    }
    digitalWriteForCurrentPosition();
    return;
  }
  digitalWriteForCurrentPosition();
  currentPosition = nextSegmentBasedOnCurrentPosition[currentPosition][movement];
}


void implementXMovement() {
  if (xValueJoystick > upperLimit) {
    if (lastXState != 1) {
      valueLeftDefaultRange(right, true);
      Serial.println("X RIght");
    }
    lastXState = 1;
    return;
  }
  if (xValueJoystick < lowerLimit) {
    if (lastXState != -1) {
      valueLeftDefaultRange(left, true);
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

  if (state == 1 && lastButtonInputChange && millis() - lastButtonInputChange > longPressThreshold) {
    lastButtonInputChange = 0;
    reset();
  }

  previousSwReading = swState;
}

void takeOnlyHigherJoystickInput() {
  if (state != 1) {
    implementXMovement();
    return;
  }
  int xDistanceToHitLimit = 0;
  int yDistanceToHitLimit = 0;
  if (xValueJoystick > upperLimit) {
    xDistanceToHitLimit = maxJoystickValue - xValueJoystick;
  } else {
    xDistanceToHitLimit = xValueJoystick;
  }
  if (yValueJoystick > upperLimit) {
    yDistanceToHitLimit = maxJoystickValue - yValueJoystick;
  } else {
    yDistanceToHitLimit = yValueJoystick;
  }

  if (xDistanceToHitLimit < yDistanceToHitLimit) {
    implementXMovement();
  } else {
    implementYMovement();
  }
  if (lowerLimit + debounceJoystick < yValueJoystick && yValueJoystick < upperLimit - debounceJoystick) {
    lastYState = 0;
  }
  if (lowerLimit + debounceJoystick < xValueJoystick && xValueJoystick < upperLimit - debounceJoystick) {
    lastXState = 0;
  }
}

void readJoystickInput() {
  swState = digitalRead(joystickPinSw);
  xValueJoystick = analogRead(joystickPinX);
  yValueJoystick = analogRead(joystickPinY);
  takeOnlyHigherJoystickInput();
  implementButtonPress();
}

void handleShortButtonPress() {
  if (state == 1) {
    state++;
    digitalWriteForCurrentPosition();
  } else
    state--;
}

void setup() {
  initializePinModes();
  Serial.begin(baudRate);
}

void loop() {
  readJoystickInput();
  blinkSegment();
}