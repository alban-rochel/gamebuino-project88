#include "GraphicsManager.h"

using namespace roads;

uint16_t colors[16];

void setup()
{
  gb.begin();
  // We aren't using the normal screen buffer, so initialize it to 0px × 0px.
  gb.display.init(0, 0, ColorMode::rgb565);

  // Just to push things to the limit for this example, increase to 40fps.
  gb.setFrameRate(40);

  SerialUSB.begin(9600);

  colors[0] = colors[7] = 0xFFFF;
  colors[1] = colors[8] = 0x0000;
  colors[2] = colors[9] = 0xFF00;
  colors[3] = colors[10] = 0x00FF;
  colors[5] = colors[11] = 0xF000;
  colors[6] = colors[12] = 0x000F;
  colors[13] = colors[15] = 0x0000;
  colors[14] = 0xFFFF;
}

void loop()
{
  while (!gb.update());

  // Use the serial monitor to observe the CPU utilization.
  if (gb.frameCount % 100 == 0)
  {
    SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
  }
  uint16_t* strip = GraphicsManager::StartFrame();

  for (int sliceIndex = 0; sliceIndex < SCREEN_HEIGHT / STRIP_HEIGHT; sliceIndex++)
  {
    for(uint16_t ii = 0; ii < STRIP_SIZE_PIX; ++ii)
    {
      strip[ii] = colors[sliceIndex];
    }

    strip = GraphicsManager::CommitStrip();
  }

  GraphicsManager::EndFrame();
}
