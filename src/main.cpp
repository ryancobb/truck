#include "Arduino.h"
#include "FastLED.h"
#include "M5CoreInk.h"
#include "UNIT_ENCODER.h"

#define NUM_LEDS 144
#define DATA_PIN 25

CRGB colors[] = {CRGB::FairyLightNCC, CRGB::WhiteSmoke, CRGB::DarkRed};
CRGB leds[NUM_LEDS];

Ink_Sprite InkPageSprite(&M5.M5Ink);
UNIT_ENCODER encoder;

bool colorPickMode = false;
bool currentButtonState;
bool lastButtonState;
bool powerStatus = 0;

int bounceTime = 50;
int colorSelect = 0;
int doubleTime = 500;
int held = 0;
int holdTime = 250;
int encoderSampleRate = 1000;

long lastSwitchTime = 0;
long pressTime = 0;
long encoderSampleTime;

signed short int currentEncoderValue;
signed short int lastEncoderValue;

uint8_t brightnessStep = 10;
uint8_t currentBrightness;
uint8_t maxBrightness = 170;
uint8_t powerOnBrightness = 120;

void setup() {
  M5.begin();
  M5.M5Ink.clear();

  delay(500);

  InkPageSprite.drawString(20, 20, "Booted successfully :)");
  InkPageSprite.pushSprite();

  encoder.begin();
  currentButtonState = encoder.getButtonStatus();
  currentEncoderValue = encoder.getEncoderValue();
  encoder.setLEDColor(0, CRGB::Black);

  currentBrightness = powerOnBrightness;
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
}

void setColor() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = colors[colorSelect];
  }

  FastLED.show();
}

void nextColor() {
  if (colorSelect == 2) {
    colorSelect = 0;
  } else {
    colorSelect++;
  }

  setColor();
}

void previousColor() {
  if (colorSelect == 0) {
    colorSelect = 2;
  } else {
    colorSelect--;
  }

  setColor();
}

void brightenLeds() {
  currentBrightness =
      constrain(brighten8_lin(currentBrightness), 0, maxBrightness);
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
}

void dimLeds() {
  currentBrightness = constrain(dim8_lin(currentBrightness), 0, maxBrightness);
  FastLED.setBrightness(currentBrightness);
  FastLED.show();
}

void handleEncoder() {
  if (colorPickMode) {
    if ((millis() - encoderSampleTime) > encoderSampleRate) {
      currentEncoderValue = encoder.getEncoderValue();

      if (currentEncoderValue > lastEncoderValue) {
        nextColor();
      } else if (currentEncoderValue < lastEncoderValue) {
        previousColor();
      }

      lastEncoderValue = currentEncoderValue;
      encoderSampleTime = millis();
    }
  } else {
    lastEncoderValue = currentEncoderValue;
    currentEncoderValue = encoder.getEncoderValue();

    if (currentEncoderValue > lastEncoderValue) {
      brightenLeds();
    } else if (currentEncoderValue < lastEncoderValue) {
      dimLeds();
    }
  }
}

void enterColorPickMode() {
  colorPickMode = true;
  encoder.setLEDColor(0, CRGB::Red);
}

void exitColorPickMode() {
  colorPickMode = false;
  encoder.setLEDColor(0, CRGB::Black);
}

void turnOn() {
  setColor();
}

void turnOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  FastLED.show();
}

void handleButton() {
  lastButtonState = currentButtonState;
  currentButtonState = encoder.getButtonStatus();

  // first pressed
  if (currentButtonState == 0 && lastButtonState == 1) {
    pressTime = millis();
  }

  // held
  if (powerStatus && currentButtonState == 0 && lastButtonState == 0) {
    if ((millis() - pressTime) > holdTime) {
      held = 1;

      enterColorPickMode();
    }
  }

  // released
  if (currentButtonState == 1 && lastButtonState == 0) {
    if (millis() - holdTime > bounceTime) {
      if (held == 1) {
        held = 0;
      } else {
        if (colorPickMode) {
          exitColorPickMode();
        } else {
          powerStatus = !powerStatus;

          if (powerStatus) {
            turnOn();
          } else {
            turnOff();
          }
        }
      }
    }
  }
}

void loop() {
  handleEncoder();
  handleButton();

  M5.update();

  delay(50);
}
