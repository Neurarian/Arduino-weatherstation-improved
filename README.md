This fun little project largely expands on: https://courses.arduino.cc/plugandmake/lessons/project-weather-report/. But without the cloud requirement.

Connects to home wifi and fetches weather data.
Default Modulino LED array indicates day 0 to 8 where:

- Green = Sunny
- Blue = Rainy
- Violet = Foggy
- White = Snow
- Red = Thunder

Uses Modulino Knob to select day and toggle min/max temps.

Additionally, this approach uses Modulino Buttons to switch to hourly forecast (Button A), hourly precipitation & windspeed (Button B), or weekly UV index & windspeed (Button C).
For Buttons B & C the LED array indication changes to:
- Green = Low precipitation probability / UV index
- Yellow = Mid precipitation probability / UV index
- Orange = Mid-High precipitation probability / UV index
- Red = High precipitation probability / UV index
- Violet = Extreme precipitation probability / UV index
  
Instead of relying on the cloud function, the R4 runs a simple webserver for remote control of the desired weather coordinates using the Leaflet maps API.
Remote coordinate updates are indicated by a Modulino Buzzer sound.
<details>
  <summary><b>Screenshot</b></summary>
  
![2025-01-12T22:42:54,855212069+01:00](https://github.com/user-attachments/assets/306127fc-917b-4c1b-8610-edbbcc5697df) 
