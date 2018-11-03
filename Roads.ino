#include "GraphicsManager.h"

using namespace roads;

uint16_t color_sky;
#define COLOR_TRACK_GRASS_INDEX 0
#define COLOR_TRACK_BUMPER_INDEX (COLOR_TRACK_GRASS_INDEX+1)
#define COLOR_TRACK_ROAD_INDEX (COLOR_TRACK_BUMPER_INDEX+1)
#define COLOR_TRACK_LINE_INDEX (COLOR_TRACK_ROAD_INDEX+1)
#define COLOR_TRACK_SIZE (COLOR_TRACK_LINE_INDEX)
uint16_t colors_track[2*COLOR_TRACK_SIZE];

/*
 * Perspective computation: 
 * - Unit: meter
 * - The viewport is located at depth z = 0, and is perpendicular to the ground. The bottom row has altitude y = 0.
 * - The eye of the viewer is located at z = ze (negative), height ye = 5 (5 meters above ground looks like a nice point of view for a racing game). Let's say that 5 meters is 80 pixels (horizon slightly above the middle of the screen)
 * - So, at row ye, we see the horizon (z = inf)
 * - At row 0, the viewport touches the ground.
 * - Let's find the z value for the point showed at row y :
 * - Let alpha the angle between a horizontal line and the line from the eye to the row
 * - [1] tan(alpha) = (ye - y) / (-ze)
 * - the line touches the ground at z
 * - [2] tan(alpha) = ye / (z - ze)
 * - Combining both equations:
 * - (ye - y) / (-ze) = ye / (z - ze)
 * -> (z - ze) = (-ze) / (ye - y) * ye
 * -> z = (-ze) / (ye - y) * ye + ze
 */

#define Z_E (-1.f)
#define Y_E_METERS (5.f)
#define Y_E_PIXELS (80)

uint16_t zMap[SCREEN_HEIGHT];
#define SKY_Z (0xFFFF)

uint16_t offset;

void setup()
{
  gb.begin();
  // We aren't using the normal screen buffer, so initialize it to 0px × 0px.
  gb.display.init(0, 0, ColorMode::rgb565);

  // Just to push things to the limit for this example, increase to 40fps.
  gb.setFrameRate(40);

  SerialUSB.begin(9600);

  color_sky = COLOR_565(150, 200, 255);
  colors_track[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(93, 130, 37); colors_track[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX + 1]  = COLOR_565(118, 160, 54);
  colors_track[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(178, 32, 32); colors_track[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX + 1] = COLOR_565(255, 255, 255);
  colors_track[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(142, 142, 142); colors_track[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX + 1] = COLOR_565(186, 186, 186);
  colors_track[COLOR_TRACK_LINE_INDEX]   = COLOR_565(255, 255, 255); colors_track[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX + 1] = colors_track[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX + 1];

  for(unsigned int row_index = 0; row_index < SCREEN_HEIGHT; ++row_index)
  {
    if(row_index >= Y_E_PIXELS)
    {
      zMap[SCREEN_HEIGHT - 1 - row_index] = SKY_Z;
    }
    else
    {
      float y = row_index * Y_E_METERS / Y_E_PIXELS;
      zMap[SCREEN_HEIGHT - 1 - row_index] = (uint16_t)((-Z_E) / (Y_E_METERS - y) * Y_E_METERS + Z_E + 0.5f);
    }
  }

  offset = 1;
}

void loop()
{
  while (!gb.update());

  // Use the serial monitor to observe the CPU utilization.
  if (gb.frameCount % 100 == 0)
  {
    SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
    SerialUSB.printf("WHITE: %x\n", COLOR_565(255, 255, 255));
    SerialUSB.printf("RED: %x\n", COLOR_565(255, 0, 0));
    SerialUSB.printf("GREEN: %x\n", COLOR_565(0, 255, 0));
    SerialUSB.printf("BLUE: %x\n", COLOR_565(0, 0, 255));
  }
  uint16_t* strip = GraphicsManager::StartFrame();
  uint16_t* stripCursor = strip;

#if 0
    for(unsigned int yIndex = 0; yIndex < SCREEN_HEIGHT; ++yIndex)
  {
    if(yIndex < Y_FAR)
    {
      zMap[SCREEN_HEIGHT - yIndex] /* bottom-up */ = Z_NEAR + (uint16_t)(yIndex * INV_COS_ALPHA + 0.5);
      SerialUSB.printf("ROAD %i %i\n", SCREEN_HEIGHT - yIndex, zMap[SCREEN_HEIGHT - yIndex]);
    }
    else
    {
      zMap[SCREEN_HEIGHT - yIndex] /* bottom-up */ = SKY_Z;
      SerialUSB.printf("SKY %i %i\n", SCREEN_HEIGHT - yIndex, zMap[SCREEN_HEIGHT - yIndex]);
    }
  }
#endif

  unsigned int yStrip = 1;
  uint16_t * currentZ = zMap;
  int16_t bumperIndex1 = 50;
  int16_t roadIndex1 = 55;
  int16_t lineIndex = 78;
  int16_t roadIndex2 = 84;
  int16_t bumperIndex2 = 107;
  int16_t grassIndex = 112;
  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip, ++currentZ)
  {
    //SerialUSB.printf("%i %i\n", y, *currentZ);
    if(*currentZ == SKY_Z)
    {
      // draw sky
      for(uint16_t ii = 0; ii < SCREEN_WIDTH; ++ii)
      {
        (*stripCursor++) = COLOR_565(150, 200, 255);
      }
    }
    else
    {
      int altColor = (((*currentZ + offset/2) >> 1) & 0x1);
      uint16_t* track_palette = colors_track + (COLOR_TRACK_SIZE+1) * altColor;
      int16_t col = 0;
      for(; col < bumperIndex1; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_GRASS_INDEX];
      }
      for(; col < roadIndex1; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < lineIndex; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < roadIndex2; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_LINE_INDEX];
      }
      for(; col < bumperIndex2; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < grassIndex; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_GRASS_INDEX];
      }
    }

    if(yStrip == STRIP_HEIGHT)
    {
      strip = GraphicsManager::CommitStrip();
      stripCursor = strip;
      yStrip = 0;
    }
  }

/*
  for (int sliceIndex = 0; sliceIndex < SCREEN_HEIGHT / STRIP_HEIGHT; sliceIndex++)
  {
    for(uint16_t ii = 0; ii < STRIP_SIZE_PIX; ++ii)
    {
      strip[ii] = colors[sliceIndex];
    }

    strip = GraphicsManager::CommitStrip();
  }*/
  GraphicsManager::EndFrame();
  ++offset;
}
