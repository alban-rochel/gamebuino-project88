#include "GraphicsManager.h"

using namespace roads;

#define MEMORY_SEGMENT_SIZE 8192
uint8_t memory[MEMORY_SEGMENT_SIZE];
void* nextAvailableSegment;

/*void printDirectory(File dir, int numTabs) {
  if(!dir)
    return;
  while (true)
  {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      SerialUSB.printf("\t");
    }
    char name[20];
    entry.getName(name, 20);
    SerialUSB.printf("%s\n", name);
    entry.close();
  }
}*/

inline void* mphAlloc(uint16_t size)
{
  if(nextAvailableSegment + size >= memory + MEMORY_SEGMENT_SIZE)
  {
    return NULL;
  }

  void* res = nextAvailableSegment;
  nextAvailableSegment += size;
  return res;
}

void resetAlloc()
{
  nextAvailableSegment = memory;
}

int32_t allocFreeRam()
{
  return memory + MEMORY_SEGMENT_SIZE - (uint8_t*)nextAvailableSegment;
}

extern SdFat SD;

void displayFile(const char* filename)
{
  resetAlloc();
  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  GraphicsManager gm(strip1, strip2);
  
  bool exists = SD.exists(filename);
  
  if(!exists)
  {
    SerialUSB.printf("Cannot find file %s\n", filename);
    return;
  }

  File f = SD.open(filename, FILE_READ);
  if(!f)
  {
        SerialUSB.printf("Failed opening file %s\n", filename);
    return;
  }

  uint16_t* strip = gm.StartFrame();
  uint16_t* stripCursor = strip;

  for(unsigned int y = 0; y < SCREEN_HEIGHT/8; ++y)
  {
    for(unsigned int line = 0; line < 8; ++line)
    {
      int size = f.read(stripCursor, 160);
      SerialUSB.printf("size1: %i\n", size);
      f.flush();
      stripCursor+=160;
      size = f.read(stripCursor, 160);
      SerialUSB.printf("size2: %i\n", size);
      f.flush();
      stripCursor+=160;
    }
    strip = gm.CommitStrip();
    stripCursor = strip;

  }

   gm.EndFrame();
  f.close();
}

LevelConfig levelSelectionMenu() noexcept
{
  LevelConfig config;
  config.bumperWidth  = 6;
  config.roadWidth    = 140;
  config.lineWidth    = 4;

  /*displayFile("/Roads/brown.mph");

  while(true)
  {
    while (!gb.update());

    if (gb.buttons.repeat(BUTTON_LEFT, 10))
    {
      displayFile("/Roads/brown.mph");
    }
    if (gb.buttons.repeat(BUTTON_RIGHT, 10))
    {
      displayFile("/Roads/brown2.mph");
    }

    if (gb.buttons.pressed(BUTTON_MENU))
    {
      SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
      SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
      SerialUSB.printf("REMAIN: %i\n", allocFreeRam());
    }
    
    if(gb.buttons.pressed(BUTTON_A))
    {
      return config;
    }
  }*/

  return config;
}

void initDepthInfo( const LevelConfig& config,
                    DepthInfo* depthLevels,
                    uint16_t& minSegmentSize,
                    uint16_t& maxSegmentSize) noexcept
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

  minSegmentSize = (depthLevels[Y_E_PIXELS-1].z >> 1) + 1;
  maxSegmentSize = minSegmentSize + (minSegmentSize >> 1);
}

void initPalette(uint16_t& skyColor,
                 uint16_t* trackPalette) noexcept
{
  skyColor = COLOR_565(150, 200, 255);
  trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(93, 130, 37); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX + 1]  = COLOR_565(118, 160, 54);
  trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(178, 32, 32); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX + 1] = COLOR_565(255, 255, 255);
  trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(142, 142, 142); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX + 1] = COLOR_565(186, 186, 186);
  trackPalette[COLOR_TRACK_LINE_INDEX]   = COLOR_565(255, 255, 255); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX + 1] = trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX + 1];
}

void gameLoop(const LevelConfig& config) noexcept
{
  resetAlloc();

  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  GraphicsManager gm(strip1, strip2);
  
  // declare all the arrays
//  DepthInfo depthLevels[Y_E_PIXELS];
  DepthInfo* depthLevels = (DepthInfo*) mphAlloc(Y_E_PIXELS * sizeof(DepthInfo));
  //uint8_t yToDepth[SCREEN_HEIGHT];
  uint8_t* yToDepth = (uint8_t*) mphAlloc(SCREEN_HEIGHT);
  //int16_t xAtDepth[Y_E_PIXELS];
  int16_t* xAtDepth = (int16_t*) mphAlloc(Y_E_PIXELS * sizeof(int16_t));
  
  RoadSegment* segments = (RoadSegment*) mphAlloc(3 * sizeof(RoadSegment));
  uint16_t minSegmentSize;
  uint16_t maxSegmentSize;

  CarInfo carInfo;
  carInfo.posX = 0; //SCREEN_WIDTH / 2;
  carInfo.posZ = 0;
  carInfo.speed = 0.f;

  uint16_t skyColor;
  //uint16_t trackPalette[2*COLOR_TRACK_SIZE];
  uint16_t* trackPalette = (uint16_t*) mphAlloc(2 * COLOR_TRACK_SIZE * sizeof(uint16_t));

  initDepthInfo(config, depthLevels, minSegmentSize, maxSegmentSize);
  SerialUSB.printf("SEGMENTS: %i %i\n", minSegmentSize, maxSegmentSize);
  SerialUSB.printf("depthLevels: %i %i\n", depthLevels[0].leftBumperIndex, depthLevels[50].z);
  initPalette(skyColor, trackPalette);

  //float zCurv = 0.f;

  segments[0].xCurvature = random(-50, 50)/1000.f;
  segments[0].zCurvature = random(-100, 300)/1000.f;
  segments[0].segmentStartZ = 0;

  segments[1].xCurvature = random(-50, 50)/1000.f;
  segments[1].zCurvature = random(-100, 300)/1000.f;
  segments[1].segmentStartZ = 400;//segments[0].segmentStartZ + random(minSegmentSize, maxSegmentSize);

  segments[2].xCurvature = random(-50, 50)/1000.f;
  segments[2].zCurvature = random(-100, 300)/1000.f;
  segments[2].segmentStartZ = segments[0].segmentStartZ + random(minSegmentSize, maxSegmentSize);

  // Actual game loop

  while(true)
  { 
    while (!gb.update());
// Update curvature map

  xAtDepth[0] = SCREEN_WIDTH / 2 + carInfo.posX;
  float x = xAtDepth[0];
  float prevZ = depthLevels[0].zf;
  //centering = SCREEN_WIDTH / 2 + (carInfo.posX);
 float totalOffset = 0;
  for(int16_t depthLevel = 1; depthLevel < Y_E_PIXELS; ++depthLevel)
  {
    float currZ = depthLevels[depthLevel].zf;
    if(currZ + carInfo.posZ < segments[1].segmentStartZ)
    {
      totalOffset += segments[0].xCurvature * (currZ - prevZ)*100;
    }
    else
    {
      if(currZ + carInfo.posZ < segments[2].segmentStartZ)
      {
        totalOffset += segments[1].xCurvature * (currZ - prevZ)*100;
      }
      else
      {
        totalOffset += segments[2].xCurvature * (currZ - prevZ)*100;
      }
    }
          //x = x + (currCurvature * (currZ - prevZ) * depthLevels[depthLevel].scaleFactor);
          x = SCREEN_WIDTH / 2 + (carInfo.posX + totalOffset) * depthLevels[depthLevel].scaleFactor;
    xAtDepth[depthLevel] = x;
    prevZ = currZ;
  }

  {

    float currZIndex = 0.f;
    int16_t zIndex = 0;
    int16_t y=0;
    totalOffset = 0;
    float totalCurvature = 0.f;
    prevZ = 0.f;

    while(zIndex < Y_E_PIXELS && y < SCREEN_HEIGHT && zIndex >= 0)
    {
      yToDepth[SCREEN_HEIGHT - 1 - y] = zIndex;

      const DepthInfo& di = depthLevels[zIndex];
      
      if(di.zf + carInfo.posZ < segments[1].segmentStartZ)
      {
        totalOffset += segments[0].zCurvature * di.scaleFactor;
      }
      else
      {
        if(di.zf + carInfo.posZ < segments[2].segmentStartZ)
        {
          totalOffset += segments[1].zCurvature * di.scaleFactor;
        }
        else
        {
          totalOffset += segments[2].zCurvature * di.scaleFactor;
        }
      }


      //totalOffset += totalCurvature;
      
      prevZ = depthLevels[zIndex].zf;
      currZIndex = y + totalOffset;
      zIndex = currZIndex + 0.5f;
      ++y;
    }
    
    while(y < SCREEN_HEIGHT)
    {
      yToDepth[SCREEN_HEIGHT - 1 - y] = SKY_Z;
      ++y;
    }
  }


  
  uint16_t* strip = gm.StartFrame();
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
      
      int altColor = (((uint16_t)(di.z + carInfo.posZ) >> 2) & 0x1);
      uint16_t* currentPalette = trackPalette + (COLOR_TRACK_SIZE+1) * altColor;
      int16_t col = 0;
      for(; col < x + di.leftBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_GRASS_INDEX];
      }
      for(; col < x + di.leftRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < x + di.leftLineIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.rightRoadIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_LINE_INDEX];
      }
      for(; col < x + di.rightBumperIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.rightGrassIndex && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_GRASS_INDEX];
      }
    }

    if(yStrip == STRIP_HEIGHT)
    {
      strip = gm.CommitStrip();
      stripCursor = strip;
      yStrip = 0;
    }
  }


  gm.EndFrame();

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

    if(carInfo.posZ > segments[1].segmentStartZ)
    {
      segments[0] = segments[1];
      segments[1] = segments[2];
      segments[2].xCurvature = random(-50, 50)/1000.f;
      segments[2].zCurvature = random(-100, 300)/1000.f;
      segments[2].segmentStartZ = segments[0].segmentStartZ + random(minSegmentSize, maxSegmentSize);
    }

    if(gb.buttons.repeat(BUTTON_MENU, 0))
    {
      SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
    SerialUSB.printf("REMAIN: %i\n", allocFreeRam());
//    for(unsigned int ii = 0; ii < Y_E_PIXELS; ++ii)
//    {
//      SerialUSB.printf("depth: %i %i\n", ii, depthLevels[ii].z);
//    }

/*File root;
root = SD.open("/Roads");

  printDirectory(root, 0);*/
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

  nextAvailableSegment = memory;
}

void loop()
{
  LevelConfig config = levelSelectionMenu();

  gameLoop(config);
}
