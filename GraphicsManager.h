#pragma once

#include "Defines.h"

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
