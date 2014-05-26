#include "JVS.h"

const char *JVS::_IO_ID = "SaturnJVS;I/O Board;Ver 1.00;04/05";
//const char *JVS::_IO_ID = "SEGA ENTERPRISES,LTD.;I/O 838-13683D ;Ver1.09;99/11";
const uint8_t JVS::_COMMAND_REVISION = 0x11;
const uint8_t JVS::_JAMMA_VIDEO_REVISION = 0x20;
const uint8_t JVS::_TRANSMISSION_SYSTEM_REVISION = 0x10;
const uint8_t JVS::_FEATURES[13] = { 0x01, 0x02, 0x0f, 0x00, 0x02, 0x02, 0x00, 0x00, 0x03, 0x08, 0x00, 0x00, 0x00 };

JVS::JVS(HardwareSerial &serial, uint8_t re_pin, uint8_t sense_pin) : _serial(serial), _re_pin(re_pin), _sense_pin(sense_pin), _address(-1), _switch_status_callback(NULL)
{
}

void JVS::begin()
{
  _serial.begin(115200);

  pinMode(_re_pin, OUTPUT);
  digitalWrite(_re_pin, LOW);

  pinMode(_sense_pin, OUTPUT);
  digitalWrite(_sense_pin, HIGH);
}

inline uint8_t JVS::readByte()
{
  while (!_serial.available())
    ;

  uint8_t byte = _serial.read();

  // Escape
  if (byte == 0xD0) {
    while (!_serial.available())
      ;

    byte = _serial.read();

    switch (byte) {
      case 0xDF:
        byte = 0xE0;
        break;
      case 0xCF:
        byte = 0xD0;
        break;
      default:
        // Error!
        break;
    }
  }

  return byte;
}

bool JVS::readPacket()
{
  uint8_t data = _serial.read();
  if (data != 0xE0)
    return false;

  uint16_t bytes_read = 0;

  while (bytes_read < 2) {
    data = readByte();
    _read_buffer[bytes_read] = data;
    bytes_read++;
  }

  Header *header = (Header *)_read_buffer;

  while (bytes_read < header->size + 2) {
    data = readByte();
    _read_buffer[bytes_read] = data;
    bytes_read++;
  }

  _read_buffer_size = bytes_read;

  /*
  Serial.println("Packet received!");
  char buf[3];
  for (int i = 0; i < bytes_read; i++) {
    sprintf(buf, "%02x ", _read_buffer[i]);
    Serial.print(buf);
  }
  Serial.println();
  */

  if (header->address != 0xFF && header->address != _address) {
    // Not for us
    return false;
  }

  return true;
}

void JVS::writeByte(uint8_t byte, bool escape)
{
  if (escape && (byte == 0xE0 || byte == 0xD0)) {
    _serial.write(0xD0);
    _serial.write(byte - 1);
  } else
    _serial.write(byte);
}

void JVS::writePacket()
{
  if (_write_buffer_size == 0)
    return;

  digitalWrite(_re_pin, HIGH);

  writeByte(0xE0, false);
  for (uint16_t i = 0; i < _write_buffer_size; i++)
    writeByte(_write_buffer[i], true);

  _serial.flush();
  digitalWrite(_re_pin, LOW);

  /*
  Serial.println("Sent packet!");
  char buf[3];
  for (int i = 0; i < _write_buffer_size; i++) {
    sprintf(buf, "%02x ", _write_buffer[i]);
    Serial.print(buf);
  }
  Serial.println();
  */
}

uint8_t JVS::checksum(uint8_t *data, uint16_t length)
{
  uint8_t sum = 0;
  for (uint8_t *byte = data; byte < data + length; byte++)
    if (*byte == 0xE0 || *byte == 0xD0) {
      sum += 0xD0;
      sum += *byte - 1;
    } else
      sum += *byte;

  return sum;
}

void JVS::handleCommand()
{
  Header *command_header = (Header *)_read_buffer;

  Header *response_header = (Header *)_write_buffer;
  uint8_t *status = _write_buffer + sizeof(Header);

  response_header->address = 0x00;
  response_header->size = 0x00;
  *status = STATUS_NORMAL;

  if (checksum(_read_buffer, _read_buffer_size - 1) != _read_buffer[_read_buffer_size - 1]) {
    *status = STATUS_SUM_ERROR;
    //Serial.println("Checksum error");
  }

  uint8_t *command = (_read_buffer + sizeof(Header));
  uint8_t *response = (_write_buffer + sizeof(Header) + 1);
  while (*status == STATUS_NORMAL && command < _read_buffer + sizeof(Header) + command_header->size - 1) {
    switch (*command) {
      case COMMAND_RESET:
        command += 2;

        _address = -1;
        digitalWrite(_sense_pin, HIGH);
        break;

      case COMMAND_SET_ADDRESS:
        _address = *(command + 1);
        command += 2;

        digitalWrite(_sense_pin, LOW);

        response[0] = REPORT_NORMAL;
        response++;
        break;

      case COMMAND_GET_IO_ID:
        command += 1;

        response[0] = REPORT_NORMAL;
        response++;

        memcpy(response, _IO_ID, strlen(_IO_ID) + 1);
        response += strlen(_IO_ID) + 1;
        break;

      case COMMAND_GET_COMMAND_REVISION:
        command += 1;

        response[0] = REPORT_NORMAL;
        response[1] = _COMMAND_REVISION;
        response += 2;
        break;

      case COMMAND_GET_JAMMA_VIDEO_REVISION:
        command += 1;

        response[0] = REPORT_NORMAL;
        response[1] = _JAMMA_VIDEO_REVISION;
        response += 2;
        break;

      case COMMAND_GET_TRANSMISSION_SYSTEM_REVISION:
        command += 1;

        response[0] = REPORT_NORMAL;
        response[1] = _TRANSMISSION_SYSTEM_REVISION;
        response += 2;
        break;

      case COMMAND_GET_FEATURES:
        command += 1;

        response[0] = REPORT_NORMAL;
        response++;

        // TODO: Make this settable?
        memcpy(response, _FEATURES, 3 * 4 + 1);
        response += 3 * 4 + 1;
        break;

      case COMMAND_GET_SWITCH_STATE:
      {
        uint8_t player_count = command[1];
        uint8_t byte_count = command[2];
        command += 3;

        response[0] = REPORT_NORMAL;
        response++;

        // System data
        response[0] = 0x00;
        response++;

        memset(response, 0, player_count * byte_count);

        for (uint8_t i = 0; i < player_count; i++) {
          if (_switch_status_callback) {
            _switch_status_callback(i, (PlayerSwitchStatus *)response);
          }
          response += byte_count;
        }
        break;
      }

      case COMMAND_GET_COIN_STATE:
      {
        uint8_t slot_count = command[1];
        command += 2;

        response[0] = REPORT_NORMAL;
        response++;

        for (uint8_t i = 0; i < slot_count; i++) {
          response[0] = 0x80;
          response[1] = 0x00;
          response += 2;
        }
        break;
      }

      case COMMAND_GET_ANALOG_STATE:
      {
        uint8_t channel_count = command[1];
        command += 2;

        response[0] = REPORT_NORMAL;
        response++;

        for (uint8_t i = 0; i < channel_count; i++) {
          response[0] = 0x00;
          response[1] = 0x00;
          response += 2;
        }
        break;
      }

      default:
        *status = STATUS_COMMAND_UNKNOWN;

        //Serial.print("Unknown command: ");
        //Serial.print(*command, HEX);
        //Serial.println();
        break;
    }
  }

  if (*status != STATUS_NORMAL) {
    _write_buffer_size = 3;
  } else if (response - _write_buffer == sizeof(Header) + 1) {
    _write_buffer_size = 0;
  } else {
    response_header->size = response - _write_buffer - sizeof(Header) + 1;
    _write_buffer_size = response_header->size + sizeof(Header);
    *response = checksum(_write_buffer, _write_buffer_size - 1);
  }
}

bool JVS::loop()
{
  if (_serial.available()) {
    if (!readPacket())
      return false;

    handleCommand();
    writePacket();

    return true;
  } else
    return false;
}

void JVS::setSwitchStatusCallback(void (*switch_status_callback)(uint8_t, PlayerSwitchStatus *))
{
  _switch_status_callback = switch_status_callback;
}

