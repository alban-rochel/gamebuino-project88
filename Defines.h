#pragma once

#include <Gamebuino-Meta.h>

namespace Gamebuino_Meta
{
  static SPISettings tftSPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);
};

#define COLOR_R(r) ((uint16_t)(r & 0xF8))
#define COLOR_G_LOW(g) ((uint16_t)(g) << 13)
#define COLOR_G_HIGH(g) ((uint16_t)(g) >> 5)
#define COLOR_B(b) ((uint16_t)(b & 0xF8) << 5)

#define COLOR_565(r, g, b) (COLOR_R(r) | COLOR_G_LOW(g) | COLOR_G_HIGH(g) | COLOR_B(b))
