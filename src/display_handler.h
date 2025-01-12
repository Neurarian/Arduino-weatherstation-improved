// Functions to handle the controls and display for the Arduino UNO R4 WiFi
#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H


ArduinoLEDMatrix matrix;
ModulinoPixels leds;
ModulinoKnob encoder;
ModulinoButtons buttons;

int brightness = 5;
int dayHourLEDCount = 0;

int lastEncoderVal = 0;
int currentEncoderVal = 0;

bool buttonStates[3] = {false};
int activeButton = -1;

void updateLightsWeather(String period) {
  // The API gives us forecasts as weather codes
  // Loops through all the daily forecasts
  for (int i = 0; i < NUM_OF_FORECASTS; i++) {
    // Puts the current days weather forecast code in a variable to easier
    // compare
    int c;
    if (period == "hourly") {
      c = hourlyForecast[i].weatherCode;
    }
    if (period == "daily") {
      c = dailyForecast[i].weatherCode;
    }
    uint8_t r, g, b;
    ModulinoColor colour(r, g, b);
    // Clear weather conditions
    if (c < 4) {
      colour = GREEN;
    }
    // Rainy weather
    if ((c > 50 && c < 67) || (c < 82 && c >= 80)) {
      colour = BLUE;
    }
    // Foggy weather
    if (c == 45 || c == 48) {
      colour = VIOLET;
    }
    // Snowy weather
    if ((c < 78 && c > 70) || c == 85 || c == 86) {
      colour = WHITE;
    }
    // Thunder
    if (c == 95 || c == 96 || c == 99) {
      colour = RED;
    }
    if (dayHourLEDCount == i) {
      brightness = 20;
    } else {
      brightness = 5;
    }

    // Now lets turn on the light
    leds.set(i, colour, brightness);
  }
  leds.show();
}

void updateLightsUVPrecip(String period) {
  // The API gives us forecasts as weather codes
  // Loops through all the daily forecasts
  for (int i = 0; i < NUM_OF_FORECASTS; i++) {
    // Puts the current days weather forecast code in a variable to easier
    // compare
    int c;
    if (period == "precipHourly") {
      c = hourlyForecast[i].precipitationProb;
    }
    if (period == "uvDaily") {
      c = (dailyForecast[i].uvIndex / 11) * 100;
    }
    uint8_t r, g, b;
    ModulinoColor colour(r, g, b);
    ModulinoColor yellow(255, 255, 0);
    ModulinoColor orange(255, 165, 0);
    // Clear weather conditions
    if (c < 30) {
      colour = GREEN;
    }
    // Rainy weather
    if (c > 30 && c < 60) {
      colour = yellow;
    }
    // Foggy weather
    if (c > 60 && c < 70) {
      colour = orange;
    }
    // Snowy weather
    if (c > 70 && c < 90) {
      colour = RED;
    }
    // Thunder
    if (c > 90) {
      colour = VIOLET;
    }
    if (dayHourLEDCount == i) {
      brightness = 20;
    } else {
      brightness = 5;
    }

    // Now lets turn on the light
    leds.set(i, colour, brightness);
  }
  leds.show();
}

int readEncoder() {
  lastEncoderVal = currentEncoderVal;
  currentEncoderVal = encoder.get();
  if (currentEncoderVal > lastEncoderVal) {
    return 1;
  } else if (currentEncoderVal < lastEncoderVal) {
    return -1;
  } else {
    return 0;
  }
}

String getDisplayText(String period) {
  bool isDaily = (period == "daily" || period == "windDaily");
  bool isWind = (period == "windDaily" || period == "windHourly");

  if (isDaily) {
    if (isWind) {
      return String(int(round(dailyForecast[dayHourLEDCount].windspeed))) +
             "km/h";
    } else {
      return String(int(
                 round(encoder.isPressed()
                           ? dailyForecast[dayHourLEDCount].temperatureMin
                           : dailyForecast[dayHourLEDCount].temperatureMax))) +
             "°C";
    }
  } else {
    if (isWind) {
      return String(int(round(hourlyForecast[dayHourLEDCount].windspeed))) +
             "km/h";
    } else {
      return String(
                 int(round(hourlyForecast[dayHourLEDCount].temperatureCurr))) +
             "°C";
    }
  }
}

void updateDisplay(String period) {
  String text = getDisplayText(period);

  char drawText[6]; // Increased size to accommodate "km/h"
  text.toCharArray(drawText, 6);

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(80);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 2, 0xFFFFFF);
  matrix.println(drawText);
  matrix.endText();
  matrix.endDraw();
}

void toggleButton(int buttonIndex) {
  if (activeButton == buttonIndex) {
    buttonStates[buttonIndex] = false;
    activeButton = -1;
    buttons.setLeds(false, false, false); // Turn off all LEDs
  } else {
    if (activeButton != -1) {
      buttonStates[activeButton] = false;
    }
    buttonStates[buttonIndex] = true;
    activeButton = buttonIndex;
    
    // Turn on the LED for the active button
    bool led1 = (buttonIndex == 0);
    bool led2 = (buttonIndex == 1);
    bool led3 = (buttonIndex == 2);
    buttons.setLeds(led1, led2, led3);
  }
}

#endif
