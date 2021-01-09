#pragma once

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  #define INT0   0
  #define INT1   1
  #define INT2   2
  #define INT3   3
  #define INT4   4
  #define INT5   5
  #define INT6   6
  #define INT7   7
  #define INT8   8
  #define INT9   9
  #define INT10 10
  #define INT11 11
  #define INT12 12
  #define INT13 13
  #define INT14 14
  #define INT15 15
#endif



// #if defined(ARDUINO_ARCH_AVR)

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                                                      EXTERNAL INTERRUPTS HANDLING SECTION

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifdef FEATURE_EXTERNAL_INTERRUPT_ENABLE

void initExtInt(void);

/*****************************************************************************************************************************
*
*  Attach/detach interrupts, return counter value
*
*  Returns: 
*    - value of counter belonging to interrupt
*    - RESULT_IS_FAIL if interrupt mode is wrong of wrong pin is specified
*
*****************************************************************************************************************************/
int8_t manageExtInt(const uint8_t, const uint8_t, uint32_t*);

/*
 macro CASE_INT_N_FOR_EXTINT(INT0) will be transformed to code:

   case INT0:
     interruptHandler = handleExtINT0;
     break;

*/

#define CASE_INT_N_FOR_EXTINT(_interrupt) \
    case _interrupt: \
       interruptHandler = handleExt##_interrupt;\
    break;





/*

 macro HANDLE_INT_N_FOR_EXTINT(INT0) will be  transformed to code:

    void handleExtINT0() { extern extInterrupt_t* extInterrupt; extInterrupt[INT0].value++; }
*/

#if defined(ARDUINO_ARCH_AVR)  

#define HANDLE_INT_N_FOR_EXTINT(_interrupt) \
   void handleExt##_interrupt(void) { extInterrupt[_interrupt].value++; }

#elif defined(ARDUINO_ARCH_ESP8266) 

#define HANDLE_INT_N_FOR_EXTINT(_interrupt) \
   ICACHE_RAM_ATTR void handleExt##_interrupt(void) { extInterrupt[_interrupt].value++; }

#elif defined(ARDUINO_ARCH_ESP32)

#define HANDLE_INT_N_FOR_EXTINT(_interrupt) \
   IRAM_ATTR void handleExt##_interrupt(void) { extInterrupt[_interrupt].value++; }

#endif



 
#if (EXTERNAL_NUM_INTERRUPTS > 0x0F)
  void handleExtINT15(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0E)
  void handleExtINT14(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0D)
  void handleExtINT13(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0C)
  void handleExtINT12(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0B)
  void handleExtINT11(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x0A)
  void handleExtINT10(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x09)
  void handleExtINT9(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x08)
  void handleExtINT8(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
  void handleExtINT7(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
  void handleExtINT6(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
  void handleExtINT5(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
  void handleExtINT4(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
  void handleExtINT3(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
  void handleExtINT2(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
  void handleExtINT1(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
  void handleExtINT0(void);
#endif


#endif // FEATURE_EXTERNAL_INTERRUPT_ENABLE


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                                                      ENCODER INTERRUPTS HANDLING SECTION

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifdef FEATURE_INCREMENTAL_ENCODER_ENABLE
/*****************************************************************************************************************************
*
*  Attach/detach interrupts, return encoder's variable value
*
*  Returns: 
*    - value of variable belonging to interrupt applies to the pin to which encoder's "Terminal A" connected
*    - RESULT_IS_FAIL if wrong pin is specified
*
*****************************************************************************************************************************/
int8_t manageIncEnc(int32_t* _dst, uint8_t const _terminalAPin, uint8_t const _terminalBPin, int32_t const _initialValue);

/*
 macro CASE_INT_N_FOR_INCENC(INT0) will be transformed to code:

   case INT0:
     interruptHandler = handleExtINT0;
     break;

*/
#define CASE_INT_N_FOR_INCENC(_interrupt) \
    case _interrupt: \
       interruptHandler = handleIncEnc##_interrupt;\
    break;

                                            
// need use overflow test for "extInterrupt[_interrupt].value++" and "extInterrupt[_interrupt].value--" to avoid sign bit flapping  
// { ((int32_t) extInterrupt[_interrupt].value)++; } else { ((int32_t) extInterrupt[_interrupt].value)--; }
#define HANDLE_INT_N_FOR_INCENC(_interrupt) \
   void handleIncEnc##_interrupt(void) { volatile static uint8_t stateTerminalA = 0, statePrevTerminalA = 0;\
         delayMicroseconds(constEncoderStabilizationDelay);\
         stateTerminalA = *extInterrupt[_interrupt].encTerminalAPIR & extInterrupt[_interrupt].encTerminalAPinBit;\
         if ((!stateTerminalA) && (statePrevTerminalA)) { \
           if (*extInterrupt[_interrupt].encTerminalBPIR & extInterrupt[_interrupt].encTerminalBPinBit) \
              { ++extInterrupt[_interrupt].value; } else { --extInterrupt[_interrupt].value; } \
         } statePrevTerminalA = stateTerminalA; }

#if (EXTERNAL_NUM_INTERRUPTS > 0x07)
  void handleIncEncINT7(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x06)
  void handleIncEncINT6(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x05)
  void handleIncEncINT5(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x04)
  void handleIncEncINT4(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x03)
  void handleIncEncINT3(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x02)
  void handleIncEncINT2(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x01)
  void handleIncEncINT1(void);
#endif
#if (EXTERNAL_NUM_INTERRUPTS > 0x00)
  void handleIncEncINT0(void);
#endif

#endif // FEATURE_INCREMENTAL_ENCODER_ENABLE

// #endif // #if defined(ARDUINO_ARCH_AVR)