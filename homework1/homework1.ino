const int inputPinRed = A0;
const int inputPinGreen = A2;
const int inputPinBlue = A4;
const int ledPinRed = 9;
const int ledPinGreen = 10;
const int ledPinBlue = 11;
const int inputLowestValue = 0;
const int inputHighestValue = 1023;
const int ledLowestValue = 0;
const int ledHighestValue = 255;
const int baudRate = 9600;
int inputRedValue = 0;
int inputGreenValue = 0;
int inputBlueValue = 0;

void initializePinModes() {
  pinMode(inputPinRed, INPUT);
  pinMode(inputPinGreen, INPUT);
  pinMode(inputPinBlue, INPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
}

void readInputPinValues() {
  inputRedValue = analogRead(inputPinRed);
  inputGreenValue = analogRead(inputPinGreen);
  inputBlueValue = analogRead(inputPinBlue);
}

int mapInputValueToLedValue(int inputValue) {
  int mappedInputValue = map(inputValue, inputLowestValue, inputHighestValue, ledLowestValue, ledHighestValue);
  return mappedInputValue;
}

void setLedPinsValuesBasedOnInputInput() {
  int mappedInputRedValue = mapInputValueToLedValue(inputRedValue);
  int mappedInputGreenValue = mapInputValueToLedValue(inputGreenValue);
  int mappedInputBlueValue = mapInputValueToLedValue(inputBlueValue);

  analogWrite(ledPinRed, mappedInputRedValue);
  analogWrite(ledPinGreen, mappedInputGreenValue);
  analogWrite(ledPinBlue, mappedInputBlueValue);
}

void setup() {
  Serial.begin(baudRate);
  initializePinModes();
}

void loop() {
  readInputPinValues();
  setLedPinsValuesBasedOnInputInput();
}