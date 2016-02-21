#ifndef SATURN_PAD_H
#define SATURN_PAD_H

#include <Arduino.h>

class SaturnPad
{
  public:
    struct DigitalPadState {
      uint8_t id:3;

      uint8_t button_start:1;
      uint8_t button_up:1;
      uint8_t button_down:1;
      uint8_t button_left:1;
      uint8_t button_right:1;
      uint8_t button_a:1;
      uint8_t button_b:1;
      uint8_t button_c:1;
      uint8_t button_x:1;
      uint8_t button_y:1;
      uint8_t button_z:1;
      uint8_t button_l:1;
      uint8_t button_r:1;
    };

    struct AnalogPadState {
      uint8_t CTRL_BYTE_COUNT:4;
      uint8_t CTRL_ID_3D:4;

      uint8_t BTN_B:1;
      uint8_t BTN_C:1;
      uint8_t BTN_A:1;
      uint8_t BTN_START:1;
      uint8_t BTN_UP:1;
      uint8_t BTN_DOWN:1;
      uint8_t BTN_LEFT:1;
      uint8_t BTN_RIGHT:1;

      uint8_t CTRL_ID:3;
      uint8_t BTN_L:1;
      uint8_t BTN_Z:1;
      uint8_t BTN_Y:1;
      uint8_t BTN_X:1;
      uint8_t BTN_R:1;

      uint8_t ANLG_STICK_LR;
      uint8_t ANLG_STICK_UD;
      uint8_t ANLG_TRIGGER_R;
      uint8_t ANLG_TRIGGER_L;
    };

    struct Nibble {
      union {
        uint8_t nibble:4;
        struct {
          uint8_t bit0:1;
          uint8_t bit1:1;
          uint8_t bit2:1;
          uint8_t bit3:1;
        } components;
      };
    };

    enum PadType {
      TYPE_NONE,
      TYPE_DIGITAL,
      TYPE_ANALOG
    };

    SaturnPad(uint8_t s0_pin, uint8_t s1_pin, uint8_t s2_pin, uint8_t d0_pin, uint8_t d1_pin, uint8_t d2_pin, uint8_t d3_pin);

    void begin();
    void loop();

    PadType padType();
    DigitalPadState digitalPadState();
    AnalogPadState analogPadState();

  private:
    void setType();
    Nibble readNibble(uint8_t s0, uint8_t s1);
    void readDigital();
    void readAnalog();

    uint8_t _s_pins[3];
    uint8_t _d_pins[4];

    PadType _pad_type;
    DigitalPadState _digital_pad_state;
    AnalogPadState _analog_pad_state;
};

#endif

