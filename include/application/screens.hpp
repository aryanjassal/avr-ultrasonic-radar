#pragma once

#include <stdint.h>

enum class ScreenID : uint8_t {
  None,
  Loading,
  Calibration,
  Menu,
  Alarm,
  Popup,
};