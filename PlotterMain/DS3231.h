/*
 * DS3231.h
 *
 * Created: 2017-05-21 22:02:11
 *  Author: Sebastian
 */

#ifndef DS3231_H_
#define DS3231_H_
#include <ctime>
#include <stdint.h>

class DS3231 {
private:
  const int DS3231ADDR = 0x68;

public:
  DS3231();
  std::tm getTime();
  unsigned long getTimestamp();
  void setTime(std::tm &t);
  uint8_t dec2bcd(uint8_t d);
  uint8_t bcd2dec(uint8_t b);
  void synchonizeTime();
};

#endif /* DS3231_H_ */
