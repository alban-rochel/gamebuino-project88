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

#define Z_E (-5.f)
#define Y_E_METERS (5.f)
#define Y_E_PIXELS (80)

struct DepthInfo
{
  uint16_t z = 0;
  float zf = 0.f;               // TODO 8.8
  float scaleFactor = 0;        // TODO 8.8
  int16_t leftBumperIndex = 0;  // TODO int8_t
  int16_t leftRoadIndex = 0;    // TODO int8_t
  int16_t leftLineIndex = 0;    // TODO int8_t
  int16_t rightRoadIndex = 0;   // TODO int8_t
  int16_t rightBumperIndex = 0; // TODO int8_t
  int16_t rightGrassIndex = 0;  // TODO int8_t
};
DepthInfo depthLevels[Y_E_PIXELS];

#define SKY_Z (0xFF)
uint8_t yToDepth[SCREEN_HEIGHT];

uint16_t offset;
float curvature;
float zCurv;
int16_t xAtDepth[Y_E_PIXELS];


bool growing;
bool zGrowing;

struct LevelConfig
{
  uint8_t bumperWidth;
  uint8_t roadWidth;
  uint8_t lineWidth;
};

struct CarInfo
{
  int16_t posX; // 0 is the center of the road
  float posZ;
  float speed;
};

CarInfo carInfo;

void resetDepthInfo(const LevelConfig& config)
{
  for(unsigned int rowIndex = 0; rowIndex < Y_E_PIXELS; ++rowIndex)
  {
      float y = rowIndex * Y_E_METERS / Y_E_PIXELS;
      DepthInfo& depthInfo = depthLevels[rowIndex];
      depthInfo.zf = ((-Z_E) / (Y_E_METERS - y) * Y_E_METERS + Z_E);
      depthInfo.z = (uint16_t)(depthInfo.zf + 0.5f);

    //We consider that at the horizon, the road is 10 times narrower than at the bottom of the viewport
      depthInfo.scaleFactor = 1.f - (0.9f * (rowIndex)) / Y_E_PIXELS;

      depthInfo.leftBumperIndex = - (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + (config.roadWidth >> 1) + config.bumperWidth + 0.5f));
      depthInfo.leftRoadIndex =   - (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + (config.roadWidth >> 1) + 0.5f));
      depthInfo.leftLineIndex =   - (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + 0.5f));
      depthInfo.rightRoadIndex =    (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + 0.5f));
      depthInfo.rightBumperIndex =  (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + (config.roadWidth >> 1) + 0.5f));
      depthInfo.rightGrassIndex =   (int16_t)(depthInfo.scaleFactor*((config.lineWidth >> 1) + (config.roadWidth >> 1) + config.bumperWidth + 0.5f));
  
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

  LevelConfig config;
  config.bumperWidth  = 6;
  config.roadWidth    = 140;
  config.lineWidth    = 4;

  resetDepthInfo(config);

  for(unsigned int rowIndex = 0; rowIndex < SCREEN_HEIGHT; ++rowIndex)
  {
    if(rowIndex >= Y_E_PIXELS)
    {
      yToDepth[SCREEN_HEIGHT - 1 - rowIndex] = SKY_Z;
    }
    else
    {
      yToDepth[SCREEN_HEIGHT - 1 - rowIndex] = rowIndex;
    }
  }

  carInfo.posX = SCREEN_WIDTH / 2;
  carInfo.posZ = 0;
  carInfo.speed = 0.f;

  curvature = -1.f;
  zCurv = 0.f;

  growing = true;
  zGrowing = true;
}

void loop()
{
  while (!gb.update());

  curvature = 15.f * sin(carInfo.posZ / 50.);
  zCurv = 0.3f + sin(carInfo.posZ / 80.)/2.f;

  // Use the serial monitor to observe the CPU utilization.
/*  if (gb.frameCount % 100 == 0)
  {
    SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
  }*/

// Update curvature map

  xAtDepth[0] = carInfo.posX;
  float x = xAtDepth[0];
  float prevZ = depthLevels[0].zf;
  for(int16_t depthLevel = 1; depthLevel < Y_E_PIXELS; ++depthLevel)
  {
    float currZ = depthLevels[depthLevel].zf;
    x = x + (curvature * (currZ - prevZ) * depthLevels[depthLevel].scaleFactor);
    xAtDepth[depthLevel] = x;
    prevZ = currZ;
  }

  {
    float currentZ = 0.f;
    int16_t prevZInt = 0;
    int16_t zInt = 0;
    int16_t y=0;
  
    while(zInt < Y_E_PIXELS && y < SCREEN_HEIGHT && zInt >= prevZInt)
    {
      yToDepth[SCREEN_HEIGHT - 1 - y] = zInt;
      currentZ += 1.f + zCurv;
      prevZInt = zInt;
     zInt = (currentZ + 0.5f);
      ++y;
    }
    while(y < SCREEN_HEIGHT)
    {
      yToDepth[SCREEN_HEIGHT - 1 - y] = SKY_Z;
      ++y;
    }
  }


  
  uint16_t* strip = GraphicsManager::StartFrame();
  uint16_t* stripCursor = strip;

  unsigned int yStrip = 1;
  //DepthInfo * currentZ = zMap;

  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip)
  {
    uint8_t depthLevel = yToDepth[y];
    //SerialUSB.printf("%i %i\n", y, *currentZ);
    if(depthLevel == SKY_Z)
    {
      // draw sky
      for(uint16_t ii = 0; ii < SCREEN_WIDTH; ++ii)
      {
        (*stripCursor++) = COLOR_565(150, 200, 255);
      }
    }
    else
    {
      DepthInfo& di = depthLevels[depthLevel];
      int16_t x = xAtDepth[depthLevel];
      
      int altColor = (((uint16_t)(di.z + carInfo.posZ) >> 1) & 0x1);
      uint16_t* track_palette = colors_track + (COLOR_TRACK_SIZE+1) * altColor;
      int16_t col = 0;
      for(; col < x + di.leftBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_GRASS_INDEX];
      }
      for(; col < x + di.leftRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < x + di.leftLineIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.rightRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_LINE_INDEX];
      }
      for(; col < x + di.rightBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = track_palette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.rightGrassIndex && col < SCREEN_WIDTH; ++col)
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

  if (gb.buttons.repeat(BUTTON_LEFT, 0))
  {
    ++carInfo.posX;
  }
  else if (gb.buttons.repeat(BUTTON_RIGHT, 0))
  {
    --carInfo.posX;
  }

  if (gb.buttons.repeat(BUTTON_A, 0))
  {
    carInfo.speed += 0.005;
  }
  else if(gb.buttons.repeat(BUTTON_B, 0))
  {
    carInfo.speed -= 0.02;
  }
  else
  {
    carInfo.speed -= 0.001;
  }

  if(carInfo.speed <= 0)
  {
    carInfo.speed = 0;
  }
  else if(carInfo.speed > 1.)
  {
    carInfo.speed = 1.;
  }
  

    carInfo.posZ += carInfo.speed;

    if(gb.buttons.repeat(BUTTON_MENU, 0))
    {
      SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
    for(unsigned int ii = 0; ii < Y_E_PIXELS; ++ii)
    {
      SerialUSB.printf("depth: %i %i\n", ii, depthLevels[ii].z);
    }
    }
}
