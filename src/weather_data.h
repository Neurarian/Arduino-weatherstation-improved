// Functions to fetch and parse the open-meteo data

#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

const int NUM_OF_FORECASTS = 8;

struct WeatherForecast {
  double temperatureCurr;
  double temperatureMax;
  double temperatureMin;
  int weatherCode;
  int precipitationProb;
  double windspeed;
  double uvIndex;
};

WeatherForecast dailyForecast[NUM_OF_FORECASTS];

WeatherForecast hourlyForecast[NUM_OF_FORECASTS];

void parseWeatherData(const String &json, String period, int currentHour) {
  StaticJsonDocument<16384> doc;
  deserializeJson(doc, json);

  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // This loop will look through the JSON response and find the lines
  // that contain the temperature and weather codes,
  // which will then be placed in arrays so they can be easily accessed
  if (period == "daily") {
    for (int i = 0; i < NUM_OF_FORECASTS; i++) {
      Serial.println("Weather data for day " + String(i) + ":");
      dailyForecast[i].weatherCode = doc["daily"]["weather_code"][i];
      Serial.println("Weather code: " + String(dailyForecast[i].weatherCode));
      dailyForecast[i].temperatureMax = doc["daily"]["temperature_2m_max"][i];
      Serial.println("Max temp: " + String(dailyForecast[i].temperatureMax) +
                     "°C");
      dailyForecast[i].temperatureMin = doc["daily"]["temperature_2m_min"][i];
      Serial.println("Min temp: " + String(dailyForecast[i].temperatureMin) +
                     "°C");
      dailyForecast[i].uvIndex = doc["daily"]["uv_index_max"][i];
      Serial.println("UV index: " + String(dailyForecast[i].uvIndex));
      dailyForecast[i].windspeed = doc["daily"]["wind_speed_10m_max"][i];
      Serial.println("Max wind speed: " + String(dailyForecast[i].windspeed));
      Serial.println();
    }
  }
  if (period == "hourly") {
    for (int i = 0; i < NUM_OF_FORECASTS; i++) {
      Serial.println("Weather data for hour " + String(i + currentHour) + ":");
      hourlyForecast[i].weatherCode =
          doc["hourly"]["weather_code"][i + currentHour];
      Serial.println("Weather code: " + String(hourlyForecast[i].weatherCode));
      hourlyForecast[i].temperatureCurr =
          doc["hourly"]["temperature_2m"][i + currentHour];
      Serial.println(
          "Current temp: " + String(hourlyForecast[i].temperatureCurr) + "°C");
      hourlyForecast[i].precipitationProb =
          doc["hourly"]["precipitation_probability"][i + currentHour];
      Serial.println("Precipitation Probability: " +
                     String(hourlyForecast[i].precipitationProb) + "%");
      hourlyForecast[i].windspeed =
          doc["hourly"]["wind_speed_10m"][i + currentHour];
      Serial.println("Wind speed: " + String(hourlyForecast[i].windspeed) +
                     "km/h");
      Serial.println();
    }
  }
}

void getWeatherData(float latitude, float longitude, int currentHour) {
  int httpPort = 80;
  char serverAddress[] = "api.open-meteo.com"; // server address
  WiFiClient wifi;
  HttpClient client = HttpClient(wifi, serverAddress, httpPort);

  // This loop will look through the JSON response and find the lines
  // that contain the temperature and weather codes,
  // which will then be placed in arrays so they can be easily accessed
  String apiRequests[] = {
      "&daily=weather_code,temperature_2m_max,temperature_2m_min,uv_index_max,"
      "wind_speed_10m_max&forecast_days=8",
      "&hourly=temperature_2m,precipitation_probability,weather_code,wind_"
      "speed_10m&forecast_days=2"};
  for (int i = 0; i < sizeof(apiRequests) / sizeof(apiRequests[0]); i++) {

    client.get("/v1/forecast?latitude=" + String(latitude) +
               "&longitude=" + String(longitude) + apiRequests[i]);

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        client.stop();
        return;
      }
    }
    int statusCode = client.responseStatusCode();
    Serial.println(statusCode);
    // The entire response from the API in JSON is then stored in "result"
    String response = client.responseBody();

    // To only get the information that is needed, the JSON response will be
    // parsed using a parsing function
    parseWeatherData(response, i == 0 ? "daily" : "hourly", currentHour);
  }
}

#endif
