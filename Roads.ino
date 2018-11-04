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

struct DepthInfo
{
  uint16_t z = 0;
  float zf = 0.f;
  float scaleFactor = 0;
  int16_t leftBumperIndex = 0;
  int16_t leftRoadIndex = 0;
  int16_t leftLineIndex = 0;
  int16_t rightRoadIndex = 0;
  int16_t rightBumperIndex = 0;
  int16_t rightGrassIndex = 0;
};

/*uint16_t zMap[SCREEN_HEIGHT];*/
DepthInfo zMap[SCREEN_HEIGHT];
#define SKY_Z (0xFFFF)

uint16_t offset;
int16_t screen_x;
float curvature;
int16_t xMap[SCREEN_HEIGHT];

bool growing;

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

//  float zMin(0), zMax(0);
#define BUMPER_WIDTH (6)
#define LANE_WIDTH (70)
#define LINE_WIDTH (4)

  for(unsigned int row_index = 0; row_index < SCREEN_HEIGHT; ++row_index)
  {
    if(row_index >= Y_E_PIXELS)
    {
      zMap[SCREEN_HEIGHT - 1 - row_index].z = SKY_Z;
    }
    else
    {
      float y = row_index * Y_E_METERS / Y_E_PIXELS;
      DepthInfo& depthInfo = zMap[SCREEN_HEIGHT - 1 - row_index];
      //depthInfo.z = (uint16_t)((-Z_E) / (Y_E_METERS - y) * Y_E_METERS + Z_E + 0.5f);
      depthInfo.zf = ((-Z_E) / (Y_E_METERS - y) * Y_E_METERS + Z_E);
      depthInfo.z = (uint16_t)(depthInfo.zf + 0.5f);

    //We consider that at the horizon, the road is 10 times narrower than at the bottom of the viewport
      depthInfo.scaleFactor = 1.f - (0.9f * (row_index)) / Y_E_PIXELS;

      depthInfo.leftBumperIndex = - (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + LANE_WIDTH + BUMPER_WIDTH + 0.5f));
      depthInfo.leftRoadIndex =   - (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + LANE_WIDTH + 0.5f));
      depthInfo.leftLineIndex =   - (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + 0.5f));
      depthInfo.rightRoadIndex =    (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + 0.5f));
      depthInfo.rightBumperIndex =  (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + LANE_WIDTH + 0.5f));
      depthInfo.rightGrassIndex =   (int16_t)(depthInfo.scaleFactor*((LINE_WIDTH >> 1) + LANE_WIDTH + BUMPER_WIDTH + 0.5f));
  
      if(depthInfo.leftBumperIndex == depthInfo.leftRoadIndex)
      {
        ++depthInfo.leftRoadIndex;
      }

      if(depthInfo.leftLineIndex == depthInfo.rightRoadIndex)
      {
        ++depthInfo.rightRoadIndex;
      }

      if(depthInfo.rightBumperIndex == depthInfo.rightGrassIndex)
      {
        ++depthInfo.rightGrassIndex;
      }
    }
  }



  offset = 1;
  screen_x = SCREEN_WIDTH / 2;

  curvature = -1.f;

  growing = true;
}

void loop()
{
  while (!gb.update());

  if(growing)
  {
    curvature += 0.1f;
    if(curvature > 15.f)
      growing = false;
  }
  else
  {
    curvature -= 0.1f;
    if(curvature < -15.f)
      growing = true;
  }

  // Use the serial monitor to observe the CPU utilization.
  if (gb.frameCount % 100 == 0)
  {
    SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
//    SerialUSB.printf("WHITE: %x\n", COLOR_565(255, 255, 255));
//    SerialUSB.printf("RED: %x\n", COLOR_565(255, 0, 0));
//    SerialUSB.printf("GREEN: %x\n", COLOR_565(0, 255, 0));
//    SerialUSB.printf("BLUE: %x\n", COLOR_565(0, 0, 255));
  }

// Update curvature map
  xMap[SCREEN_HEIGHT - 1] = screen_x;
  float x = xMap[SCREEN_HEIGHT - 1];
  float prevZ = zMap[SCREEN_HEIGHT - 1].zf;
  for(int16_t y = SCREEN_HEIGHT - 2; y >= 0; --y)
  {
    float currZ = zMap[y].zf;
    if(currZ != SKY_Z)
    {
      /*xMap[y]*/x = /*xMap[y+1]*/x + (curvature * (currZ - prevZ) * zMap[y].scaleFactor);
      xMap[y] = x;
      prevZ = currZ;
    }
  }

  
  uint16_t* strip = GraphicsManager::StartFrame();
  uint16_t* stripCursor = strip;

  unsigned int yStrip = 1;
  DepthInfo * currentZ = zMap;

  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip, ++currentZ)
  {
    //SerialUSB.printf("%i %i\n", y, *currentZ);
    if(currentZ->z == SKY_Z)
    {
      // draw sky
      for(uint16_t ii = 0; ii < SCREEN_WIDTH; ++ii)
      {
        (*stripCursor++) = COLOR_565(150, 200, 255);
      }
    }
    else
    {
      int altColor = (((currentZ->z + offset/2) >> 1) & 0x1);
      uint16_t* track_palette = colors_track + (COLOR_TRACK_SIZE+1) * altColor;
      int16_t col = 0;
      for(; col < xMap[y] + currentZ->leftBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_GRASS_INDEX];
      }
      for(; col < xMap[y] + currentZ->leftRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < xMap[y] + currentZ->leftLineIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < xMap[y] + currentZ->rightRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_LINE_INDEX];
      }
      for(; col < xMap[y] + currentZ->rightBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < xMap[y] + currentZ->rightGrassIndex && col < SCREEN_WIDTH; ++col)
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


  GraphicsManager::EndFrame();
  ++offset;

  if (gb.buttons.repeat(BUTTON_LEFT, 0))
  {
    ++screen_x;
  }
  else if (gb.buttons.repeat(BUTTON_RIGHT, 0))
  {
    --screen_x;
  }
}
