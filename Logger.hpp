#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#define _LOGGER_ENABLED

#include "Arduino.h"

#ifdef _LOGGER_ENABLED

	#define LOG_BUF_SIZE 128 /* Keep it low to avoid stack overflow/corruption! */

	#define LOG_INITIALIZE() \
		Serial.begin(115200)

  #define LOG_DECLARE_BUFFER() \
    char szLogBuffer[LOG_BUF_SIZE]

  #define LOG(sMessage) \
    Serial.println(F(sMessage))

  #define LOG_FMT(sFormat, ...) \
    SerialLogFmt(szLogBuffer, sizeof(szLogBuffer), F(sFormat), ##__VA_ARGS__);

  void SerialLogFmt(
    char*                      lpszBuffer,
    size_t                     nSize,
    const __FlashStringHelper *lpszFormat,
                               ...
  );

#else

	#define LOG_INITIALIZE()
  #define LOG_DECLARE_BUFFER()

	#define LOG(sMessage)
	#define LOG_FMT(sFormat, ...)

#endif

#endif /* LOGGER_HPP_ */
