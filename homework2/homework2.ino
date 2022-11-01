const int baudRate = 9600;
const int carsLedPinRed = 12;
const int carsLedPinYellow = 13;
const int carsLedPinGreen = 11;
const int pedestriansLedPinGreen = 7;
const int pedestriansLedPinRed = 8;
const int crosswalkBuzzerPin = 3;
const int buttonPressWaitTime = 8000;
const int stateTwoDuration = 3000;
const int stateThreeDuration = 8000;
const int stateFourDuration = 4000;
const int buttonInputPin = 2;
const int buzzerBeepIntervalStateThree = 1000;
const int buzzerBeepIntervalStateFour = 500;
const int pedestriansGreenLedBlinkInterval = 500;
const int buzzerFrequency = 1000;
unsigned long lastStateChangeTime = 0;
const unsigned long debounceTime = 50;
unsigned long buttonStateChangeTime = 0;
unsigned long buzzerLastToneChange = 0;
unsigned long pedestriansLastGreenLedStateChange = 0;
byte buzzerState = 1;
byte pedestriansGreenLedState = 0;
int state = 1;
byte crosswalkButtonState = HIGH;
byte previousCrossalkButtonReading = HIGH;
volatile byte crosswalkButtonReading = 0;
unsigned long lastButtonInputChange = 0;

void readButtonState() {
  if (state != 1 || buttonStateChangeTime != 0) {
    return;
  }
  crosswalkButtonReading = digitalRead(buttonInputPin);

  if (previousCrossalkButtonReading != crosswalkButtonReading && crosswalkButtonReading == LOW) {
    lastButtonInputChange = millis();
  }

  if (millis() - lastButtonInputChange > debounceTime && crosswalkButtonReading == LOW) {
    crosswalkButtonState = crosswalkButtonReading;
    buttonStateChangeTime = millis();
  }
  previousCrossalkButtonReading = crosswalkButtonReading;
}

void initPinModes() {
  pinMode(carsLedPinRed, OUTPUT);
  pinMode(carsLedPinYellow, OUTPUT);
  pinMode(carsLedPinGreen, OUTPUT);
  pinMode(pedestriansLedPinGreen, OUTPUT);
  pinMode(pedestriansLedPinRed, OUTPUT);
  pinMode(crosswalkBuzzerPin, OUTPUT);
  pinMode(buttonInputPin, INPUT_PULLUP);
}

void buzzerBeepingSound(int beepInterval) {
  if (millis() - buzzerLastToneChange > beepInterval) {
    if (buzzerState == 1) {
      noTone(crosswalkBuzzerPin);
      buzzerState = 0;
    } else {
      tone(crosswalkBuzzerPin, buzzerFrequency);
      buzzerState = 1;
    }
    buzzerLastToneChange = millis();
  }
}

void pedestriansGreenLedBlink(int blinkInterval) {
  if (millis() - pedestriansLastGreenLedStateChange > blinkInterval) {
    pedestriansGreenLedState = !pedestriansGreenLedState;
    digitalWrite(pedestriansLedPinGreen, pedestriansGreenLedState);
    pedestriansLastGreenLedStateChange = millis();
  }
}

void applyStateOne() {
  state = 1;
  digitalWrite(carsLedPinGreen, HIGH);
  digitalWrite(carsLedPinRed, LOW);
  digitalWrite(carsLedPinYellow, LOW);
  digitalWrite(pedestriansLedPinRed, HIGH);
  digitalWrite(pedestriansLedPinGreen, LOW);
}

void applyStateTwo() {
  state = 2;
  digitalWrite(carsLedPinGreen, LOW);
  digitalWrite(carsLedPinYellow, HIGH);
  buttonStateChangeTime = 0;
  lastStateChangeTime = millis();
}

void applyStateThree() {
  state = 3;
  digitalWrite(carsLedPinYellow, LOW);
  digitalWrite(pedestriansLedPinRed, LOW);
  digitalWrite(carsLedPinRed, HIGH);
  digitalWrite(pedestriansLedPinGreen, HIGH);
  tone(crosswalkBuzzerPin, buzzerFrequency);
  buzzerLastToneChange = millis();
  buzzerState = 1;
  lastStateChangeTime = buzzerLastToneChange;
}

void applyStateFour() {
  state = 4;
  lastStateChangeTime = millis();
}

void manageStates() {
  switch (state) {
    case 1:
      if (buttonStateChangeTime != 0 && millis() - buttonStateChangeTime >= buttonPressWaitTime)
        applyStateTwo();
      break;
    case 2:
      if (millis() - lastStateChangeTime > stateTwoDuration) {
        applyStateThree();
      }
      break;
    case 3:
      buzzerBeepingSound(buzzerBeepIntervalStateThree);
      if (millis() - lastStateChangeTime > stateThreeDuration) {
        applyStateFour();
      }
      break;
    case 4:
      buzzerBeepingSound(buzzerBeepIntervalStateFour);
      pedestriansGreenLedBlink(pedestriansGreenLedBlinkInterval);
      if (millis() - lastStateChangeTime > stateFourDuration) {
        applyStateOne();
      }
      break;
  }
}

void setup() {
  Serial.begin(baudRate);
  initPinModes();
  applyStateOne();
}

void loop() {
  readButtonState();
  manageStates();
}