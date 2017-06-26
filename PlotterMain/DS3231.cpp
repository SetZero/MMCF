/*
 * CPPFile1.cpp
 *
 * Created: 2017-05-21 22:01:17
 *  Author: Sebastian
 */

#include "DS3231.h"
#include "LibWiFi.h"
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

DS3231::DS3231() { Serial.print("Starting I2C\n\r"); }

std::tm DS3231::getTime() {
  std::tm when;
  uint8_t rtc[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t century = 0;

  Wire.beginTransmission(DS3231ADDR);
  Wire.write(0x0);
  Wire.endTransmission();

  Wire.requestFrom(DS3231ADDR, 7);

  for (uint8_t i = 0; i < 7; i++) {
    rtc[i] = Wire.read();
  }

  Wire.endTransmission();

  when.tm_sec = bcd2dec(rtc[0]);
  when.tm_min = bcd2dec(rtc[1]);
  when.tm_hour = bcd2dec(rtc[2]);
  when.tm_mday = bcd2dec(rtc[4]);
  when.tm_mon = bcd2dec(rtc[5] & 0x1F);
  century = (rtc[5] & 0x80) >> 7;
  when.tm_year = century == 1 ? 2000 + bcd2dec(rtc[6])
                              : 1900 + bcd2dec(rtc[6]); // year 0-99
  when.tm_wday = bcd2dec(rtc[3]);                       // returns 1-7
  return when;
}

unsigned long DS3231::getTimestamp() {
  std::tm t = getTime();
  return (unsigned long)mktime(&t);
}

void DS3231::setTime(std::tm &when) {
  Wire.beginTransmission(DS3231ADDR);
  Wire.write(0x0); // start address
  uint8_t century;

  if (when.tm_year > 100) {
    century = 0x80;
    when.tm_year = when.tm_year - 100;
  } else {
    century = 0;
    when.tm_year = when.tm_year;
  }

  Wire.write(dec2bcd(when.tm_sec));
  Wire.write(dec2bcd(when.tm_min));
  Wire.write(dec2bcd(when.tm_hour));
  Wire.write(dec2bcd(when.tm_wday));
  Wire.write(dec2bcd(when.tm_mday));
  Wire.write(dec2bcd(when.tm_mon + 1) + century);
  Wire.write(dec2bcd(when.tm_year));

  Wire.endTransmission();
}

uint8_t DS3231::dec2bcd(uint8_t d) { return ((d / 10 * 16) + (d % 10)); }

uint8_t DS3231::bcd2dec(uint8_t b) { return ((b / 16 * 10) + (b % 16)); }

void DS3231::synchonizeTime() {
  struct tm *timeinfo;
  time_t currTime = (time_t)LibWiFi::instance().getUnixTime();
  // If the time is 0 (1.1.1970) something went wrong
  if ((unsigned long)currTime == 0)
    return;
  if (currTime != 0) {
    timeinfo = std::localtime(&currTime);
    // Evil Timezone Hack
    if (timeinfo->tm_mon > 2 && timeinfo->tm_mon < 9) {
      timeinfo->tm_hour += 2;
    } else {
      timeinfo->tm_hour += 1;
    }
    setTime(*timeinfo);
  }
}
