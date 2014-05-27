#ifndef JVS_H
#define JVS_H

#include <Arduino.h>

class JVS
{
  public:
    enum Command {
      // Commands applying to all nodes (Address 0xFF)
      COMMAND_RESET                            = 0xF0,
      COMMAND_SET_ADDRESS                      = 0xF1,
      COMMAND_SET_TRANSMISSION_SYSTEM          = 0xF2,

      // Initial setting commands
      COMMAND_GET_IO_ID                        = 0x10,
      COMMAND_GET_COMMAND_REVISION             = 0x11,
      COMMAND_GET_JAMMA_VIDEO_REVISION         = 0x12,
      COMMAND_GET_TRANSMISSION_SYSTEM_REVISION = 0x13,
      COMMAND_GET_FEATURES                     = 0x14,
      COMMAND_TELL_MAINBOARD_ID                = 0x15,

      // Data I/O commands
      COMMAND_GET_SWITCH_STATE                 = 0x20,
      COMMAND_GET_COIN_STATE                   = 0x21,
      COMMAND_GET_ANALOG_STATE                 = 0x22,
      COMMAND_GET_ROTARY_STATE                 = 0x23,
      COMMAND_GET_KEY_CODE_STATE               = 0x24,
      COMMAND_GET_SCREEN_POSITION_STATE        = 0x25,
      COMMAND_GET_GENERAL_PURPOSE_SWITCH_STATE = 0x26,

      // TODO: 2E-38
    };

    enum Status {
      STATUS_NORMAL               = 0x01,
      STATUS_COMMAND_UNKNOWN      = 0x02,
      STATUS_SUM_ERROR            = 0x03,
      STATUS_ACKNOWLEDGE_OVERFLOW = 0x04,
    };

    enum Report {
      REPORT_NORMAL                 = 0x01,
      REPORT_MASTER_PARAMETER_ERROR = 0x02,
      REPORT_SLAVE_PARAMETER_ERROR  = 0x03,
      REPORT_BUSY                   = 0x04,
    };

    struct Header {
      uint8_t address;
      uint8_t size;
    };

    struct PlayerSwitchStatus {
      uint8_t button_2:1;
      uint8_t button_1:1;
      uint8_t button_right:1;
      uint8_t button_left:1;
      uint8_t button_down:1;
      uint8_t button_up:1;
      uint8_t button_service:1;
      uint8_t button_start:1;
      uint8_t _junk:2;
      uint8_t button_8:1;
      uint8_t button_7:1;
      uint8_t button_6:1;
      uint8_t button_5:1;
      uint8_t button_4:1;
      uint8_t button_3:1;
    };


    JVS(HardwareSerial &serial, uint8_t re_pin, uint8_t sense_pin);

    void begin();
    bool loop();

    void setSwitchStatusCallback(void (*switch_status_callback)(uint8_t, PlayerSwitchStatus *));

  private:
    uint8_t readByte();
    bool readPacket();

    void writeByte(uint8_t byte, bool escape);
    void writePacket();

    uint8_t checksum(uint8_t *data, uint16_t length);

    void handleCommand();

    HardwareSerial &_serial;
    uint8_t _re_pin;
    uint8_t _sense_pin;
    int8_t _address;

    uint8_t _read_buffer[257];
    uint16_t _read_buffer_size;
    uint8_t _write_buffer[257];
    uint16_t _write_buffer_size;

    void (*_switch_status_callback)(uint8_t, PlayerSwitchStatus *);

    static const char *_IO_ID;
    static const uint8_t _COMMAND_REVISION;
    static const uint8_t _JAMMA_VIDEO_REVISION;
    static const uint8_t _TRANSMISSION_SYSTEM_REVISION;
    static const uint8_t _FEATURES[17];
};

#endif
