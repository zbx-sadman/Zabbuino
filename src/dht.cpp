/*
Based on: http://playground.arduino.cc/Main/DHTLib
version 0.1.13 is used

*/

#include "dht.h"

/*****************************************************************************************************************************
*
*  Read specified metric's value of the AM/DHT sensor, put it to output buffer on success. 
*
*   Returns: 
*     - RESULT_IN_BUFFER on success
*     - DEVICE_ERROR_CONNECT on connection error
*     - DEVICE_ERROR_ACK_L
*     - DEVICE_ERROR_ACK_H
*     - DEVICE_ERROR_TIMEOUT if sensor stops answer to the request
*
*****************************************************************************************************************************/
int8_t getDHTMetric(const uint8_t _pin, const uint8_t _sensorModel, const uint8_t _metric, char *_dst)
{
  int8_t rc = DEVICE_ERROR_CONNECT;
  // INIT BUFFERVAR TO RECEIVE DATA
  uint8_t mask = 128,
          idx = 0,
          data = 0,
          state = LOW,
          pstate = LOW,
          leadingZeroBits, wakeupDelay, sum,
          bits[5];  // buffer to receive data
  uint16_t zeroLoop = DHTLIB_TIMEOUT,
           delta = 0;
  uint32_t humidity, temperature, result,
           waitTime = 0;

  static uint32_t lastReadTime = 0;
  
  switch (_sensorModel) {
    case DHT11_ID:
      leadingZeroBits = DHTLIB_DHT11_LEADING_ZEROS;
      wakeupDelay = DHTLIB_DHT11_WAKEUP;
      break;
    case DHT21_ID:
    case DHT22_ID:
    case DHT33_ID:
    case DHT44_ID:
    default:
      leadingZeroBits = DHTLIB_DHT_LEADING_ZEROS;
      wakeupDelay = DHTLIB_DHT_WAKEUP;
  }

  leadingZeroBits = 40 - leadingZeroBits; // reverse counting...

  // replace digitalRead() with Direct Port Reads.
  // reduces footprint ~100 bytes => portability issue?
  // direct port read is about 3x faster
  uint8_t bit = digitalPinToBitMask(_pin);
  uint8_t port = digitalPinToPort(_pin);
  volatile uint8_t *PIR = portInputRegister(port);

  pinMode(_pin, OUTPUT);

  // DHT sensor have limit for taking samples frequency - 1kHz (1 sample/sec)
  waitTime = millis() - lastReadTime;
  waitTime = (waitTime < 1100) ? (1100 - waitTime) : 0;

  // Sensor won't to connect if no HIGH level exist on pin for a few time (500ms at least)
  if ((*PIR & bit) == LOW ) {
     digitalWrite(_pin, HIGH);
     if (waitTime < 500) {waitTime = 500;}
  }
  delay(waitTime); 

  // REQUEST SAMPLE
  digitalWrite(_pin, LOW); // T-be
  delayMicroseconds(wakeupDelay * 1000UL);
  digitalWrite(_pin, HIGH); // T-go
  pinMode(_pin, INPUT);

  // data exchange with DHT sensor must not be interrupted
  noInterrupts();
  uint16_t loopCount = DHTLIB_TIMEOUT * 2;  // 200uSec max
  // while(digitalRead(_pin) == HIGH)
  while ((*PIR & bit) != LOW ) {
    if (--loopCount == 0) { goto finish; }  // rc already init with DEVICE_ERROR_CONNECT value
  }

  // GET ACKNOWLEDGE or TIMEOUT
  loopCount = DHTLIB_TIMEOUT;
  // following operation eqial to: while(digitalRead(_pin) == LOW)
  while ((*PIR & bit) == LOW )  // T-rel
    {
      if (--loopCount == 0) { rc = DEVICE_ERROR_ACK_L; goto finish; }
    }

  loopCount = DHTLIB_TIMEOUT;
  // following operation eqial to: while(digitalRead(_pin) == HIGH)
  while ((*PIR & bit) != LOW )  // T-reh
    {
      if (--loopCount == 0) { rc = DEVICE_ERROR_ACK_H; goto finish; }
    }

  loopCount = DHTLIB_TIMEOUT;

  // READ THE OUTPUT - 40 BITS => 5 BYTES
  for (uint8_t i = 40; i != 0; )
    {
      // WAIT FOR FALLING EDGE
      // following operation eqial to:  state = (digitalRead(_pin));
      state = (*PIR & bit);
      if (state == LOW && pstate != LOW)
         {
           // DHT22 return zero for first 6 bits!! DHT11 return only one zero bit
           if (i > leadingZeroBits) 
              {
                zeroLoop = min(zeroLoop, loopCount);
                delta = (DHTLIB_TIMEOUT - zeroLoop)/4;
              }
              else if ( loopCount <= (zeroLoop - delta) ) // long -> one
              {
                 data |= mask;
              }
              mask >>= 1;
              if (mask == 0)   // next byte
                 {
                   mask = 128;
                   bits[idx] = data;
                   idx++;
                  data = 0;
               }
               // next bit
               --i;

               // reset timeout flag
               loopCount = DHTLIB_TIMEOUT;
      }
      pstate = state;
      // Check timeout
      if (--loopCount == 0) { rc = DEVICE_ERROR_TIMEOUT; goto finish; }  // rc already init with DEVICE_ERROR_CONNECT value

  }
  interrupts();
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);

  // Store time to use it to use on the next call of sub
  lastReadTime = millis();
     
  // TEST CHECKSUM
  sum = bits[0] + bits[1] + bits[2] + bits[3];
  if (bits[4] != sum) { rc = DEVICE_ERROR_CHECKSUM; goto finish; }

  switch (_sensorModel) {
    case DHT11_ID:
      // original code part:
      //        these bits are always zero, masking them reduces errors.
      //        bits[0] &= 0x7F;
      //        bits[2] &= 0x7F;
      humidity    = bits[0];  // bits[1] == 0;
      temperature = bits[2];  // bits[3] == 0;
      break;
    case DHT21_ID:
    case DHT22_ID:
    case DHT33_ID:
    case DHT44_ID:
    default:
      // original code part:
      //        these bits are always zero, masking them reduces errors.
      //        bits[0] &= 0x03;
      //        bits[2] &= 0x83;
      //        humidity = (bits[0]*256 + bits[1]) * 0.1;  <== * 0.1 processed by ltoaf()
      humidity = (bits[0]*256 + bits[1]);
      temperature = ((bits[2] & 0x7F)*256 + bits[3]);// * 0.1;
      if (bits[2] & 0x80)  // negative temperature
         {
           temperature = -temperature;
         }
  }
  result = (SENS_READ_HUMD == _metric) ? humidity : temperature;
  ltoaf(result, _dst, 1);
  rc = RESULT_IN_BUFFER;

  finish:
  interrupts();
  return rc;
}


