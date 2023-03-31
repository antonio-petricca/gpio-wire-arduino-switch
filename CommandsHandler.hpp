#ifndef _COMMANDS_HANDLER_HPP_
#define _COMMANDS_HANDLER_HPP_

#include "CommandsParser.hpp"
#include "Configuration.hpp"

using namespace Commands::Parser;

namespace Commands {
  namespace Handler {

    #define HANDLER_LOG_MASK_INFO   1
    #define HANDLER_LOG_MASK_ERRORS 2

    #define HANDLER_LOG_LEVEL \
        HANDLER_LOG_MASK_INFO \
      + HANDLER_LOG_MASK_ERRORS

    typedef
      char RawDeviceID[MAX_DEVICE_ID_LENGTH + 1 /* '\0' */];

    void Initialize();
    bool IsPaired();
    void ProcessCommand(String sCommand);

    void BlinkLED();
  }
}

#endif /* _COMMANDS_HANDLER_HPP_ */
