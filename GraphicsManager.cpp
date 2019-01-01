#include "GraphicsManager.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
#define STRIP_HEIGHT 8
#define STRIP_SIZE_PIX (SCREEN_WIDTH * STRIP_HEIGHT)
#define STRIP_SIZE_BYTES (STRIP_SIZE_PIX * 2)

using namespace roads;

#ifndef DMA_DESC_COUNT
#define DMA_DESC_COUNT (3)
#endif

//#define DEBUG_PERF
//#define SCREENSHOT_MODE

namespace Gamebuino_Meta
{
  extern volatile uint32_t dma_desc_free_count;
}

namespace roads
{
#ifdef DEBUG_PERF
  int minCount = 32000;
  int totalCount = 0;
#endif

#ifdef SCREENSHOT_MODE
uint16_t counter = 0;
File* wfile;
#endif

      static inline void WaitForDescAvailable(const uint32_t min_desc_num)
      {
#ifdef DEBUG_PERF
        int count = 0;
        while (Gamebuino_Meta::dma_desc_free_count < min_desc_num){
          ++count;
        }
        if(count < minCount)
        {
          minCount = count;
        }
        totalCount += count;
#else
        while (Gamebuino_Meta::dma_desc_free_count < min_desc_num);
#endif
      }
      
      static inline void WaitForTransfersDone(void)
      {
        WaitForDescAvailable(DMA_DESC_COUNT);
      }
}

GraphicsManager::GraphicsManager(uint16_t* strip1, uint16_t* strip2) noexcept:
  strip1(strip1),
  strip2(strip2)
{}

uint16_t* GraphicsManager::StartFrame() noexcept
{
#ifdef SCREENSHOT_MODE
  if(counter == 0)
  {
    wfile = new File;
    *wfile = SD.open("foobar", FILE_WRITE | O_SYNC);
    SerialUSB.printf("Opened %i\n", wfile->isOpen());
  }
#endif
    gb.tft.setAddrWindow(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    //initiate SPI
    SPI.beginTransaction(Gamebuino_Meta::tftSPISettings);
    gb.tft.dataMode();

    currentStrip = strip1;
    sentStrip = strip2;

    return currentStrip;
}

void GraphicsManager::EndFrame() noexcept
{
  WaitForTransfersDone();
  gb.tft.idleMode();
  SPI.endTransaction();
#ifdef DEBUG_PERF
  SerialUSB.printf("Min Count %i / %i\n", minCount, totalCount);
    roads::minCount = 32000;
    roads::totalCount = 0;
#endif

#ifdef SCREENSHOT_MODE
  if(wfile)
  {
    wfile->close();
    delete wfile;
    wfile = nullptr;
  }
  ++counter;
  if(counter == 400)
  {
    counter = 0;
  }
#endif
}

uint16_t* GraphicsManager::CommitStrip() noexcept
{
    uint16_t *temp = currentStrip;
    currentStrip = sentStrip;
    sentStrip = temp;

    gb.tft.sendBuffer(sentStrip, STRIP_SIZE_PIX);
#ifdef SCREENSHOT_MODE
    if(wfile)
    {
      int written = wfile->write((uint8_t*)sentStrip, 1/*STRIP_SIZE_BYTES*/);
      SerialUSB.printf("written %i\n", written);
    }
#endif
    WaitForDescAvailable(2);

    return currentStrip;
}
