// Webserver functions to handle HTML response and remote location change requests

#ifndef WEBSERVER_H
#define WEBSERVER_H

float latitude = 0;
float longitude = 0;

void sendHtmlResponse(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println(R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Weather Location Selector</title>
  <link rel='stylesheet' href='https://unpkg.com/leaflet@1.7.1/dist/leaflet.css'/>
  <script src='https://unpkg.com/leaflet@1.7.1/dist/leaflet.js'></script>
  <style>
    :root {
      --bg-color: #ffffff;
      --text-color: #333333;
      --button-bg: #135a91;
      --button-text: white;
    }
    .dark-mode {
      --bg-color: #333333;
      --text-color: #ffffff;
      --button-bg: #2196F3;
      --button-text: black;
    }
  body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 20px;
    padding-top: 50px;
    display: flex;
    flex-direction: column;
    align-items: center;
    background-color: var(--bg-color);
    color: var(--text-color);
  }

  h1 {
    color: var(--text-color);
    text-align: center;
  }

  p {
    max-width: 600px;
    text-align: center;
    margin-bottom: 20px;
  }

#map {
    height: 400px;
    width: 100%;
    max-width: 800px;
    margin-bottom: 20px;
  }

  .input-group {
    margin-bottom: 10px;
    text-align: left;
    width: 100%;
    max-width: 300px;
  }

  label {
    display: block;
    margin-bottom: 5px;
    font-weight: bold;
  }

  input[type='text'] {
    width: 100%;
    padding: 8px;
    box-sizing: border-box;
    background-color: var(--bg-color);
    color: var(--text-color);
    border: 1px solid var(--text-color);
  }

  button {
    padding: 10px 20px;
    background-color: var(--button-bg);
    color: var(--button-text);
    border: none;
    cursor: pointer;
    margin-top: 10px;
  }

  .switch {
    position: relative;
    display: inline-block;
    width: 50px;
    height: 28px;
  }

  .switch input {
    opacity: 0;
    width: 0;
    height: 0;
  }

  .slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #ccc;
    transition: .4s;
  }

  .slider:before {
    position: absolute;
    content: "";
    height: 20px;
    width: 20px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    transition: .4s;
  }

  input:checked + .slider {
    background-color: #2196F3;
  }

  input:checked + .slider:before {
    transform: translateX(26px);
  }

  .slider.round {
    border-radius: 34px;
  }

  .slider.round:before {
    border-radius: 50%;
  }

  .header {
    width: 100%;
    max-width: 800px;
    margin-bottom: 20px;
    position: relative;
  }

  h1 {
    text-align: center;
    margin: 0;
  }

  .dark-mode-container {
    position: absolute;
    top: 10px;
    right: 10px;
    display: flex;
    align-items: center;
    z-index: 1001;
  }

  .dark-mode-label {
    margin-right: 8px;
    font-size: 0.9em;
  }

  .header {
    text-align: center;
    width: 100%;
    max-width: 800px;
    margin: 20px auto;
  }

  h1 {
    margin: 0;
  }

  @media (max-width: 768px) {
    body {
      padding-top: 60px;
    }

    .header {
      margin-top: 10px;
    }

    h1 {
      font-size: 1.5em;
    }

    #map {
      height: 300px;
    }
  }
</style>

<body>
  
  <div class="dark-mode-container">
    <span class="dark-mode-label">Dark Mode</span>
    <label class="switch">
      <input type="checkbox" id="darkModeToggle">
      <span class="slider round"></span>
    </label>
  </div>

  <div class="header">
    <h1>Weather Location Selector</h1>
  </div>
  
  <p>Click on the map to select a location or geolocate by IP. These coordinates will be used to fetch weather data for the next week and display them on the Arduino board.</p>
  <div id='map'></div>
  <div class='input-group'>
    <label for='latitude'>Latitude:</label>
    <input type='text' id='latitude' readonly>
  </div>
  <div class='input-group'>
    <label for='longitude'>Longitude:</label>
    <input type='text' id='longitude' readonly>
  </div>
  <button id="ipButton">Use IP Location</button>
  <button onclick='submitCoordinates()'>Submit</button>
  <script>
    var map = L.map('map').setView([0, 0], 2);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: '© OpenStreetMap contributors'
    }).addTo(map);
    var marker;

    function setLocation(lat, lng) {
      map.setView([lat, lng], 13);
      if (marker) { map.removeLayer(marker); }
      marker = L.marker([lat, lng]).addTo(map);
      document.getElementById('latitude').value = lat.toFixed(6);
      document.getElementById('longitude').value = lng.toFixed(6);
    }

    function IPGeolocation() {
      fetch('https://ipapi.co/json/')
        .then(response => response.json())
        .then(data => {
          setLocation(data.latitude, data.longitude);
        })
        .catch(error => {
          console.error("Error getting IP location:", error);
        });
    }

    document.getElementById("ipButton").addEventListener("click", IPGeolocation);

    map.on('click', function(e) {
      setLocation(e.latlng.lat, e.latlng.lng);
    });

    function submitCoordinates() {
      var lat = document.getElementById('latitude').value;
      var lng = document.getElementById('longitude').value;
      if (lat && lng) {
        fetch(`/update?lat=${lat}&lng=${lng}`)
          .then(response => response.text())
          .then(data => alert('Coordinates updated: ' + lat + ', ' + lng));
      } else {
        alert('Please select a location on the map');
      }
    }

  // Dark mode toggle
  const darkModeToggle = document.getElementById('darkModeToggle');
  
  // Function to set dark mode
  function setDarkMode(isDark) {
    document.body.classList.toggle('dark-mode', isDark);
    darkModeToggle.checked = isDark;
    localStorage.setItem('darkMode', isDark);
    map.invalidateSize(); // Refresh map size after theme change
  }

  // Check for saved dark mode preference
  const savedDarkMode = localStorage.getItem('darkMode');
  if (savedDarkMode !== null) {
    setDarkMode(savedDarkMode === 'true');
  }

  // Dark mode toggle event listener
  darkModeToggle.addEventListener('change', function() {
    setDarkMode(this.checked);
  });
  </script>
</body>
</html>
)rawliteral");
}

void handleCoordinateUpdate(WiFiClient &client, const String &request) {
  int latStart = request.indexOf("lat=") + 4;
  int latEnd = request.indexOf("&lng=");
  int lngStart = request.indexOf("lng=") + 4;
  int lngEnd = request.indexOf(" HTTP");

  String latStr = request.substring(latStart, latEnd);
  String lngStr = request.substring(lngStart, lngEnd);

  latitude = latStr.toFloat();
  longitude = lngStr.toFloat();

  Serial.print("Updated Latitude: ");
  Serial.println(latitude);
  Serial.print("Updated Longitude: ");
  Serial.println(longitude);

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/plain");
  client.println();
  client.println("Coordinates updated");
}

#endif
