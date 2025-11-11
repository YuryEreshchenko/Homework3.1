/*
  RGB LED Multi-Mode Controller
  -----------------------------
  Controls a common-anode RGB LED using one push button.
  - Short press: changes LED color
  - Long press: cycles through light modes

  Modes:
    0 → Static (solid color)
    1 → Blink (on/off at regular interval)
    2 → Fade (smooth breathing effect)
    3 → Strobe (rapid flashing)

  Author: Yury Ereshchenko
  Date: 12/11/2025
*/


// Pin definitions
const int RedLEDPin   = 9;
const int GreenLEDPin = 10;
const int BlueLEDPin  = 11;

const int pushButton  = 2;

// Button state variables
int buttonState           = 0;
unsigned long pressingTime = 0;
const long longPressInterval = 1000;
bool buttonPressed        = false;

// Color / mode tracking
int ledCounter = 1;  // Tracks current color (0–4)

// Mode 0 : Constant Light
// Mode 1 : Blink mode
// Mode 2 : Breathing (fading) light
int currentMode = 0;

// Blink mode variables
unsigned long blinkTimer = 0;
const int blinkInterval  = 500;
bool blinkOn             = true;

// Current color values
int RCurrentColor = 0;
int GCurrentColor = 0;
int BCurrentColor = 0;

// Fading mode variables
unsigned long fadeTimer  = 0;
const int fadeInterval   = 15;  // Update every 15ms for smooth fading
int fadeBrightness       = 0;
int fadeDirection        = 1;   // 1 for increasing, -1 for decreasing
const int fadeStep       = 5;   // How much to change brightness each step

// Strobe mode variables
unsigned long strobeTimer = 0;
const int strobeOnTime = 80;   // LED ON duration 
const int strobeOffTime = 80;  // LED OFF duration
bool strobeOn = false;



void setup() {
  Serial.begin(9600);

  pinMode(RedLEDPin, OUTPUT);
  pinMode(GreenLEDPin, OUTPUT);
  pinMode(BlueLEDPin, OUTPUT);

  pinMode(pushButton, INPUT);
}


void loop() {
  int clickType = checkButton();

  if (clickType == 1) {
    changeLEDColor();
  } 
  
  else if (clickType == 2) {
    currentMode++;
    if (currentMode >= 4) {
      currentMode = 0;
    }

    if (currentMode == 1) {
      blinkTimer = 0;
      blinkOn = true;
    }
  }

  updateLEDColor(currentMode);
}


// Updates LED according to current mode
void updateLEDColor(int currentMode) {

  // Static mode
  if (currentMode == 0) {
    Serial.println("Mode 0 : Static light");
    // Nothing else — static light
  }

  // Blink mode
  else if (currentMode == 1) {
    Serial.println("Mode 1 : Blink");

    unsigned long currentTime = millis();

    if (currentTime - blinkTimer > blinkInterval) {
      blinkOn = !blinkOn;
      blinkTimer = currentTime;
    }

    if (blinkOn == true) {
      PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
    } 
    else {
      PowerLEDControl(255, 255, 255);
    }
  }

  // Fading mode
  else if (currentMode == 2) {
    unsigned long currentTime = millis();

    if (currentTime - fadeTimer > fadeInterval) {
      fadeBrightness += fadeDirection * fadeStep;

      // Reverse direction at the limits
      if (fadeBrightness >= 255) {
        fadeBrightness = 255;
        fadeDirection  = -1;
      } else if (fadeBrightness <= 0) {
        fadeBrightness = 0;
        fadeDirection  = 1;
      }

      fadeTimer = currentTime;

      // Apply fading to current color
      int fadeR = map(fadeBrightness, 0, 255, 255, RCurrentColor);
      int fadeG = map(fadeBrightness, 0, 255, 255, GCurrentColor);
      int fadeB = map(fadeBrightness, 0, 255, 255, BCurrentColor);

      PowerLEDControl(fadeR, fadeG, fadeB);
    }
  }

  else if (currentMode == 3){
      unsigned long currentTime = millis();

  
  if (strobeOn && (currentTime - strobeTimer > strobeOnTime)) {
    PowerLEDControl(255, 255, 255);  // Off (full white = off in your color logic)
    strobeOn = false;
    strobeTimer = currentTime;
  }

  
  else if (!strobeOn && (currentTime - strobeTimer > strobeOffTime)) {
    PowerLEDControl(RCurrentColor, GCurrentColor, BCurrentColor);
    strobeOn = true;
    strobeTimer = currentTime;
  }
  }
}


// Cycle through LED colors on short click
void changeLEDColor() {
  ledCounter++;

  if (ledCounter == 5) {
    ledCounter = 0;
  }

  if (ledCounter == 0) {
    PowerLEDControl(0, 0, 0);
    RCurrentColor = 0;
    GCurrentColor = 0;
    BCurrentColor = 0;
  }

  if (ledCounter == 1) {
    PowerLEDControl(0, 255, 255);
    RCurrentColor = 0;
    GCurrentColor = 255;
    BCurrentColor = 255;
  }

  if (ledCounter == 2) {
    PowerLEDControl(255, 0, 255);
    RCurrentColor = 255;
    GCurrentColor = 0;
    BCurrentColor = 255;
  }

  if (ledCounter == 3) {
    PowerLEDControl(255, 255, 0);
    RCurrentColor = 255;
    GCurrentColor = 255;
    BCurrentColor = 0;
  }

  if (ledCounter == 4) {
    PowerLEDControl(255, 255, 255);
    RCurrentColor = 255;
    GCurrentColor = 255;
    BCurrentColor = 255;
  }
}


// Detects short vs long press
int checkButton() {
  int clickType = 0;

  buttonState = !digitalRead(pushButton);
  delay(5);

  if (buttonState == HIGH && !buttonPressed) {
    pressingTime  = millis();
    buttonPressed = true;
  }

  if (buttonState == LOW && buttonPressed) {
    unsigned long currentTime = millis();

    if (currentTime - pressingTime < longPressInterval) {
      Serial.println("Short click");
      clickType = 1;
    } 
    else {
      Serial.println("Long Press");
      clickType = 2;
    }

    buttonPressed = false;
  }

  return clickType;
}



// Write RGB values to the LED pins
void PowerLEDControl(int red, int green, int blue) {
  analogWrite(RedLEDPin,   red);
  analogWrite(GreenLEDPin, green);
  analogWrite(BlueLEDPin,  blue);
}
