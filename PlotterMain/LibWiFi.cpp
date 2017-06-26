#include "LibWiFi.h"
#include <Arduino.h>
#include <WiFi.h>

LibWiFi::LibWiFi() {

  if (WiFi.status() != WL_CONNECTED) {
    uint8_t timeout = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && timeout++ < 20) {
      delay(500);
      Serial.print(".");
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

LibWiFi::~LibWiFi() { WiFi.disconnect(true); }

/*
 * Source : https://playground.arduino.cc/Code/NTPclient
 */
unsigned long LibWiFi::getUnixTime() {
  if (WiFi.status() != WL_CONNECTED)
    return 0;
  static WiFiUDP udp;
  static int udpInited = udp.begin(123);     // open socket on arbitrary port
  const char timeServer[] = "pool.ntp.org";  // NTP server
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (!udpInited)
    return 0;
  udp.flush();
  // Send an NTP request
  if (!(udp.beginPacket(timeServer, 123) // 123 is the NTP port
        && udp.write((byte *)&ntpFirstFourBytes, 48) == 48 && udp.endPacket()))
    return 0;
  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150; // poll every this many ms
  const byte maxPoll = 15;  // poll up to this many times
  int pktLen;               // received packet length
  for (byte i = 0; i < maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0; // no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read(); // NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv / 8);

  // Discard the rest of the packet
  udp.flush();
  udp.stop();

  return time - 2208988800ul; // convert NTP time to Unix time
}
