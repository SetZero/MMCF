/*
 * FunctionLibary.cpp
 *
 * Created: 2017-05-13 21:09:58
 *  Author: Sebastian
 */
#include "Arduino.h"
#include "FunctionLibary.h"

void FunctionLibary::my_delay_1ms(uint16_t n) {
  while (n--)
    delay(1);
}

void FunctionLibary::my_delay_10us(uint16_t n) {
  while (n--)
    delayMicroseconds(10);
}

void FunctionLibary::pause(long ms) {
  delay(ms / 1000);
  delayMicroseconds((ms) % 1000);
}
