#include "Logger.hpp"

// https://playground.arduino.cc/Main/CorruptArrayVariablesAndMemory
// https://www.arduino.cc/en/Reference/PROGMEM
// https://playground.arduino.cc/Main/Printf
// avr-objdump -s -j .data switch.elf | less

#ifdef _LOGGER_ENABLED

void SerialLogFmt(
  char*                      lpszBuffer,
  size_t                     nSize,
  const __FlashStringHelper *lpszFormat,
  ...
)
{
  va_list  lpArgs;

  va_start (lpArgs, lpszFormat);

#ifdef __AVR__
  // PROGMEM for AVR

  vsnprintf_P(
    lpszBuffer,
    nSize,
    (const char *)lpszFormat,
    lpArgs
  );
#else
  // For the rest of the world

  vsnprintf(
    buf,
    nSize,
    (const char *)fmt,
    args
  );
#endif

  va_end(lpArgs);

  Serial.println(lpszBuffer);
}

#endif
