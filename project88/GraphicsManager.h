#pragma once

#include "Defines.h"

namespace  roads
{
  
  class GraphicsManager
  {
    public:

      struct Task
      {
        void (*function)(void*);
        void* param;
      };

      struct TaskSet
      {
        Task* tasks;
        uint8_t taskCount;
        uint8_t currentTask;
      };

      

      GraphicsManager(uint16_t* strip1,
                      uint16_t* strip2) noexcept;
    
      // returns the buffer for the next strip
      uint16_t* StartFrame() noexcept;

      void EndFrame() noexcept;

      // returns the buffer for the next strip
      uint16_t* CommitStrip(TaskSet* taskSet) noexcept;

    private:
     uint16_t* strip1;
     uint16_t* strip2;
     uint16_t* currentStrip;
     uint16_t* sentStrip;
  };
}
