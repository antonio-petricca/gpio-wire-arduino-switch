#include <string.h>

#include "CommandsParser.hpp"
#include "Configuration.hpp"
#include "Logger.hpp"

namespace Commands {
  namespace Parser {

    #define TOKEN(lpCommand) \
      (lpToken = strtok_r(lpCommand, "|", &lpSave))

    const String OT_SWITCH_CMD_PAIR    = "P";
    const String OT_SWITCH_CMD_SWITCH  = "S";

    const String OT_SWITCH_CMD_PARAM_0 = "0";
    const String OT_SWITCH_CMD_PARAM_1 = "1";

    LOG_DECLARE_BUFFER();

    bool CheckDeviceID(String sSourceDeviceID, String sTargetDeviceID)
    {
      if (sSourceDeviceID != sTargetDeviceID)
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS

          LOG_FMT(
            "Parser: ID not matching (S: \"%s\", T: \"%s\").",
            sSourceDeviceID.c_str(),
            sTargetDeviceID.c_str()
          );

        #endif

        return false;
      }

      return true;
    }

    bool ProcessPairCommand(
      String   sSourceDeviceID,
      String&  sTargetDeviceID,
      String   sParameter,
      Command& eCommand
    )
    {
      if (OT_SWITCH_CMD_PARAM_1 == sParameter)
      {
        sTargetDeviceID = sSourceDeviceID;
        eCommand        = Command::Pair;
      }
      else if (OT_SWITCH_CMD_PARAM_0 == sParameter)
      {
        sTargetDeviceID = "";
        eCommand        = Command::Unpair;
      }
      else
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG_FMT(
            "Parser: unsupported parameter \"%s\".",
            sParameter.c_str()
          );
        #endif

        return false;
      }

      return true;
    }

    bool ProcessSwitchCommand(
      String   sSourceDeviceID,
      String  sTargetDeviceID,
      String   sParameter,
      Command& eCommand
    )
    {
      if (OT_SWITCH_CMD_PARAM_1 == sParameter)
      {
        if (!CheckDeviceID(sSourceDeviceID, sTargetDeviceID))
        {
          return false;
        }

        eCommand = Command::SwitchOn;
      }
      else if (OT_SWITCH_CMD_PARAM_0 == sParameter)
      {
        if (!CheckDeviceID(sSourceDeviceID, sTargetDeviceID))
        {
          return false;
        }

        eCommand = Command::SwitchOff;
      }
      else
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG_FMT(
            "Parser: unsupported parameter \"%s\".",
            sParameter.c_str()
          );
        #endif

        return false;
      }

      return true;
    }

    bool Parse(String sCommand, String& sDeviceID, Command& eCommand)
    {
      #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_INFO
        LOG_FMT(
          "Parser: parsing \"%s\", ID \"%s\"...",
          sCommand.c_str(),
          sDeviceID.c_str()
        );
      #endif

      eCommand        = Command::Unknown;

      char* lpCommand = (char*)sCommand.c_str();
      char* lpSave    = nullptr;
      char* lpToken   = nullptr;

      // OT

      if (!TOKEN(lpCommand))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: missing header.");
        #endif

        return false;
      }

      if (0 != strcmp(lpToken, "OT"))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG_FMT(
            "Parser: invalid header \"%s\".",
            lpToken
          );
        #endif

        return false;
      }

      // Protocol version

      if (!TOKEN(nullptr))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: missing protocol version.");
        #endif

        return false;
      }

      if (0 != strcmp(lpToken, OT_SWITCH_PROTOCOL_VERSION))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: protocol version not supported.");
        #endif

        return false;
      }

      // Device ID

      if (!TOKEN(nullptr))
      {
          #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
            LOG("Parser: missing ID.");
          #endif

          return false;
      }

      int nLen = strlen(lpToken);

      if (0 == nLen)
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: empty ID.");
        #endif

        return false;
      }

      if (nLen > MAX_DEVICE_ID_LENGTH)
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG_FMT("Parser: ID too long (%d).", nLen);
        #endif

        return false;
      }

      String sSourceDeviceID(lpToken);

      // Command ID

      if (!TOKEN(nullptr))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: missing command ID.");
        #endif

        return false;
      }

      String sCommandID(lpToken);

      // Command parameter

      if (!TOKEN(nullptr))
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG("Parser: missing command parameter.");
        #endif

        return false;
      }

      String sParameter(lpToken);

      // Process command

      if (OT_SWITCH_CMD_PAIR == sCommandID)
      {
        return ProcessPairCommand(
          sSourceDeviceID,
          sDeviceID,
          sParameter,
          eCommand
        );
      }
      else if (OT_SWITCH_CMD_SWITCH == sCommandID)
      {
        return ProcessSwitchCommand(
          sSourceDeviceID,
          sDeviceID,
          sParameter,
          eCommand
        );
      }
      else
      {
        #if PARSER_LOG_LEVEL & PARSER_LOG_MASK_ERRORS
          LOG_FMT(
            "Parser: unknown command ID \"%s\".",
            sCommandID.c_str()
          );
        #endif

        return false;
      }

      return false;
    }

  }
}
