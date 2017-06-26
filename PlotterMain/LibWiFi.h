#ifndef LIBWIFI_H_
#define LIBWIFI_H_
#include <WiFi.h>
#include <iostream>

/*
 * Singleton Pattern, we only need one WiFi connection, we don't want multiple
 * at the same time
 */
/*
 * TODO:
 * - Add WiFi as char buffer input
 * - Add WiFi notifications about printer status (HTTP-Deamon?)
 */
class LibWiFi {
private:
  const char *ssid = "WLAN-C32D56";
  const char *password = "8554418519542634";
  LibWiFi();

public:
  unsigned long getUnixTime();
  static LibWiFi &instance() {
    static LibWiFi _instance;
    return _instance;
  }
  LibWiFi(LibWiFi const &) = delete;
  void operator=(LibWiFi const &) = delete;
  ~LibWiFi();
};

#endif
