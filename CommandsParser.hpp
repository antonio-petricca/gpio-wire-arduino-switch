#ifndef _COMMANDS_PARSER_H_
#define _COMMANDS_PARSER_H_

#include "Arduino.h"

namespace Commands {
  namespace Parser {

    #define OT_SWITCH_PROTOCOL_VERSION "1.0"

    #define PARSER_LOG_MASK_INFO       1
    #define PARSER_LOG_MASK_ERRORS     2

    #define PARSER_LOG_LEVEL \
        PARSER_LOG_MASK_INFO \
      + PARSER_LOG_MASK_ERRORS

    enum class Command
    {
      Unknown   = 0,
      SwitchOn  = 1,
      SwitchOff = 2,
      Pair      = 3,
      Unpair    = 4
    };

    bool Parse(String sCommand, String& sDeviceID, Command& eCommand);

  }
}

#endif /* _COMMANDS_PARSER_H_ */
