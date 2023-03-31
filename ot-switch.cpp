#include "ot-switch.hpp"

void setup()
{
  // Initialize environment

  LOG_INITIALIZE();

  // Initialize commands handler

  Commands::Handler::Initialize();

  // Initialize decoder

  GPIOWire::Initialize(
    PIN_INTERRUPT,
    DECODER_BIT_ZERO,
    DECODER_BIT_ONE,
    DECODER_BIT_SYNC,
    DECODER_PULLUP,
    DECODER_STX,
    DECODER_ETX
  );

  GPIOWire::Start();
}

void loop()
{
  GPIOWire::GPIOWireBuffer lpMessage;

  Commands::Handler::BlinkLED();

  switch (GPIOWire::GetMessage(lpMessage, DECODER_VALIDATE_CRC))
  {
    case GPIOWire::MessageResult::Valid :
      Commands::Handler::ProcessCommand(lpMessage);
      GPIOWire::Start();

      break;

    case GPIOWire::MessageResult::BadCRC :
      GPIOWire::Start();
      break;

    case GPIOWire::MessageResult::NotYetAvailable :
      // Do nothing so far...
      break;
  }
}
