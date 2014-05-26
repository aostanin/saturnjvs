#include "SaturnPad.h"

SaturnPad::SaturnPad(uint8_t s0_pin, uint8_t s1_pin, uint8_t s2_pin, uint8_t d0_pin, uint8_t d1_pin, uint8_t d2_pin, uint8_t d3_pin)
{
  _s_pins[0] = s0_pin;
  _s_pins[1] = s1_pin;
  _s_pins[2] = s2_pin;
  _d_pins[0] = d0_pin;
  _d_pins[1] = d1_pin;
  _d_pins[2] = d2_pin;
  _d_pins[3] = d3_pin;
}

void SaturnPad::begin()
{
  for (uint8_t i = 0; i < 3; i++)
    pinMode(_s_pins[i], OUTPUT);
  for (uint8_t i = 0; i < 4; i++)
    pinMode(_d_pins[i], INPUT_PULLUP);

  _pad_type = TYPE_NONE;
}

void SaturnPad::loop()
{
  setType();

  switch (_pad_type) {
    case TYPE_NONE:
    case TYPE_DIGITAL:
      readDigital();
      break;

    case TYPE_ANALOG:
      readAnalog();
      break;
  }
}

SaturnPad::PadType SaturnPad::padType()
{
  return _pad_type;
}

SaturnPad::DigitalPadState SaturnPad::digitalPadState()
{
  return _digital_pad_state;
}

SaturnPad::AnalogPadState SaturnPad::analogPadState()
{
  return _analog_pad_state;
}

void SaturnPad::setType()
{
  switch (_digital_pad_state.id) {
    case 0x4: // Standard pad
      _pad_type = TYPE_DIGITAL;
      break;

    default:
      _pad_type = TYPE_NONE;
      break;
  }
}

SaturnPad::Nibble SaturnPad::readNibble(uint8_t s0, uint8_t s1)
{
  digitalWrite(_s_pins[0], s0);
  digitalWrite(_s_pins[1], s1);

  delayMicroseconds(500);

  Nibble nibble;
  nibble.components.bit0 = digitalRead(_d_pins[0]);
  nibble.components.bit1 = digitalRead(_d_pins[1]);
  nibble.components.bit2 = digitalRead(_d_pins[2]);
  nibble.components.bit3 = digitalRead(_d_pins[3]);
  return nibble;
}

void SaturnPad::readDigital()
{
  Nibble nibble;

  nibble = readNibble(HIGH, HIGH);
  _digital_pad_state.id = nibble.nibble & 0x3;
  _digital_pad_state.button_l = !nibble.components.bit3;

  nibble = readNibble(HIGH, LOW);
  _digital_pad_state.button_b = !nibble.components.bit0;
  _digital_pad_state.button_c = !nibble.components.bit1;
  _digital_pad_state.button_a = !nibble.components.bit2;
  _digital_pad_state.button_start = !nibble.components.bit3;

  nibble = readNibble(LOW, HIGH);
  _digital_pad_state.button_up = !nibble.components.bit0;
  _digital_pad_state.button_down = !nibble.components.bit1;
  _digital_pad_state.button_left = !nibble.components.bit2;
  _digital_pad_state.button_right = !nibble.components.bit3;

  nibble = readNibble(LOW, LOW);
  _digital_pad_state.button_z = !nibble.components.bit0;
  _digital_pad_state.button_y = !nibble.components.bit1;
  _digital_pad_state.button_x = !nibble.components.bit2;
  _digital_pad_state.button_r = !nibble.components.bit3;
}

void SaturnPad::readAnalog()
{
}

