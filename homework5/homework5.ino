#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// declaring pins and values for joystick
const int joystickPinSw = 2;  // digital pin connected to switch output
const int joystickPinX = A0;  // A0 - analog pin connected to X output
const int joystickPinY = A1;  // A1 - analog pin connected to Y output
const int upperLimit = 900;
const int lowerLimit = 300;
const int defaultThresholdLower = 450;
const int defaultThresholdUpper = 650;
const byte timeBetweenEachTextScroll = 150;
const byte left = 0;
const byte right = 1;
const byte up = 2;
const byte down = 3;
const int longPressThreshold = 2000;
byte previousSwReading = 1;
byte swState = LOW;
int xValueJoystick = 0;
int yValueJoystick = 0;
int lastXState = 0;
int lastYState = 0;

// declaring pins and values for matrix
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
const byte defaultFacing = 4;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  //DIN, CLK, LOAD, No. DRIVER
int snakeHeadYPosition = 0;
int snakeHeadXPosition = 0;
int foodYPosition = random(1, 7);
int foodXPosition = random(1, 7);
byte facing = defaultFacing;

// declaring pins and values for lcd
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const int LCDcontrastPin = A5;
const int LCDContrastMaxValue = 125;
const int LCDBrightnessPin = 3;
const int LCDbrightnessMaxValue = 255;
const int matrixbrightnessMaxValue = 15;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// declaring time values
const unsigned long debounceTime = 50;
const unsigned long moveHeadInterval = 450;
unsigned long lastButtonInputChange = 0;
unsigned long lastTextScroll = 0;
unsigned long lastHeadMove = 50;
unsigned long upTime = 0;

const int gameOverState = -1;
const byte gameState = 0;
const byte menuState = 1;
const byte settingsState = 2;
const byte highscoreState = 3;
const byte aboutState = 4;
const byte howToPlayState = 5;
const byte greetingState = 6;
const byte settingsClickedState = 7;
const byte numberOfHighscores = 5;
byte currentMenuPosition = 0;
byte currentSettingsPosition = 0;
byte currentHighscorePosition = 0;
int appState = greetingState;
int score = 0;

// declaring menu items
const byte menuItemsNumber = 5;
const byte settingsItemsNumber = 7;

const String menuItems[menuItemsNumber] = {
  "1.Start Game",
  "2.Highscore",
  "3.Settings",
  "4.About",
  "5.How to play"
};

const String settingsItems[settingsItemsNumber] = {
  "1.Enter name",
  "2.Level",
  "3.LCD_Ct",
  "4.LCD_Br",
  "5.M_Br",
  "6.Sound",
  "7.Back"
};

String highScoresNames[numberOfHighscores];

int highScores[numberOfHighscores];

const byte matrixGameOver[matrixSize] = {
  0b11000011,
  0b11100111,
  0b01111110,
  0b00111100,
  0b00111100,
  0b01111110,
  0b11100111,
  0b11000011
};

const byte matrixDefault[matrixSize] = {
  0b00000000,
  0b01100110,
  0b01100110,
  0b00000000,
  0b01000010,
  0b01111110,
  0b00111100,
  0b00000000
};

int longTextCurrentPosition = 0;

// game data
String userName;
byte nameLength = 0;
byte gameDifficulty = 0;
byte LCDContrast = 0;
byte LCDBrightness = 0;
byte MTXBrightness = 0;
bool sound = false;
// initialize addresses
const byte nameStartAddress = 0;
const byte nameEndAddress = 9;
const byte stringLengthAddress = 10;
const byte gameDifficultyAddress = 11;
const byte LCDContrastAddress = 12;
const byte LCDBrightnessAddress = 13;
const byte MTXBrightnessAddress = 14;
const byte soundAddress = 15;
const byte startingHighscoresAddress = 16;
const byte maxNameSpacesOccupied = 10;
const byte scoreSpacesOccupied = 4;
const byte nameLengthSpacesOccupied = 1;
const byte positionsNeededForOneHighScore = 15;


void setMatrixToDefaultDisplay() {
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(matrixDefault[i], 7 - j));
    }
  }
}

void initializePinModes() {
  pinMode(joystickPinSw, INPUT_PULLUP);
}

void saveNameToEEPROM(byte lengthAddress, byte length, String name, byte nameStartAddress) {
  EEPROM.update(lengthAddress, length);
  for (byte i = nameStartAddress; i < nameStartAddress + length; i++)
    EEPROM.update(i, name[i - nameStartAddress]);
}

String readNameFromEEPROM(byte userNameLengthAddress, byte nameStartAddress) {
  byte userNameLength;
  EEPROM.get(userNameLengthAddress, userNameLength);
  char name[userNameLength];
  for (byte i = nameStartAddress; i < nameStartAddress + userNameLength; i++) {
    name[i - nameStartAddress] = EEPROM.read(i);
  }
  return String(name).substring(0, userNameLength);
}

void saveNewHighscoresToEEPROM() {
  for (byte i = 0; i < numberOfHighscores; i++) {
    const byte startingAddressForIHighScoreName = startingHighscoresAddress + i * positionsNeededForOneHighScore;
    const byte startingAddressForIHighScoreNameLength = startingAddressForIHighScoreName + maxNameSpacesOccupied;
    const byte startingAddressForIHighScore = startingAddressForIHighScoreNameLength + nameLengthSpacesOccupied;
    saveNameToEEPROM(startingAddressForIHighScoreNameLength, highScoresNames[i].length(), highScoresNames[i], startingAddressForIHighScoreName);
    EEPROM.update(startingAddressForIHighScore, highScores[i]);
  }
}

void getHighScoresFromEEPROM() {
  for (byte i = 0; i < numberOfHighscores; i++) {
    const byte startingAddressForIHighScoreName = startingHighscoresAddress + i * positionsNeededForOneHighScore;
    const byte startingAddressForIHighScoreNameLength = startingAddressForIHighScoreName + maxNameSpacesOccupied;
    const byte startingAddressForIHighScore = startingAddressForIHighScoreNameLength + nameLengthSpacesOccupied;
    highScoresNames[i] = readNameFromEEPROM(startingAddressForIHighScoreNameLength, startingAddressForIHighScoreName);
    EEPROM.get(startingAddressForIHighScore, highScores[i]);
    if (!highScores[i]) {
      highScores[i] = 0;
      highScoresNames[i] = "Empty";
    }
  }
}

void readGameDetailsFromEEPROM() {
  EEPROM.get(gameDifficultyAddress, gameDifficulty);
  EEPROM.get(LCDContrastAddress, LCDContrast);
  EEPROM.get(LCDBrightnessAddress, LCDBrightness);
  EEPROM.get(MTXBrightnessAddress, MTXBrightness);
  EEPROM.get(soundAddress, sound);

  analogWrite(LCDcontrastPin, LCDContrast);
  analogWrite(LCDBrightnessPin, LCDBrightness);
  lc.setIntensity(0, MTXBrightness);
}

void changeUserName() {
  if (Serial.available()) {
    String serialInput = Serial.readString();
    serialInput.trim();
    nameLength = (byte)serialInput.length();
    if (nameLength) {
      if (nameLength > maxNameSpacesOccupied) {
        serialInput = serialInput.substring(0, maxNameSpacesOccupied);
        nameLength = maxNameSpacesOccupied;
      }
      userName = serialInput;
      saveNameToEEPROM(stringLengthAddress, nameLength, userName, nameStartAddress);
      appState = settingsState;
      printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, false);
    }
  }
}

void printGreetingMessage() {
  printFirstAndSecondLineForLCD("    Welcome!    ", "*Press to start*");
}

void printItemsForCurrentPosition(String items[], String title, int position, bool replaceNumberForClicked) {
  lcd.clear();
  lcd.print(title);
  lcd.setCursor(0, 1);
  if (replaceNumberForClicked) {
    String itemToDisplay = items[position];
    String replaceValue = String(position + 1) + ".";
    itemToDisplay.replace(replaceValue, ">");
    lcd.print(itemToDisplay);
    return;
  }
  lcd.print(items[position]);
}

void printFirstAndSecondLineForLCD(String firstLine, String secondLine) {
  lcd.clear();
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
}

void valueLeftDefaultRange(byte movement) {
  switch (appState) {
    case gameState:
      {
        facing = movement;
        return;
      }
    case menuState:
      {
        implementMenuMovement(movement);
        return;
      }
    case settingsState:
      {
        implementSettingsMovement(movement);
        return;
      }
    case highscoreState:
      {
        implementHighscoresMovement(movement);
        return;
      }
    case aboutState:
      {
        const int maxLength = 23;
        implementAboutAndHowToPlayMovement(movement, maxLength);
        return;
      }
    case howToPlayState:
      {
        const int maxLength = 24;
        implementAboutAndHowToPlayMovement(movement, maxLength);
        return;
      }
    case settingsClickedState:
      {
        implementSettingsClickedMovement(movement);
        return;
      }
  }
}

void implementAboutAndHowToPlayMovement(byte movement, byte maxLength) {
  switch (movement) {
    case left:
      {
        if (longTextCurrentPosition == 0 || millis() - lastTextScroll < timeBetweenEachTextScroll) {
          return;
        }
        lastTextScroll = millis();
        longTextCurrentPosition--;
        lcd.scrollDisplayRight();
        break;
      }
    case right:
      {
        if (longTextCurrentPosition == maxLength || millis() - lastTextScroll < timeBetweenEachTextScroll) {
          return;
        }
        longTextCurrentPosition++;
        lastTextScroll = millis();
        lcd.scrollDisplayLeft();
        break;
      }
  }
}

void implementMenuMovement(byte movement) {
  switch (movement) {
    case up:
      {
        if (currentMenuPosition == 0) {
          break;
        }
        currentMenuPosition--;
        printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
        break;
      }
    case down:
      {
        if (currentMenuPosition == menuItemsNumber - 1) {
          break;
        }
        currentMenuPosition++;
        printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
      }
  }
}

void implementSettingsMovement(byte movement) {
  switch (movement) {
    case up:
      {
        if (currentSettingsPosition == 0) {
          return;
        }
        currentSettingsPosition--;
        printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, false);
        break;
      }
    case down:
      {
        if (currentSettingsPosition == settingsItemsNumber - 1) {
          return;
        }
        currentSettingsPosition++;
        printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, false);
        break;
      }
  }
  completeSettingsWithValues();
}

void implementSettingsClickedMovement(byte movement) {
  switch (movement) {
    case up:
      {
        switch (currentSettingsPosition) {
          case 1:
            {
              if (gameDifficulty == 2) {
                return;
              }
              gameDifficulty++;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              EEPROM.update(gameDifficultyAddress, gameDifficulty);
              break;
            }
          case 2:
            {
              if (LCDContrast == LCDContrastMaxValue) {
                return;
              }
              LCDContrast += 5;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              analogWrite(LCDcontrastPin, LCDContrast);
              EEPROM.update(LCDContrastAddress, LCDContrast);
              break;
            }
          case 3:
            {
              if (LCDBrightness == LCDbrightnessMaxValue) {
                return;
              }
              LCDBrightness += 15;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              analogWrite(LCDBrightnessPin, LCDBrightness);
              EEPROM.update(LCDBrightnessAddress, LCDBrightness);
              break;
            }
          case 4:
            {
              if (MTXBrightness == matrixbrightnessMaxValue) {
                return;
              }
              MTXBrightness++;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              lc.setIntensity(0, MTXBrightness);
              EEPROM.update(MTXBrightnessAddress, MTXBrightness);
              break;
            }
          case 5:
            {
              sound = !sound;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              EEPROM.update(soundAddress, sound);
              break;
            }
        }
        break;
      }
    case down:
      {
        switch (currentSettingsPosition) {
          case 1:
            {
              if (gameDifficulty == 0) {
                return;
              }
              gameDifficulty--;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              EEPROM.update(gameDifficultyAddress, gameDifficulty);
              break;
            }
          case 2:
            {
              // EEPROM.get(MTXBrightnessAddress, MTXBrightness);
              // EEPROM.get(soundAddress, sound);
              if (LCDContrast == 0) {
                return;
              }
              LCDContrast -= 5;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              analogWrite(LCDcontrastPin, LCDContrast);
              EEPROM.update(LCDContrastAddress, LCDContrast);
              break;
            }
          case 3:
            {
              if (LCDBrightness == 0) {
                return;
              }
              LCDBrightness -= 15;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              analogWrite(LCDBrightnessPin, LCDBrightness);
              EEPROM.update(LCDBrightnessAddress, LCDBrightness);
              break;
            }
          case 4:
            {
              if (MTXBrightness == 0) {
                return;
              }
              MTXBrightness--;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              lc.setIntensity(0, MTXBrightness);
              EEPROM.update(MTXBrightnessAddress, MTXBrightness);
              break;
            }
          case 5:
            {
              sound = !sound;
              printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, true);
              EEPROM.update(soundAddress, sound);
              break;
            }
        }
        break;
      }
  }
  completeSettingsWithValues();
}

void implementHighscoresMovement(byte movement) {
  switch (movement) {
    case up:
      {
        if (currentHighscorePosition == 0) {
          return;
        }
        currentHighscorePosition--;
        printItemsForCurrentPosition(highScoresNames, "Position: " + String(currentHighscorePosition + 1), currentHighscorePosition, false);
        break;
      }
    case down:
      {
        if (currentHighscorePosition == numberOfHighscores - 1) {
          return;
        }
        currentHighscorePosition++;
        printItemsForCurrentPosition(highScoresNames, "Position: " + String(currentHighscorePosition + 1), currentHighscorePosition, false);
        break;
      }
  }
}

void completeSettingsWithValues() {
  switch (currentSettingsPosition) {
    case 1:
      {
        lcd.print(": ");
        if (gameDifficulty == 0) {
          lcd.print("Easy");
          break;
        }
        if (gameDifficulty == 1) {
          lcd.print("Medium");
          break;
        }
        lcd.print("Hard");
        break;
      }
    case 2:
      {
        lcd.print(": " + String(LCDContrast));
        break;
      }
    case 3:
      {
        lcd.print(": " + String(LCDBrightness));
        break;
      }
    case 4:
      {
        lcd.print(": " + String(MTXBrightness));
        break;
      }
    case 5:
      {
        lcd.print(": ");
        if (sound == 1)
          lcd.print("On");
        else lcd.print("Off");
        break;
      }
  }
}

void implementXMovement(bool continous = false) {
  if (xValueJoystick > upperLimit) {
    if (lastXState != 1 || continous) {
      valueLeftDefaultRange(right);
    }
    lastXState = 1;
    return;
  }
  if (xValueJoystick < lowerLimit) {
    if (lastXState != -1 || continous) {
      valueLeftDefaultRange(left);
    }
    lastXState = -1;
    return;
  }
  if (xValueJoystick > defaultThresholdLower && xValueJoystick < defaultThresholdUpper) {
    lastXState = 0;
  }
}

void implementYMovement(bool continous = false) {
  if (yValueJoystick > upperLimit) {
    if (lastYState != 1 || continous) {
      valueLeftDefaultRange(down);
    }
    lastYState = 1;
    return;
  }
  if (yValueJoystick < lowerLimit) {
    if (lastYState != -1 || continous) {
      valueLeftDefaultRange(up);
    }
    lastYState = -1;
    return;
  }
  if (yValueJoystick > defaultThresholdLower && yValueJoystick < defaultThresholdUpper) {
    lastYState = 0;
  }
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
    handleButtonPress();
  }

  if (lastButtonInputChange && millis() - lastButtonInputChange > longPressThreshold) {
    lastButtonInputChange = 0;
    handleButtonPress();
  }

  previousSwReading = swState;
}

void takeInputBasedOnState() {
  if (appState == aboutState || appState == howToPlayState) {
    implementXMovement(true);
    return;
  }
  if (appState == gameState) {
    implementXMovement(false);
    implementYMovement(false);
    return;
  }

  if (appState != 0) {
    implementYMovement(false);
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

void handleButtonPress() {
  if (appState == menuState) {
    implementMenuButtonPress();
    return;
  }
  if (appState == settingsState || appState == settingsClickedState) {
    implementSettingsButtonPress();
    return;
  }
  if (appState == greetingState) {
    appState = menuState;
    printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
    return;
  }
  if (appState == aboutState || appState == howToPlayState) {
    appState = menuState;
    printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
    longTextCurrentPosition = 0;
    return;
  }
  if (appState == highscoreState) {
    appState = menuState;
    printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
    currentHighscorePosition = 0;
    return;
  }
  if (appState == gameOverState) {
    appState = menuState;
    setMatrixToDefaultDisplay();
    printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
    return;
  }
}

void implementMenuButtonPress() {
  switch (currentMenuPosition) {
    case 0:
      {
        printFirstAndSecondLineForLCD("Name: " + userName, "Score: ");
        lcd.print(score);
        appState = gameState;
        for (int row = 0; row < matrixSize; row++) {
          for (int col = 0; col < matrixSize; col++) {
            lc.setLed(0, row, col, false);
          }
        }
        updateMatrix();
        return;
      }
    case 1:
      {
        appState = highscoreState;
        printItemsForCurrentPosition(highScoresNames, "Position: " + String(currentHighscorePosition + 1), currentHighscorePosition, false);
        return;
      }
    case 2:
      {
        appState = settingsState;
        printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, false);
        return;
      }
    case 3:
      {
        appState = aboutState;
        printFirstAndSecondLineForLCD("Creator: Safta Adelin Gabriel", "Github: adesafta2002 | Game Name: Snake");
        return;
      }
    case 4:
      {
        appState = howToPlayState;
        printFirstAndSecondLineForLCD("1.Move using joystick & eat food to grow", "2.Avoid walls and your tail!");
        return;
      }
  }
}

void implementSettingsButtonPress() {
  if (appState != settingsClickedState) {
    appState = settingsClickedState;
  } else {
    appState = settingsState;
  }
  switch (currentSettingsPosition) {
    case 0:
      {
        if (appState == settingsClickedState) {
          printFirstAndSecondLineForLCD("  -Enter Name-  ", "*Type in serial*");
        } else {
          printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, false);
        }
        return;
      }
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      {
        const bool replaceWithArrow = appState == settingsClickedState ? true : false;
        printItemsForCurrentPosition(settingsItems, "   -SETTINGS-   ", currentSettingsPosition, replaceWithArrow);
        completeSettingsWithValues();
        return;
      }
    case 6:
      {
        appState = menuState;
        currentSettingsPosition = 0;
        printItemsForCurrentPosition(menuItems, "     -MENU-     ", currentMenuPosition, false);
        return;
      }
  }
}

void updateMatrix() {
  lc.setLed(0, snakeHeadYPosition, snakeHeadXPosition, true);
  lc.setLed(0, foodYPosition, foodXPosition, true);
}

void updateHighScores() {
  if (score <= highScores[numberOfHighscores - 1]) {
    score = 0;
    return;
  }
  // int insertPosition = 255;

  // for (int i = 0; i < numberOfHighscores; i++) {
  //   if (highScores[i] < score) {
  //     insertPosition = i;
  //     break;
  //   }
  // }
  // Serial.print(insertPosition);

  // for (int i = numberOfHighscores - 1; i > insertPosition; i++) {
  //   highScores[i] = highScores[i - 1];
  //   highScoresNames[i] = highScoresNames[i - 1];
  // }
  // highScores[insertPosition] = score;
  // highScoresNames[insertPosition] = userName;
  // saveNewHighscoresToEEPROM();
  score = 0;
}

void gameOver() {
  appState = gameOverState;
  snakeHeadXPosition = 0;
  snakeHeadYPosition = 0;
  facing = defaultFacing;
  printFirstAndSecondLineForLCD("   Game  Over   ", "Your Score: " + String(score));
  updateHighScores();
  for (byte i = 0; i < matrixSize; i++) {
    for (byte j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(matrixGameOver[i], 7 - j));
    }
  }
  return;
}

void implementGameLogic() {
  if (facing == defaultFacing) {
    return;
  }
  if (millis() - lastHeadMove > moveHeadInterval / (gameDifficulty + 1)) {
    lc.setLed(0, snakeHeadYPosition, snakeHeadXPosition, false);
    switch (facing) {
      case up:
        {
          snakeHeadYPosition--;
          if (snakeHeadYPosition < 0) {
            gameOver();
            return;
          }
          break;
        }
      case down:
        {
          snakeHeadYPosition++;
          if (snakeHeadYPosition > 7) {
            gameOver();
            return;
          }
          break;
        }
      case right:
        {
          snakeHeadXPosition++;
          if (snakeHeadXPosition > 7) {
            gameOver();
            return;
          }
          break;
        }
      case left:
        {
          snakeHeadXPosition--;
          if (snakeHeadXPosition < 0) {
            gameOver();
            return;
          }
          break;
        }
    }
    if (snakeHeadYPosition == foodYPosition && snakeHeadXPosition == foodXPosition) {
      score += 1 * (gameDifficulty + 1);
      lc.setLed(0, foodYPosition, foodXPosition, false);
      foodYPosition = random(1, 7);
      foodXPosition = random(1, 7);
    }
    printFirstAndSecondLineForLCD("Name: " + userName, "Score: ");
    lcd.print(score);
    lastHeadMove = millis();
    updateMatrix();
  }
}

void setup() {
  Serial.begin(9600);
  initializePinModes();
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.clearDisplay(0);
  printGreetingMessage();
  userName = readNameFromEEPROM(stringLengthAddress, nameStartAddress);
  readGameDetailsFromEEPROM();
  randomSeed(analogRead(A4));
  setMatrixToDefaultDisplay();
  // getHighScoresFromEEPROM();
}

void loop() {
  readJoystickInput();
  if (appState == settingsClickedState && currentSettingsPosition == 0) {
    changeUserName();
  }
  if (appState == gameState) {
    implementGameLogic();
  }
}