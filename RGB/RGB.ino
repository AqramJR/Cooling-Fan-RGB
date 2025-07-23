#define RED_PIN    0  // PB0 - PWM Supported
#define GREEN_PIN  1  // PB1 - PWM Supported
#define BLUE_PIN   4  // PB4 - PWM Supported
#define BUTTON_PIN 2  // Button to cycle through modes

int currentMode = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
unsigned long longPressDuration = 1000;
unsigned long buttonPressTime = 0;
bool buttonPressed = false; // Track button state
int breathingColor = 0;
unsigned long previousMillis = 0;  // Stores the last time the effect was updated
const long fadeInterval = 20;      // Interval to update the LED (in milliseconds)
const long offInterval = 1000;     // Interval to keep the LED off (in milliseconds)
int effectPhase = 0;               // 0: fading in, 1: fading out, 2: off
int fadeValue = 0;                 // Current fade value (0 to 255)
int colorIndex = 0;                // Index for the color array

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Use internal pull-up resistor
}

void loop() {
  // Button press handling
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressed = true;
      buttonPressTime = millis();  // Record the time when the button is pressed
    }
  } else {
    if (buttonPressed) {
      buttonPressed = false;
      if ((millis() - buttonPressTime) > longPressDuration) {
        lightFlickerRed();
        delay(120);
        currentMode = 12;  // Set to 12 for long press
      } else {
        lightFlicker();
        currentMode++;
        if (currentMode > 12) {  // Updated to 12 to match the number of cases
          currentMode = 0;
        }
      }
    }
  }

  // Execute current mode
  switch (currentMode) {
    case 0:
      rainbowEffect();  // Rainbow effect
      break;
    case 1:
      breathingEffect();  // Breathing effect with color change
      break;
    case 2:
      staticColor(255, 0, 0);  // Static Red
      break;
    case 3:
      staticColor(0, 255, 0);  // Static Green
      break;
    case 4:
      staticColor(0, 0, 255);  // Static Blue
      break;
    case 5:
      staticColor(255, 255, 0);  // Static Yellow
      break;
    case 6:
      staticColor(255, 165, 0);  // Static Orange
      break;
    case 7:
      staticColor(255, 0, 255);  // Static Magenta
      break;
    case 8:
      staticColor(0, 255, 255);  // Static Cyan
      break;
    case 9:
      staticColor(238, 130, 238);  // Static Violet
      break;
    case 10:
      staticColor(75, 0, 130);  // Static Indigo
      break;
    case 11:
      staticColor(255, 255, 255);  // Static White
      break;
    case 12:
      staticColor(0, 0, 0);  // LEDs Off
      break;
  }
}

// Static Color Function
void staticColor(int redValue, int greenValue, int blueValue) {
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);
}

// Light Flicker Effect
void lightFlicker() {
  for (int i = 0; i < 3; i++) { // Flicker 3 times
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 0);
    delay(50); // On
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    delay(50); // Off
  }
}

// Light Flicker Effect (Red)
void lightFlickerRed() {
  for (int i = 0; i < 3; i++) { // Flicker 3 times
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    delay(50); // On
    analogWrite(RED_PIN, 0);
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    delay(50); // Off
  }
}

// Rainbow Effect
// Rainbow Effect (Slowed Down)
void rainbowEffect() {
  static unsigned long lastMillis = 0;
  const unsigned long updateInterval = 100;  // Slower interval, 100 milliseconds

  if (millis() - lastMillis > updateInterval) {
    static int colorIndex = 0;
    static int phase = 0;

    if (phase == 0) {
      analogWrite(RED_PIN, 255 - colorIndex);
      analogWrite(GREEN_PIN, colorIndex);
      analogWrite(BLUE_PIN, 0);
    } else if (phase == 1) {
      analogWrite(RED_PIN, 0);
      analogWrite(GREEN_PIN, 255 - colorIndex);
      analogWrite(BLUE_PIN, colorIndex);
    } else if (phase == 2) {
      analogWrite(RED_PIN, colorIndex);
      analogWrite(GREEN_PIN, 0);
      analogWrite(BLUE_PIN, 255 - colorIndex);
    }

    colorIndex = (colorIndex + 1) % 256;
    if (colorIndex == 0) {
      phase = (phase + 1) % 3;
    }

    lastMillis = millis();
  }
}


// Breathing Effect (Modified with Proper Color Reset)
void breathingEffect() {
  unsigned long currentMillis = millis();

  switch (effectPhase) {
    case 0: // Fading in
      if (currentMillis - previousMillis >= fadeInterval) {
        previousMillis = currentMillis;
        fadeValue++;
        if (fadeValue >= 255) {
          fadeValue = 255;
          effectPhase = 1; // Move to fading out
        }
        updateLED(fadeValue);
      }
      break;

    case 1: // Fading out
      if (currentMillis - previousMillis >= fadeInterval) {
        previousMillis = currentMillis;
        fadeValue--;
        if (fadeValue <= 0) {
          fadeValue = 0;
          effectPhase = 2; // Move to off
          previousMillis = currentMillis; // Reset time for the off phase
        }
        updateLED(fadeValue);
      }
      break;

    case 2: // LED off
      if (currentMillis - previousMillis >= offInterval) {
        previousMillis = currentMillis;
        effectPhase = 0; // Restart the effect
        colorIndex = (colorIndex + 1) % 11; // Move to next color in the array
        if (colorIndex > 7) {  // Reset after the last color
          colorIndex = 0;  // Start back at Red
        }
      }
      break;
  }
}

void updateLED(int brightness) {
  int colors[8][3] = {
    {255, 0, 0},    // Red
    {0, 255, 0},    // Green
    {0, 0, 255},    // Blue
    {255, 165, 0},  // Orange
    {255, 0, 255},  // Magenta
    {0, 255, 255},  // Cyan
    {148, 0, 211},  // Purple
    {255, 255, 255} // White
  };

  // Properly calculate brightness scaling for each color component
  int redValue = colors[colorIndex][0] * brightness / 255;
  int greenValue = colors[colorIndex][1] * brightness / 255;
  int blueValue = colors[colorIndex][2] * brightness / 255;

  // Update PWM values for the ATtiny85
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);
  analogWrite(BLUE_PIN, blueValue);
}
