#include "JVS.h"
#include "Wire.h"

JVS jvs(Serial, A3, 12);

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

struct ControllerStatus {
  DigitalStatus digital;
  uint8_t *analog;
};

struct ControllerBoard {
  uint8_t controller_count;
  uint8_t digital_count;
  uint8_t analog_count;
  ControllerStatus *controllers;
};

ControllerBoard *controllerBoard = NULL;

void switchStatusCallback(uint8_t controller, JVS::PlayerSwitchStatus *switchStatus)
{
  ControllerStatus *controllerStatus = &controllerBoard->controllers[controller];
  DigitalStatus *digital = &controllerStatus->digital;

  switchStatus->button_start = digital->button_start;
  switchStatus->button_up = digital->button_up;
  switchStatus->button_down = digital->button_down;
  switchStatus->button_left = digital->button_left;
  switchStatus->button_right = digital->button_right;
  switchStatus->button_1 = digital->button_1;
  switchStatus->button_2 = digital->button_2;
  switchStatus->button_3 = digital->button_3;
  switchStatus->button_4 = digital->button_4;
  switchStatus->button_5 = digital->button_5;
  switchStatus->button_6 = digital->button_6;
  switchStatus->button_7 = digital->button_7;
  switchStatus->button_8 = digital->button_8;
}

void setup()
{
  Wire.begin();

  jvs.begin();
  jvs.setSwitchStatusCallback(&switchStatusCallback);
}

void commandInfo()
{
  Wire.beginTransmission(2);
  Wire.write(COMMAND_INFO);
  Wire.endTransmission();

  delayMicroseconds(500);

  Wire.requestFrom(2, 3);
  if (Wire.available() == 3) {
    controllerBoard = (ControllerBoard *)calloc(1, sizeof(ControllerBoard));
    controllerBoard->controller_count = Wire.read();
    controllerBoard->digital_count = Wire.read();
    controllerBoard->analog_count = Wire.read();

    controllerBoard->controllers = (ControllerStatus *)calloc(controllerBoard->controller_count, sizeof(ControllerStatus));
    for (uint8_t controllerIdx = 0; controllerIdx < controllerBoard->controller_count; controllerIdx++) {
      ControllerStatus *controller = &controllerBoard->controllers[controllerIdx];
      controller->analog = (uint8_t *)calloc(controllerBoard->analog_count, sizeof(uint8_t));
    }
  }
}

void commandStatus()
{
  Wire.beginTransmission(2);
  Wire.write(COMMAND_STATUS);
  Wire.endTransmission();

  delayMicroseconds(500);

  uint8_t responseLength = controllerBoard->controller_count * (2 + controllerBoard->analog_count);
  Wire.requestFrom((uint8_t)2, responseLength);
  if (Wire.available() == responseLength) {
    for (uint8_t controllerIdx = 0; controllerIdx < controllerBoard->controller_count; controllerIdx++) {
      ControllerStatus *controller = &controllerBoard->controllers[controllerIdx];
      ((uint8_t *)&(controller->digital))[0] = Wire.read();
      ((uint8_t *)&(controller->digital))[1] = Wire.read();
      for (uint8_t analogIdx = 0; analogIdx < controllerBoard->analog_count; analogIdx++) {
        controller->analog[analogIdx] = Wire.read();
      }
    }
  }
}

void loop()
{
  if (jvs.loop()) {
    if (controllerBoard == NULL) {
      commandInfo();
    } else {
      commandStatus();
    }
  }
}
