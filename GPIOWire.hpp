#ifndef _GPIOWire_H_
#define _GPIOWire_H_

namespace GPIOWire {

  // http://www.romanblack.com/RF/cheapRFmodules.htm

  #define DECODER_LOG_MASK_CONTROLLER      1
  #define DECODER_LOG_MASK_PULSE           2
  #define DECODER_LOG_MASK_SYNC            4
  #define DECODER_LOG_MASK_BIT             8
  #define DECODER_LOG_MASK_CHAR            16
  #define DECODER_LOG_MASK_BUFFER          32
  #define DECODER_LOG_MASK_NOISE           64
  #define DECODER_LOG_MASK_CRC_CALC        128
  #define DECODER_LOG_MASK_CRC_ERROR       256

  #define DECODER_LOG_LEVEL \
      DECODER_LOG_MASK_CRC_ERROR \
    + DECODER_LOG_MASK_CONTROLLER

  #define DECODER_BUFFER_SIZE              64
  #define DECODER_PULSES_DEBUG_BUFFER_SIZE ((8 + 2) * 3)

  typedef
    char GPIOWireBuffer[DECODER_BUFFER_SIZE];

  enum class MessageResult
  {
    Valid           = 0,
    NotYetAvailable = 1,
    BadCRC          = 2
  };

  void Initialize(
    unsigned int nInterruptPin,
    unsigned int nBitZero,
    unsigned int nBitOne,
    unsigned int nSyncBit,
    bool         bPullUp,
    char         cSTX,
    char         cETX
  );

  void          Start();
  void          Stop();

  bool          HasMessage();
  bool          IsStarted();

  MessageResult GetMessage(GPIOWireBuffer& lpMessage, bool bValidateCRC);
}

#endif /* _GPIOWire_H_ */
