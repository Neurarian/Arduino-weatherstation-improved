// Function to manage updates to the internal RTC

#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

RTCTime currentTime;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

unsigned long lastNTPUpdate = 0;

void updateRTCFromNTP() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  RTCTime timeToSet = RTCTime(epochTime);
  RTC.setTime(timeToSet);
  lastNTPUpdate = millis();
  Serial.println("RTC updated from NTP");
}

#endif
