/*
 * FunctionLibary.h
 *
 * Created: 2017-05-13 21:08:49
 *  Author: Sebastian
 */

#ifndef FUNCTIONLIBARY_H_
#define FUNCTIONLIBARY_H_

#include <stdint.h>

class FunctionLibary {
public:
  static void my_delay_1ms(uint16_t n);
  static void my_delay_10us(uint16_t n);
  static void pause(long ms);
};

#endif /* FUNCTIONLIBARY_H_ */
