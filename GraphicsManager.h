#pragma once

#include "Defines.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
#define STRIP_HEIGHT 8
#define STRIP_SIZE_PIX (SCREEN_WIDTH * STRIP_HEIGHT)
#define STRIP_SIZE_BYTES (STRIP_SIZE_PIX * 2)

namespace roads
{
  class GraphicsManager
  {
    public:
      // returns the buffer for the next strip
      static uint16_t* StartFrame() noexcept;

      static void EndFrame() noexcept;

      // returns the buffer for the next strip
      static uint16_t* CommitStrip() noexcept;

      private:
        static uint16_t strip1[STRIP_SIZE_PIX];
        static uint16_t strip2[STRIP_SIZE_PIX];
        static uint16_t* currentStrip;
        static uint16_t* sentStrip;
  };
}
