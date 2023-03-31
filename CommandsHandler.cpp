#include <EEPROM.h>

#include "CRC.hpp"
#include "CommandsHandler.hpp"
#include "Logger.hpp"

namespace Commands {
  namespace Handler {

    /***********/
    /* Private */
    /***********/

    unsigned long m_nLastBlinkTime = 0;  // 0 = Blinking disabled

    String        m_sDeviceID      = "";
    RawDeviceID   m_szTmpDeviceID  = ""; // Declared here to avoid stack overrun

    LOG_DECLARE_BUFFER();

    bool GetLED()
    {
      return (HIGH == digitalRead(PIN_LED));
    }

    void SetLED(bool bState)
    {
      digitalWrite(PIN_LED, (bState ? HIGH : LOW));
    }

    void ToggleLED()
    {
      SetLED(!GetLED());
    }

    void FlashLED()
    {
      bool bOldState = GetLED();

      for (int nIndex = 0; nIndex < (2 * LED_FLASH_COUNT); nIndex++)
      {
        ToggleLED();
        delay(LED_FLASH_TIMEOUT);
      }

      SetLED(bOldState);
    }

    void BlinkLEDStart()
    {
      m_nLastBlinkTime = micros();
    }

    void BlinkLEDStop()
    {
      m_nLastBlinkTime = 0;
    }

    bool CheckPairingButtonPressed()
    {
      bool bPressed = (LOW == digitalRead(PIN_BUTTON));

      if (!bPressed)
      {
        #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_ERRORS
          LOG("Command: pairing button not pressed.");
        #endif
      }

      return bPressed;
    }

    void ReadDeviceID()
    {
      // Reset device ID

      m_sDeviceID = "";

      BlinkLEDStart();

      // Read device ID

      memset(m_szTmpDeviceID, 0, sizeof(m_szTmpDeviceID));
      EEPROM.get(0, m_szTmpDeviceID);

      size_t nLength = strlen(m_szTmpDeviceID);

      if ((0 == nLength) || (nLength > MAX_DEVICE_ID_LENGTH))
      {
        #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_ERRORS

          LOG_FMT(
            "Command: invalid stored ID (size %d).",
            nLength
          );

        #endif

        return;
      }

      // Check CRC

      uint16_t nStoredCRC;
      EEPROM.get(sizeof(m_szTmpDeviceID), nStoredCRC);

      uint16_t nCalculatedCRC = CRC16(
        (const unsigned char*)m_szTmpDeviceID,
        nLength
      );

      if (nStoredCRC != nCalculatedCRC)
      {
        #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_ERRORS

          LOG_FMT(
            "Command: bad CRC (stored = 0x%04X, calc = 0x%04X).",
            nStoredCRC,
            nCalculatedCRC
          );

        #endif

        return;
      }

      #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_INFO

        LOG_FMT(
          "Command: read ID \"%s\", CRC = 0x%04X.",
          m_szTmpDeviceID,
          nStoredCRC
        );

      #endif

      // Store valid device ID

      m_sDeviceID = m_szTmpDeviceID;

      // Final check

      if (IsPaired())
      {
          BlinkLEDStop();
          SetLED(false);
      }
    }

    void ExecCommand_Pair(String sDeviceID)
    {
      if (CheckPairingButtonPressed())
      {
          memset(m_szTmpDeviceID, 0, sizeof(m_szTmpDeviceID));
          sDeviceID.toCharArray(m_szTmpDeviceID, sizeof(m_szTmpDeviceID));

          EEPROM.put(0, m_szTmpDeviceID);

          uint16_t nCRC = CRC16(
            (const unsigned char*)m_szTmpDeviceID,
            sDeviceID.length()
          );

          EEPROM.put(sizeof(m_szTmpDeviceID), nCRC);

          #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_INFO

            LOG_FMT(
              "Command: ID \"%s\", CRC = 0x%04X written.",
              m_szTmpDeviceID,
              nCRC
            );

          #endif

          m_sDeviceID = sDeviceID;

          BlinkLEDStop();
          SetLED(false);
      }
    }

    void ExecCommand_Switch(bool bStatus)
    {
      digitalWrite(PIN_RELAY, (bStatus ? LOW : HIGH));

      #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_INFO

        LOG_FMT(
          "Command: relay switched %s.",
          (bStatus ? "on" : "off")
        );

      #endif
    }

    void ExecCommand_Unpair()
    {
      if (CheckPairingButtonPressed())
      {
        for (unsigned int nIndex = 0 ; nIndex < EEPROM.length(); nIndex++)
        {
          EEPROM.write(nIndex, 0);
        }

        #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_INFO

          LOG_FMT(
            "Command: EEPROM (size %d) cleared.",
            EEPROM.length()
          );

        #endif

        m_sDeviceID = "";

        ExecCommand_Switch(false);

        BlinkLEDStart();
      }
    }

    /**********/
    /* Public */
    /**********/

    void Initialize()
    {
      // LED

      pinMode(PIN_LED, OUTPUT);

      // Relay

      pinMode(PIN_RELAY, OUTPUT);
      digitalWrite(PIN_RELAY, HIGH);

      // Button

      pinMode(PIN_BUTTON, INPUT_PULLUP);

      // Device ID

      ReadDeviceID();
    }

    bool IsPaired()
    {
      return m_sDeviceID.length();
    }

    void ProcessCommand(String sCommand)
    {
      #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_INFO

        LOG_FMT(
          "Command: handling command \"%s\"...",
          sCommand.c_str()
        );

      #endif

      Command eCommand;
      String  sDeviceID = m_sDeviceID;

      if (!Parse(sCommand, sDeviceID, eCommand))
      {
        #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_ERRORS

          LOG_FMT(
            "Command: command \"%s\" not parsed.",
            sCommand.c_str()
          );

        #endif
      }
      else
      {
        switch(eCommand)
        {
          case Command::SwitchOn :
            ExecCommand_Switch(true);
            break;

          case Command::SwitchOff :
            ExecCommand_Switch(false);
            break;

          case Command::Pair :
            ExecCommand_Pair(sDeviceID);
            break;

          case Command::Unpair :
            ExecCommand_Unpair();
            break;

          default:
            #if HANDLER_LOG_LEVEL & HANDLER_LOG_MASK_ERRORS

              LOG_FMT(
                "Command: unknown command ID \"%d\".",
                (int)eCommand
              );

            #endif
        }

        FlashLED();
      }
    }

    void BlinkLED()
    {
      if (0 != m_nLastBlinkTime)
      {
        unsigned long nTimeDiff = (micros() - m_nLastBlinkTime);

        if (nTimeDiff >= (unsigned long)LED_BLINK_TIMEOUT)
        {
            ToggleLED();
            m_nLastBlinkTime = micros();
        }
      }
    }

  }
}
