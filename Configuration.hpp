#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#define MAX_DEVICE_ID_LENGTH 6 // HHMMSS

#define PIN_INTERRUPT        2
#define PIN_LED              LED_BUILTIN
#define PIN_BUTTON           3
#define PIN_RELAY            4

#define DECODER_BIT_ZERO     1000
#define DECODER_BIT_ONE      1500
#define DECODER_BIT_SYNC     2500

#define DECODER_PULLUP       false
#define DECODER_STX          '\x02'
#define DECODER_ETX          '\x03'
#define DECODER_VALIDATE_CRC true

#define LED_BLINK_TIMEOUT    500 * 1000 /* Microseconds */
#define LED_FLASH_TIMEOUT    125        /* Milliseconds */
#define LED_FLASH_COUNT      3

/*
 * Configure logging on:
 *
 *  - CommandsHandler.hpp
 *  - CommandsParser.hpp
 *  - GPIOWire.hpp
 */

#endif /* _CONFIGURATION_HPP_ */
