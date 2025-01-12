/*
  Builds upon the Arduino Plug and Make Kit tutorial but without the Cloud requirement:
  https://courses.arduino.cc/plugandmake/lessons/project-weather-report/

  Connects to home wifi and fetch weather data.
  Modulino LED array indicates day 0 to 8.
  Green = Sunny
  Blue = Rainy
  Violet = Foggy
  White = Snow
  Red = Thunder
  Uses Modulino Knob to select day and toggle min/max temps.
  Uses Modulino Buttons to Switch to hourly forecast (Button A), hourly precipitation & windspeed (Button B), or weekly UV index & windspeed (Button C).
  Instead of relying on the cloud function, the R4 runs a simple webserver for remote control of the desired weather coordinates using the Leaflet maps API.
  Remote Coordinate updates are indicated by a Modulino Buzzer sound.
 */

#include <WiFiS3.h>
#include <math.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>
#include <Modulino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RTC.h>
#include "src/arduino_secrets.h" 
#include "src/weather_data.h" 
#include "src/wifi_handler.h" 
#include "src/time_handler.h" 
#include "src/webserver.h" 
#include "src/display_handler.h" 
#include "src/util.h" 


const unsigned long NTP_UPDATE_INTERVAL = 24 * 60 * 60 * 1000;

WiFiServer server(80);

int currentHour = 0;

unsigned long lastCallTime = 0;


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  RTC.begin();
  matrix.begin();
  Modulino.begin();
  leds.begin();
  encoder.begin();
  buzzer.begin();
  buttons.begin();

  buttons.setLeds(false, false, false);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  
  printCurrentNet();
  printWifiData();
  timeClient.begin();
  updateRTCFromNTP();

  RTC.getTime(currentTime);
  currentHour = currentTime.getHour();

  getWeatherData(latitude, longitude, currentHour);
  server.begin();
  }

void loop() {

  RTC.getTime(currentTime);
  currentHour = currentTime.getHour();
  
  // Update weather every hour 
  if((millis() - lastCallTime) > 3600000) {
  getWeatherData(latitude, longitude, currentHour);
  lastCallTime = millis();
  printTimestamp(lastCallTime);
  }

  // Update clock every day
  if ((millis() - lastNTPUpdate) > NTP_UPDATE_INTERVAL) {
    updateRTCFromNTP();
  }

  dayHourLEDCount = dayHourLEDCount + readEncoder();
  if(dayHourLEDCount < 0) {
    dayHourLEDCount = 0;
  }
  if(dayHourLEDCount >= NUM_OF_FORECASTS) {
    dayHourLEDCount = NUM_OF_FORECASTS-1;
  }
  if (activeButton == -1) {
    updateLightsWeather("daily");
    updateDisplay("daily");
  }

  if (activeButton == 0) {
    updateLightsWeather("hourly");
    updateDisplay("hourly");
  }

  if (activeButton == 1) {
    updateLightsUVPrecip("precipHourly");
    updateDisplay("windHourly");
  }

  if (activeButton == 2) {
    updateLightsUVPrecip("uvDaily");
    updateDisplay("windDaily");
  }

 if (buttons.update()) {
    if (buttons.isPressed(0)) {
      toggleButton(0);
    } else if (buttons.isPressed(1)) {
      toggleButton(1);
    } else if (buttons.isPressed(2)) {
      toggleButton(2);
    }
 }
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            sendHtmlResponse(client);
            break;
          } 
          else {
            currentLine = "";
          }
          }
        else if (c != '\r') {
          currentLine += c;
        }
        
        // Check if the client is updating coordinates
        if (currentLine.startsWith("GET /update?lat=") && currentLine.indexOf(" HTTP") != -1) {
          handleCoordinateUpdate(client, currentLine);
          getWeatherData(latitude, longitude, currentHour);
          playMelody(melody);
          break;
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

