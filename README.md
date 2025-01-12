This fun little project largely expands on: https://courses.arduino.cc/plugandmake/lessons/project-weather-report/. But without the cloud requirement.

Connects to home wifi and fetches weather data.
Modulino LED array indicates day 0 to 8.

Green = Sunny

Blue = Rainy

Violet = Foggy

White = Snow

Red = Thunder

Uses Modulino Knob to select day and toggle min/max temps.
Uses Modulino Buttons to switch to hourly forecast (Button A), hourly precipitation & windspeed (Button B), or weekly UV index & windspeed (Button C).
Instead of relying on the cloud function, the R4 runs a simple webserver for remote control of the desired weather coordinates using the Leaflet maps API.
Remote coordinate updates are indicated by a Modulino Buzzer sound.
