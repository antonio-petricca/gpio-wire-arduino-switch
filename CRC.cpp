#include "CRC.hpp"
#include "Logger.hpp"

uint16_t CRC16(const unsigned char* lpData, size_t nSize)
{
  // https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum
  // https://www.lammertbies.nl/comm/info/crc-calculation.html

  // CRC-CCITT (0xFFFF)

  uint16_t nCRC = 0xFFFF;

  while (nSize--)
  {
    uint8_t nTemp  = (nCRC >> 8) ^ *lpData++;
    nTemp         ^= (nTemp >> 4);

    nCRC =
        (nCRC << 8)
      ^ ((unsigned short) (nTemp << 12))
      ^ ((unsigned short) (nTemp << 5))
      ^ ((unsigned short) nTemp)
    ;
  }

  return nCRC;
}
