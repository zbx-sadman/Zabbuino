// Config & common included files
#include "sys_includes.h"

#include <wiring_private.h>

#if defined(ARDUINO_ARCH_AVR)
  #include <util/atomic.h>
#endif

#include "service.h"
#include "system.h"

#include "interrupts.h"

// #if defined(ARDUINO_ARCH_AVR)


// EXTERNAL_NUM_INTERRUPTS its a macro from <wiring_private.h>
volatile static extInterrupt_t extInterrupt[EXTERNAL_NUM_INTERRUPTS];


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                                                      EXTERNAL INTERRUPTS HANDLING SECTION

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void initExtInt() {
  // Init external interrupts info structure
  uint8_t extIntNo = EXTERNAL_NUM_INTERRUPTS;
  while (extIntNo) {
    extIntNo--;
    // -1 - interrupt is detached
    // just use NOT_AN_INTERRUPT = -1 macro from Arduino.h
    extInterrupt[extIntNo].mode  = NOT_AN_INTERRUPT;
    extInterrupt[extIntNo].owner = OWNER_IS_NOBODY;
    extInterrupt[extIntNo].value = 0x00;
  }
}

/*****************************************************************************************************************************
*
*  Create interrupts handling subs from the Macro (see interrupts.h)
*
*****************************************************************************************************************************/

 // EXTERNAL_NUM_INTERRUPTS its a macro from <wiring_private.h>
#ifdef FEATURE_EXTERNAL_INTERRUPT_ENABLE

#if (EXTERNAL_NUM_INTERRUPTS > 0x0F)
   HANDLE_INT_N_FOR_EXTINT(INT15)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0E)
   HANDLE_INT_N_FOR_EXTINT(INT14)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0D)
   HANDLE_INT_N_FOR_EXTINT(INT13)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0C)
   HANDLE_INT_N_FOR_EXTINT(INT12)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0B)
   HANDLE_INT_N_FOR_EXTINT(INT11)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0A)
   HANDLE_INT_N_FOR_EXTINT(INT10)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x09)
   HANDLE_INT_N_FOR_EXTINT(INT9)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x08)
   HANDLE_INT_N_FOR_EXTINT(INT8)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
   HANDLE_INT_N_FOR_EXTINT(INT7)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
   HANDLE_INT_N_FOR_EXTINT(INT6)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
   HANDLE_INT_N_FOR_EXTINT(INT5)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
   HANDLE_INT_N_FOR_EXTINT(INT4)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
   HANDLE_INT_N_FOR_EXTINT(INT3)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
   HANDLE_INT_N_FOR_EXTINT(INT2)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
   HANDLE_INT_N_FOR_EXTINT(INT1)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
   HANDLE_INT_N_FOR_EXTINT(INT0)
#endif


/*****************************************************************************************************************************
*
*  Attach/detach interrupts, return counter value
*
*  Returns: 
*    - value of counter belonging to interrupt
*    - RESULT_IS_FAIL if interrupt mode is wrong of wrong pin is specified
*
*****************************************************************************************************************************/
// This function make more that just return counter...
int8_t manageExtInt(const uint8_t _pin, const uint8_t _mode, uint32_t* _value) {
   int8_t rc = RESULT_IS_FAIL;
   // This condition placed here to avoid using defaut case (EXTERNAL_NUM_INTERRUPTS <= 0) in switch(interruptNumber) due its very strange but 
   // theoretically possible situation 
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)

   voidFuncPtr interruptHandler;
   int8_t interruptNumber = digitalPinToInterrupt(_pin);

   // ESP8266 & ESP32 Core have different number of modes && AVR RISING value != ESP RISING value
#if defined(ARDUINO_ARCH_AVR)
  const uint8_t maxInterruptMode = RISING;
#elif (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))
  const uint8_t maxInterruptMode = ONHIGH;
#endif
   Serial.println("- 1 -");
   // NOT_AN_INTERRUPT == -1 - it's macro from Arduino.h
   // Interrupt number and mode is correct? If not - just jump to the end, because rc already init with RESULT_IS_FAIL value
   if ((NOT_AN_INTERRUPT == interruptNumber) || (EXTERNAL_NUM_INTERRUPTS < interruptNumber) || (maxInterruptMode < _mode)) { goto finish; }

   Serial.println("- 2 -");
   Serial.print("extInterrupt[interruptNumber].mode: "); Serial.println(extInterrupt[interruptNumber].mode);
   Serial.print("_mode: "); Serial.println(_mode);

   // Just return counter value if interrupt mode is not changed, but interrupt is attached
   // Atomic block is used to get relable counter value due AVR8 make read long variables with series of ASM commands 
   // and value of the variable can be changed before reading is finished
   if ((extInterrupt[interruptNumber].mode != _mode)) {

   Serial.println("- 3 -");
      // Detach is need to avoid get strange results in extInterrupt[interruptNumber].value if external signals still incoming to pin
      // .owner field need to detect owner of counter by manageIncEnc() sub due it don't operate .mode field 
      detachInterrupt(interruptNumber);
      extInterrupt[interruptNumber].owner = OWNER_IS_EXTINT;
      extInterrupt[interruptNumber].mode  = _mode;

      // Pin must be configured as input or system will hang up. 
      // INPUT_PULLUP pin mode allows you to avoid unwanted interference (and continuous calls of the interrupt handling sub) when 
      // pin lost connection to the external pull-up resistor
      pinMode(_pin, INPUT_PULLUP);
      // No ATOMIC_BLOCK(ATOMIC_RESTORESTATE{} used here due interrupt must be previosly detached
      // ...but if new mode is RISING and pin have HIGH state when attachInterrupt() will be called - counter will be increased immediately.
      // May be better init counter after attachInterrupt() to get 0 on any state of _pin?
      extInterrupt[interruptNumber].value = 0x00;

      switch (interruptNumber) {
// This code taken from WInterrupts.c and modifed
#if (EXTERNAL_NUM_INTERRUPTS > 0x10)
    #warning There are more than 16 external interrupts. Some callbacks may not be initialized.
#endif

#if (EXTERNAL_NUM_INTERRUPTS > 0x0F)
        CASE_INT_N_FOR_EXTINT(INT15)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0E)
        CASE_INT_N_FOR_EXTINT(INT14)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0D)
        CASE_INT_N_FOR_EXTINT(INT13)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0C)
        CASE_INT_N_FOR_EXTINT(INT12)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0B)
        CASE_INT_N_FOR_EXTINT(INT11)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0A)
        CASE_INT_N_FOR_EXTINT(INT10)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x09)
        CASE_INT_N_FOR_EXTINT(INT9)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x08)
        CASE_INT_N_FOR_EXTINT(INT8)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
        CASE_INT_N_FOR_EXTINT(INT7)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
        CASE_INT_N_FOR_EXTINT(INT6)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
        CASE_INT_N_FOR_EXTINT(INT5)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
        CASE_INT_N_FOR_EXTINT(INT4)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
        CASE_INT_N_FOR_EXTINT(INT3)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
        CASE_INT_N_FOR_EXTINT(INT2)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
        CASE_INT_N_FOR_EXTINT(INT1)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
        CASE_INT_N_FOR_EXTINT(INT0)
#endif
      }  // switch (interruptNumber)

      Serial.println("- 4 -");

      // Need to do checking NOT_AN_INTERRUPT == _mode and not attach if true?
      attachInterrupt(interruptNumber, interruptHandler, _mode);
   }

#if defined(ARDUINO_ARCH_AVR)
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { *_value = (uint32_t) extInterrupt[interruptNumber].value; }
#elif (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))
   ATOMIC() { *_value = (uint32_t) extInterrupt[interruptNumber].value; }
#endif

   rc = RESULT_IS_UNSIGNED_VALUE;
          
#endif // #if (EXTERNAL_NUM_INTERRUPTS > 0

   finish:
   return rc;

   
}

#endif // FEATURE_EXTERNAL_INTERRUPT_ENABLE


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                                                      ENCODER INTERRUPTS HANDLING SECTION

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifdef FEATURE_INCREMENTAL_ENCODER_ENABLE

#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
   HANDLE_INT_N_FOR_INCENC(INT7)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
   HANDLE_INT_N_FOR_INCENC(INT6)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
   HANDLE_INT_N_FOR_INCENC(INT5)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
   HANDLE_INT_N_FOR_INCENC(INT4)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
   HANDLE_INT_N_FOR_INCENC(INT3)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
   HANDLE_INT_N_FOR_INCENC(INT2)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
   HANDLE_INT_N_FOR_INCENC(INT1)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
   HANDLE_INT_N_FOR_INCENC(INT0)
#endif


/*****************************************************************************************************************************
*
*  Attach/detach interrupts, return encoder's variable value
*
*  Returns: 
*    - value of variable belonging to interrupt applies to the pin to which encoder's "Terminal A" connected
*    - RESULT_IS_FAIL if wrong pin is specified
*
*****************************************************************************************************************************/
int8_t manageIncEnc(int32_t* _dst, uint8_t const _terminalAPin, uint8_t const _terminalBPin, int32_t const _initialValue) {
   int8_t rc = RESULT_IS_FAIL;

   // This condition placed here to avoid using defaut case (EXTERNAL_NUM_INTERRUPTS <= 0) in switch(interruptNumber) due its very strange but 
   // theoretically possible situation 
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
   //extern volatile extInterrupt_t extInterrupt[];
//   extern extInterrupt_t *extInterrupt;
   voidFuncPtr interruptHandler;
   int8_t interruptNumber = digitalPinToInterrupt(_terminalAPin);
   // NOT_AN_INTERRUPT == -1 - it's macro from Arduino.h
   // Interrupt number and mode is correct? If not - just jump to the end, because rc already init with RESULT_IS_FAIL value
   if ((NOT_AN_INTERRUPT == interruptNumber) || (EXTERNAL_NUM_INTERRUPTS < interruptNumber)) { goto finish; }

   // Just return value of .value field if encoder's handle sub already linked to this interrupt
   // Otherwise - reattach interrupt and init .value field
   if (OWNER_IS_INCENC != extInterrupt[interruptNumber].owner) {
   
      // Detach is need to avoid get strange results in extInterrupt[interruptNumber].value if external signals still incoming to pin
      detachInterrupt(interruptNumber);
      extInterrupt[interruptNumber].owner = OWNER_IS_INCENC;
      extInterrupt[interruptNumber].mode = CHANGE;
 
      // INPUT_PULLUP pin mode allows you to avoid unwanted interference (and continuous calls of the interrupt handling sub) when 
      // pins lost connection to the external pull-up resistor
      pinMode(_terminalAPin, INPUT_PULLUP);
      pinMode(_terminalBPin, INPUT_PULLUP);
      extInterrupt[interruptNumber].encTerminalAPinBit = digitalPinToBitMask(_terminalAPin);
      extInterrupt[interruptNumber].encTerminalBPinBit = digitalPinToBitMask(_terminalBPin);
      extInterrupt[interruptNumber].encTerminalAPIR = portInputRegister(digitalPinToPort(_terminalAPin));
      extInterrupt[interruptNumber].encTerminalBPIR = portInputRegister(digitalPinToPort(_terminalBPin));
      extInterrupt[interruptNumber].value = _initialValue;

      switch (interruptNumber) {
#if (EXTERNAL_NUM_INTERRUPTS > 0x08)
    #warning There are more than 8 external interrupts. Some callbacks may not be initialized.
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
        CASE_INT_N_FOR_INCENC(INT7)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
        CASE_INT_N_FOR_INCENC(INT6)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
        CASE_INT_N_FOR_INCENC(INT5)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
        CASE_INT_N_FOR_INCENC(INT4)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
        CASE_INT_N_FOR_INCENC(INT3)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
        CASE_INT_N_FOR_INCENC(INT2)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
        CASE_INT_N_FOR_INCENC(INT1)
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
        CASE_INT_N_FOR_INCENC(INT0)
#endif
        }  // switch (interruptNumber)
        attachInterrupt(interruptNumber, interruptHandler, CHANGE);
   } // (OWNER_IS_INCENC != extInterrupt[interruptNumber].owner)

#if defined(ARDUINO_ARCH_AVR)
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { *_dst = (int32_t) extInterrupt[interruptNumber].value; }
#elif (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))
   ATOMIC() { *_dst = (int32_t) extInterrupt[interruptNumber].value; }
#endif

   rc = RESULT_IS_SIGNED_VALUE;
#endif

   finish:
   return rc;

}
#endif // FEATURE_INCREMENTAL_ENCODER_ENABLE

// #endif // #if defined(ARDUINO_ARCH_AVR)