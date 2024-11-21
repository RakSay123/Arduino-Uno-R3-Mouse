#include <Joystick.h>

// Pin declarations
const int buttonPins[] = {3, 4, 5, 6};
const int buzzerPin = 7;
const int ledPins[] = {8, 9, 10}; // LEDs for mode indication

// Pin declarations for joystick module
const int joystickXPin = A0; // X-direction movement input
const int joystickYPin = A1; // Y-direction movement input
const int joystickButtonPin = 2; // Joystick button press

// Mode status
enum Mode {NORMAL, SPECIAL, SPECIAL_SQUARED};
Mode currentMode = NORMAL;

// Timing variables for debouncing
unsigned long lastDebounceTime[4] = {0, 0, 0, 0};
const unsigned long debounceDelay = 50; // 50 ms debounce delay
bool buttonStates[4] = {false, false, false, false}; // Track current button states

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(buzzerPin, OUTPUT);
  
  // Initialize joystick pins
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(joystickButtonPin, INPUT_PULLUP);
  digitalWrite(8, HIGH); // Red
}

void loop() {
  // Read button states and send corresponding signals over serial
  for (int i = 0; i < 4; i++) {
    int currentButtonState = digitalRead(buttonPins[i]);
    if (currentButtonState == LOW && !buttonStates[i]) { // Button pressed and state not yet recorded
      if (millis() - lastDebounceTime[i] > debounceDelay) {
        buttonStates[i] = true;
        handleButtonPress(i);
        // Serial.print("B");
        // Serial.println(i + 1);
        digitalWrite(buzzerPin, HIGH);
        delay(100); // Brief delay for the buzzer
        digitalWrite(buzzerPin, LOW);
        lastDebounceTime[i] = millis(); // Reset debounce timer
      }
    }
    else if (currentButtonState == HIGH) {
      buttonStates[i] = false; // Reset state when button is released
    }
  }

  // Read joystick button press
  if (digitalRead(joystickButtonPin) == LOW) {
    handleJoystickButtonPress();
    printFunction(4);
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
  }

  // Read Joystick movements and translate into mouse actions
  handleJoystickMovement();
  
  digitalWrite(7, LOW);
}


void handleJoystickMovement() {
  // Read joystick values
  int joyX = analogRead(joystickXPin);
  int joyY = analogRead(joystickYPin);

  // Map joystick values (0-1023) to mouse movements (-127 to 127)
  // Process joystick movement
  int mappedX = map(joyX, 0, 1023, -127, 127);
  int mappedY = map(joyY, 0, 1023, -127, 127);

  // Debugging for joystick movements
  Serial.print("Joystick X: ");
  Serial.print(mappedX);
  Serial.print("\tJoystick Y: ");
  Serial.println(mappedY);
  delay(500); // Delay between prints
}

void handleButtonPress(int buttonIndex) {
  printFunction(buttonIndex);
  switch (currentMode) {
    case NORMAL:
      if (buttonIndex == 0) {
        Serial.println("B1_DOWN"); // Left click
      }
      else if (buttonIndex == 1) {
        Serial.println("B2_DOWN"); // Right click
      }
      else if (buttonIndex == 2) {
        Serial.println("SCROLL:VERTICAL:TOGGLE");
      }
      else if (buttonIndex == 3) {
        Serial.println("MODE:SPECIAL");
        if (currentMode == SPECIAL) {
          changeMode(SPECIAL);
        }
      }
    break;

    case SPECIAL:
      if (buttonIndex == 0) {
        Serial.println("B1_DOUBLECLICK");
      }
      else if (buttonIndex == 1) {
        Serial.println("PAGE:BACK");
      }
      else if (buttonIndex == 2) {
        Serial.println("AUTO:START");
      }
      else if (buttonIndex == 3) {
        Serial.println("MODE:SPECIAL_SQUARED");
        changeMode(SPECIAL_SQUARED);
      }
    break;

    case SPECIAL_SQUARED:
      if (buttonIndex == 0) {
        Serial.println("PAGE:FORWARD");
      }
      else if (buttonIndex == 1) {
        Serial.println("AUTO:SPEED:INCREASE");
      }
      else if (buttonIndex == 2) {
        Serial.println("AUTO:STOP");
      }
      else if (buttonIndex == 3) {
        Serial.println("SCROLL:HORIZONTAL:TOGGLE");
      }
    break;
  }

  // Indicate which button was pressed for debugging
  /*Serial.print("Button ");
  Serial.print(buttonIndex + 1);
  Serial.println(" pressed");*/
}

void handleJoystickButtonPress() {
  if (currentMode == SPECIAL) {
    changeMode(NORMAL); // Exit "Special" mode to "Normal"
  }
  else if (currentMode == SPECIAL_SQUARED) {
    changeMode(SPECIAL); // Exit "Special squared" mode to "Special"
  }
}

void changeMode(Mode newMode) {
  // Prevent turning on multiple modes at the same time
  if (currentMode != newMode) {
    currentMode = newMode;
    updateLEDs();
    Serial.print("Mode changed to: ");
    switch (newMode) {
      case NORMAL:
        Serial.println("Normal");
      break;
      case SPECIAL:
        Serial.println("Special");
      break;
      case SPECIAL_SQUARED:
        Serial.println("Special squared");
      break;
    }
  }
}

void updateLEDs() {
  // Turn off all LEDs initially
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPins[i], LOW);
  }
  
  // Turn on the LED corresponding to the current mode
  switch (currentMode) {
    case NORMAL:
      digitalWrite(ledPins[0], HIGH); // Red LED
    break;
    case SPECIAL:
      digitalWrite(ledPins[1], HIGH); // Blue LED
    break;
    case SPECIAL_SQUARED:
      digitalWrite(ledPins[2], HIGH); // Yellow LED
    break;
  }
}

void printFunction(int buttonIndex) {
  switch (buttonIndex) {
    case 0: // Button 1
      switch (currentMode) {
        case NORMAL: 
          Serial.println("Button 1 - Left click");
        break;
        case SPECIAL:
          Serial.println("Button 1 - Double left click");
        break;
        case SPECIAL_SQUARED:
          Serial.println("Button 1 - Go forwards a page");
        break;
      }
      break;
    case 1: // Button 2
      switch (currentMode) {
        case NORMAL:
          Serial.println("Button 2 - Right click");
        break;
        case SPECIAL:
          Serial.println("Button 2 - Go back a page");
        break;
        case SPECIAL_SQUARED:
          Serial.println("Button 2 - Change speed of autoclicker");
        break;
      }
      break;
    case 2: // Button 3
      switch (currentMode) {
        case NORMAL:
          Serial.println("Button 3 - HOLD to scroll vertically");
        break;
        case SPECIAL:
          Serial.println("Button 3 - Begin autoclicker");
        break;
        case SPECIAL_SQUARED:
          Serial.println("Button 3 - Stop autoclicker");
        break;
      }
      break;
    case 3: // Button 4
      switch (currentMode) {
        case NORMAL:
          Serial.println("Button 4 - \"Special\" mode");
        break;
        case SPECIAL:
          Serial.println("Button 4 - \"Special squared\" mode");
        break;
        case SPECIAL_SQUARED:
          Serial.println("Button 4 - HOLD to scroll horizontally");
        break;
      }
      break;
    case 4: // Joystick button
      switch (currentMode) {
        case NORMAL:
          Serial.println("Joystick Button - Middle mouse click");
        break;
        case SPECIAL:
          Serial.println("Joystick Button - Return to normal mode");
        break;
        case SPECIAL_SQUARED:
          Serial.println("Joystick Button - Return to \"special\" mode");
        break;
      }
      break;
  }
}