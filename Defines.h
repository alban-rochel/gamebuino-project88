#pragma once

#include <Gamebuino-Meta.h>

namespace Gamebuino_Meta
{
  static SPISettings tftSPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);
};
