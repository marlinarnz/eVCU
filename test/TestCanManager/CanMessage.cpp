/* ============================== CAN message ============================= 
 * Defines the functionality of the CAN message class. Bit/Byte order is
 * Motorola forward LSB.
 */

#include "CanMessage.h"


/* ============================== Message constructor =====================
 * The constructor saves the necessary attributes ID and send interval as
 * well as the content array with a default of LSCF bytes, 8 bits each. The
 * MCP2515 object enables sending the message.
 * @param ident: long ID of that message
 * @param canObj: MCP2515 class object pointer
 * @param interv: integer interval for periodic sending in millis. 0 for none
 */
CanMessage::CanMessage(uint32_t ident, MCP2515* canObj, int interv)
  : _id(ident), _canObj(canObj), _interval(interv), _lastSent(0), _frame{} {
  CanMessage::setFrameBytes(0);
}
// Default constructor to enable arrays
CanMessage::CanMessage()
  : _id(0), _canObj(NULL), _interval(0), _lastSent(0), _frame{} {}


/* ============================== Return ID ===============================
 * Returns the message ID as decimal
 * @return: long ID
 */
uint32_t CanMessage::getId() {
  return _id;
}


/* ============================== Set frame ===============================
 * Sets each byte of the frame to the given value. Can be used for testing.
 * @param val: (optional) byte. Default 0
 */
void CanMessage::setFrameBytes(uint8_t val) {
  for (uint8_t i=0; i<LSCF; i++) {
    CanMessage::writeByte(i, val);
  }
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
    struct can_frame frame;
    frame.can_id = getId();
    frame.can_dlc = LSCF;
    for (uint8_t i=0; i<LSCF; i++) {
      frame.data[i] = _frame[i];
    }
    if (_id > 2047) {
      // Extended ID
      _canObj->sendMessage(MCP2515::TXB1, &frame);
    } else {
      _canObj->sendMessage(&frame);
    }
    _lastSent = millis();
  }
}


/* ============================== Read byte ===============================
 * Returns the value of a whole byte (the one which holds the lsb). Yields
 * better performance as the readSignal function.
 * @param b: integer byte to return
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 * @return: float signal content, already converted. -1 as error code
 */
float CanMessage::readByte(int b, float conv, int offset) {
  if (CanMessage::_checkLSBandLen(b * 8, 8)) {
    return _frame[b] * conv - offset;
  }
  // Return error code otherwise  
  return -1;
}


/* ============================== Write byte ==============================
 * Writes the given byte. Yields better performance as the writeSignal
 * function.
 * @param b: integer byte to write
 * @param val: long value to be written
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 */
void CanMessage::writeByte(int b, long val, float conv, int offset) {
  if (conv > 0 && offset >= 0
      && CanMessage::_checkLSBandLen(b * 8, 8)) {
    _frame[b] = (uint8_t)(val / conv + offset);
  }
}


/* ============================== Read CAN signals ========================
 * Rertrieves the value of one signal from own content.
 * readSignalLE applies little endian byte ordering. 
 * readSignalBE applies big endian byte odering (Motorola forward LSB).
 * @param lsb: integer least significant bit
 * @param len: integer length of the signal in bits
 * @param conv: (optional) float conversion factor of physical signal value
 * @param offset: (optional) integer offset of physical signal value
 * @return: float signal content, already converted. -1 as error code
 * 
 * TODO: Properly return four byte signals with odd lsb
 */
float CanMessage::readSignalLE(int lsb, int len, float conv, int offset) {
  if (CanMessage::_checkLSBandLen(lsb, len)) {
    uint32_t sig = 0;
    uint8_t lastByte = CanMessage::_getLastByte(lsb, len);
    uint8_t firstByte = int(lsb / 8);
    uint8_t endBit = (lsb + len) % 8;

    // Handle the first byte
    sig = _frame[firstByte];
    sig >>= lsb % 8;
    if (firstByte == lastByte) {
      if (endBit != 0) {
        sig &= 255 >> ((8 - endBit) + (lsb % 8));
      }
    } else {
      // Loop through the frame with little endian byte order
      for (int i=firstByte-1; i>=lastByte-1; i--) {
        if (i == lastByte and endBit != 0) {
          // Truncate last byte
          sig <<= endBit;
          sig |= _frame[i] & (255 >> (8 - endBit));
        } else {
          // Add it to the signal value
          sig <<= 8;
          sig |= _frame[i];
        }
      }
    }

    // Return the converted signal
    return sig * conv - offset;
  }
  // Return error code otherwise  
  return -1;
}

// Big endian
float CanMessage::readSignalBE(int lsb, int len, float conv, int offset) {
  if (CanMessage::_checkLSBandLen(lsb, len)) {
    uint32_t sig = 0;
    uint8_t lastByte = CanMessage::_getLastByte(lsb, len);
    uint8_t firstByte = int(lsb / 8);
    uint8_t endBit = (lsb + len) % 8;

    sig = _frame[lastByte];
    if (endBit != 0) {
      // Truncate last byte
      sig &= 255 >> (8 - endBit);
    }

    // Loop through the frame with big endian byte order
    for (int i=lastByte+1; i<=firstByte; i++) {
      sig <<= 8;
      sig |= _frame[i];
    }

    // Truncate first byte
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
    uint32_t sig = (uint32_t)(val / conv + offset);

    // Get metrics
    uint8_t lastByte = CanMessage::_getLastByte(lsb, len);
    uint8_t firstByte = int(lsb / 8);
    uint8_t rightEmpty = lsb % 8;
    uint8_t leftFilled = (rightEmpty + len) % 8;
    uint8_t oldLeft;

    // If the signal does not end with a byte, save the rest for later
    if (leftFilled > 0) {
      oldLeft = _frame[lastByte] & (0xFF << leftFilled); // alternatively use (uint8_t)(CanMessage::readSignalBE(lastByte * 8, 8))
    }
    
    // If the signal does not fill the first byte entirely, keep the bits
    // from other signals as they were
    if (rightEmpty > 0) {
      uint8_t oldRight = (uint8_t)(CanMessage::readSignalBE(firstByte * 8, rightEmpty));
      _frame[firstByte] = (uint8_t)(sig << rightEmpty) | oldRight;
      // And take one off the loop
      sig >>= 8 - rightEmpty;
      firstByte--;
    }

    // Loop through the signal with little endian byte order
    for (int i=firstByte; i>=lastByte; i--) {
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
  return len > 0 && lsb < LSCF * 8 && lsb >= 0 && len <= sizeof(long) * 8
    && ((len <= lsb + 8 && lsb >= 8) || lsb + len <= 8);
}


/* ============================== Get last byte ===========================
 * Calculates the number of the last byte of a message with Motorola
 * forward LSB ordering.
 * @param lsb: integer least significant bit
 * @param len: integer length of a signal in bits
 * @return: uint8_t number of last byte of message
 */
uint8_t CanMessage::_getLastByte(int lsb, int len) {
  uint8_t startByte = int(lsb / 8);
  if (lsb % 8 + len <= 8) {
    return startByte;
  } else {
    return startByte - int((lsb%8 + len - 1) / 8);
  }
}
