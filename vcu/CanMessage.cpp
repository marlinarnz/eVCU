/* ============================== CAN message ============================= 
 * Defines the functionality of the CAN message class. Bit/Byte order is
 * Motorola forward LSB.
 */

#include "CanMessage.h"


/* ============================== Message constructor =====================
 * The constructor saves the necessary attributes ID and send interval as
 * well as the content array with a default of LSCF bytes, 8 bits each. The
 * MCP_CAN object enables sending the message.
 * @param ident: integer ID of that message in standard format (hex)
 * @param canObj: MCP_CAN class object pointer
 * @param interv: integer interval for periodic sending in millis. 0 for none
 */
CanMessage::CanMessage(int ident, MCP_CAN* canObj, int interv) {
  MCP_CAN* _canObj = canObj;
  uint8_t _frame[LSCF];
  int _interval = interv;
  long _lastSent;
  uint32_t id = ident;
}
// Default constructor to enable arrays
CanMessage::CanMessage() {
}


/* ============================== Send CAN message ========================
 * Sends itself on the bus, if the time interval for periodic messages is
 * exceeded or there is no interval set.
 * @param interv: (optional) integer interval for periodic sending in millis
 */
void CanMessage::send(int interv) {
  if (interv > 0) {
    _interval = interv;
  }
  if ((_interval > 0 && millis() - _lastSent > _interval) || _interval == 0) {
    _canObj->sendMsgBuf(id, 0, LSCF, _frame);
  }
  _lastSent = millis();
}


/* ============================== Read CAN signals ========================
 * Rertrieves the value of one signal from own content.
 * Always applies Motorola forward LSB.
 * @param lsb: integer least significant bit
 * @param len: integer length of the signal in bits
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 * @return sig: float signal content, already converted. -1 as error code
 */
float CanMessage::readSignal(int lsb, int len, float conv, int offset) {
  if (CanMessage::_checkLSBandLen(lsb, len)) {
    long sig;

    // Loop through the frame with little endian byte order
    uint8_t lastByte = CanMessage::_getLastByte(lsb, len);
    for (uint8_t i=int(lsb / 8); i>=lastByte; i--) {
      uint8_t newByte = _frame[i];
      // Truncate last byte
      if (i == lastByte) {
        newByte &= 0xFF >> 8 - ((lsb + len) % 8);
      }
      // Add it to the signal value
      sig <<= 8;
      sig |= newByte;
    }
    // Truncate the first byte
    sig >>= lsb % 8;
    
    // Return the converted signal
    return sig * conv - offset;
  }
  // Return error code otherwise  
  return -1;
}


/* ============================== Write CAN signals =======================
 * Writes a value of one signal into the frame.
 * Always applies Motorola forward LSB.
 * @param lsb: integer least significant bit
 * @param len: integer length of the signal in bits
 * @param val: long value to be written. Cannot be larger than 4 bytes
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 */
void CanMessage::writeSignal(int lsb, int len, long val, float conv, int offset) {
  if (conv > 0 && offset >= 0
      && CanMessage::_checkLSBandLen(lsb, len)) {
    // Convert the signal
    unsigned long sig = (unsigned long)(val / conv + offset);

    // Loop through the frame with little endian byte order
    uint8_t lastByte = CanMessage::_getLastByte(lsb, len);
    uint8_t firstByte = int(lsb / 8);
    uint8_t rightEmpty = lsb % 8;
    uint8_t leftFilled = (rightEmpty + len) % 8;
    uint8_t oldLeft = (uint8_t)(CanMessage::readSignal(lastByte * 8, 8))
                      & (0xFF << leftFilled);
    // If signal does not start at the beginning of a byte, handle first
    // byte without affecting information from other signals
    if (rightEmpty > 0) {
      uint8_t oldRight = (uint8_t)(CanMessage::readSignal(firstByte * 8, rightEmpty));
      _frame[firstByte] = (uint8_t)(sig << rightEmpty) | oldRight;
      sig >>= 8 - rightEmpty;
      firstByte--;
    }
    for (uint8_t i=firstByte; i>=lastByte; i--) {
      _frame[i] = (uint8_t)(sig);
      sig >>= 8;
    }
    // If the signal does not fill the last byte entirely, keep the bits
    // from other signals as they were
    if (leftFilled > 0) {
      _frame[lastByte] = (_frame[lastByte] & (0xFF >> (8 - leftFilled)))
                         | oldLeft;
    }
  }
}


/* ============================== Check LSB and len =======================
 * Checks whether lsb and len are valid parameters for a CAN frame with 
 * Motorola forward LSB ordering.
 * @param lsb: integer least significant bit
 * @param len: integer length of a signal in bits
 * @return: boolean whether lsb and len are valid
 */
bool CanMessage::_checkLSBandLen(int lsb, int len) {
  return len > 0 && lsb < LSCF * 8 && lsb >= 0 && len <= sizeof(long) * 8;
}


/* ============================== Get last byte ===========================
 * Calculates the number of the last byte of a message with Motorola
 * forward LSB ordering.
 * @param lsb: integer least significant bit
 * @param len: integer length of a signal in bits
 * @return: uint8_t number of last byte of message
 */
uint8_t CanMessage::_getLastByte(int lsb, int len) {
  return int(min(lsb, lsb - len + 8) / 8);
}
