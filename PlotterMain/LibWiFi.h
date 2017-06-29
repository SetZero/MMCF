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
  const char *ssid = "WLAN-****";
  const char *password = "v=wAGmD4c2y9A";
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
