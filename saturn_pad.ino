#include "SaturnPad.h"
#include <Wire.h>

SaturnPad pad1(5, 6, 0, 3, 1, 4, 2);
SaturnPad pad2(11, 12, 7, 9, 8, 10, 13);

enum Command {
  COMMAND_INFO = 0x10,
  COMMAND_STATUS = 0x20,
};

struct DigitalStatus {
  uint8_t button_start:1;
  uint8_t button_up:1;
  uint8_t button_down:1;
  uint8_t button_left:1;
  uint8_t button_right:1;
  uint8_t button_1:1;
  uint8_t button_2:1;
  uint8_t button_3:1;
  uint8_t button_4:1;
  uint8_t button_5:1;
  uint8_t button_6:1;
  uint8_t button_7:1;
  uint8_t button_8:1;
};

Command lastCommand;

void respondInfo()
{
  // 2 controllers, 13 digital, 4 analog
  uint8_t response[] = { 2, 13, 4 };
  Wire.write(response, 3);
}

void respondStatus()
{
  uint8_t responseLength = 2 * 2 * 4;
  uint8_t response[responseLength];
  memset(response, 0, responseLength);

  for (uint8_t controllerIdx = 0; controllerIdx < 2; controllerIdx++) {
    SaturnPad::DigitalPadState state;
    switch (controllerIdx) {
      case 0:
        state = pad1.digitalPadState();
        break;
      case 1:
        state = pad2.digitalPadState();
        break;
    }

    DigitalStatus *digitalStatus = (DigitalStatus *)(response + controllerIdx * (2 + 4));

    digitalStatus->button_start = state.button_start;
    digitalStatus->button_up = state.button_up;
    digitalStatus->button_down = state.button_down;
    digitalStatus->button_left = state.button_left;
    digitalStatus->button_right = state.button_right;
    digitalStatus->button_1 = state.button_a;
    digitalStatus->button_2 = state.button_b;
    digitalStatus->button_3 = state.button_c;
    digitalStatus->button_4 = state.button_x;
    digitalStatus->button_5 = state.button_y;
    digitalStatus->button_6 = state.button_z;
    digitalStatus->button_7 = state.button_l;
    digitalStatus->button_8 = state.button_r;

    for (uint8_t analogIdx = 0; analogIdx < 4; analogIdx++) {
      uint8_t *analog = (uint8_t *)digitalStatus + 2 + analogIdx;
      *analog = 0;
    }
  }

  Wire.write(response, responseLength);
}

void dataReceived(int numBytes)
{
  lastCommand = (Command)Wire.read();
}

void dataRequested()
{
  switch (lastCommand) {
    case COMMAND_INFO:
      respondInfo();
      break;

    case COMMAND_STATUS:
      respondStatus();
      break;
  }
}

void setup()
{
  pad1.begin();
  pad2.begin();

  Wire.begin(2);
  Wire.onReceive(dataReceived);
  Wire.onRequest(dataRequested);
}

void loop()
{
  pad1.loop();
  pad2.loop();
}
