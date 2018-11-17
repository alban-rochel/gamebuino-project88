#include "GraphicsManager.h"

#include "CarSprites.h"

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
/*
  displayFile("/Roads/brown.mph");

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
  }
*/
  return config;
}

void initDepthInfo( const LevelConfig& config,
                    DepthInfo* depthLevels,
                    Z_POSITION& minSegmentSize,
                    Z_POSITION& maxSegmentSize) noexcept
{
  SCALE_FACTOR scaleFactor;
  int32_t offset;
  for(unsigned int rowIndex = 0; rowIndex < DEPTH_LEVEL_COUNT; ++rowIndex)
  {
      float y = rowIndex * Y_E_METERS / DEPTH_LEVEL_COUNT;
      DepthInfo& depthInfo = depthLevels[rowIndex];
      depthInfo.z = (Z_POSITION)(((-Z_E) / (Y_E_METERS - y) * Y_E_METERS + Z_E) * (1 << Z_POSITION_SHIFT));

    //We consider that at the horizon, the road is 10 times narrower than at the bottom of the viewport
      depthInfo.scaleFactor = 1.f - (0.9f * (rowIndex)) / DEPTH_LEVEL_COUNT;

      scaleFactor = *(ScaleFactor + rowIndex);

      offset = config.lineWidth >> 1;
      depthInfo.lineWidth = (int16_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);
      offset += (config.roadWidth) >> 1;
      depthInfo.lineRoadWidth = (int16_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);
      offset += config.bumperWidth;
      depthInfo.lineRoadBumperWidth = (int16_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);

      if(depthInfo.lineWidth == 0)
      {
        depthInfo.lineWidth = 1;
      }
      
      if(depthInfo.lineWidth == depthInfo.lineRoadWidth)
      {
        ++depthInfo.lineWidth;
      }

      if(depthInfo.lineRoadWidth == depthInfo.lineRoadBumperWidth)
      {
        ++depthInfo.lineRoadBumperWidth;
      }
  }

  minSegmentSize = (depthLevels[DEPTH_LEVEL_COUNT-1].z >> 1) + 1;
  maxSegmentSize = minSegmentSize + (minSegmentSize >> 1);
}

void initPalette(uint16_t& skyColor,
                 uint16_t* trackPalette) noexcept
{
  skyColor = COLOR_565(150, 200, 255);
  trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(93, 130, 37); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX]  = COLOR_565(118, 160, 54);
  trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(178, 32, 32); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX] = COLOR_565(255, 255, 255);
  trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(142, 142, 142); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX] = COLOR_565(186, 186, 186);
  trackPalette[COLOR_TRACK_LINE_INDEX]   = COLOR_565(255, 255, 255); trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX] = trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX];
}

void computeTurns(CarInfo& carInfo, DepthInfo* depthLevels, int16_t* depthLevelToX, RoadSegment* segments) noexcept
{
  depthLevelToX[0] = SCREEN_WIDTH / 2 + carInfo.posX;
  float x = depthLevelToX[0];
  Z_POSITION previousZ = depthLevels[0].z;
  Z_POSITION currentZ = previousZ;
  Z_POSITION absoluteZ = 0;
  int32_t deltaZ = 0;
  //centering = SCREEN_WIDTH / 2 + (carInfo.posX);
  int32_t totalOffset = 0;
  for(int16_t depthLevel = 1; depthLevel < DEPTH_LEVEL_COUNT; ++depthLevel)
  {
    currentZ = depthLevels[depthLevel].z;
    deltaZ = (currentZ - previousZ);
    absoluteZ = currentZ + carInfo.posZ;
    //float dzf = deltaZ / 256.f;
    
    if(absoluteZ < segments[1].segmentStartZ)
    {
      totalOffset += (segments[0].xCurvature * deltaZ);
    }
    else
    {
      if(absoluteZ < segments[2].segmentStartZ)
      {
        totalOffset += (segments[1].xCurvature * deltaZ);
      }
      else
      {
        totalOffset += (segments[2].xCurvature * deltaZ);
      }
    }

    x = SCREEN_WIDTH / 2 + (carInfo.posX + (totalOffset >> (ROAD_CURVATURE_X_SHIFT + Z_POSITION_SHIFT))) * depthLevels[depthLevel].scaleFactor;
    depthLevelToX[depthLevel] = x;
    previousZ = currentZ;
  }
}

void computeHills(CarInfo& carInfo, DepthInfo* depthLevels, RoadSegment* segments, uint8_t* lineToDepthLevel) noexcept
{
  float prevZ = 0.f;
  float totalOffset = 0.f;

    float currZIndex = 0.f;
    int16_t zIndex = 0;
    int16_t y=0;
    totalOffset = 0;
    float totalCurvature = 0.f;

    //uint16_t scaleFactor0_16;

    while(zIndex < DEPTH_LEVEL_COUNT && y < SCREEN_HEIGHT && zIndex >= 0)
    {
      lineToDepthLevel[SCREEN_HEIGHT - 1 - y] = zIndex;

      const DepthInfo& di = depthLevels[zIndex];
      
      if(di.z + carInfo.posZ < segments[1].segmentStartZ)
      {
        totalOffset += segments[0].zCurvature * di.scaleFactor;
      }
      else
      {
        if(di.z + carInfo.posZ < segments[2].segmentStartZ)
        {
          totalOffset += segments[1].zCurvature * di.scaleFactor;
        }
        else
        {
          totalOffset += segments[2].zCurvature * di.scaleFactor;
        }
      }


      //totalOffset += totalCurvature;
      
      //prevZ = depthLevels[zIndex].zf;
      currZIndex = y + totalOffset;
      zIndex = currZIndex + 0.5f;
      ++y;
    }
    
    while(y < SCREEN_HEIGHT)
    {
      lineToDepthLevel[SCREEN_HEIGHT - 1 - y] = SKY_Z;
      ++y;
    }
}

void gameLoop(const LevelConfig& config) noexcept
{
  resetAlloc();
  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  DepthInfo* depthLevels = (DepthInfo*) mphAlloc(DEPTH_LEVEL_COUNT * sizeof(DepthInfo));
  uint8_t* lineToDepthLevel = (uint8_t*) mphAlloc(SCREEN_HEIGHT);
  int16_t* depthLevelToX = (int16_t*) mphAlloc(DEPTH_LEVEL_COUNT * sizeof(int16_t));
  uint16_t* trackPalette = (uint16_t*) mphAlloc(2 * COLOR_TRACK_SIZE * sizeof(uint16_t));
  RoadSegment* segments = (RoadSegment*) mphAlloc(3 * sizeof(RoadSegment));
  //SpriteProgram* sprites = (SpriteProgram*) mphAlloc(MAX_SPRITES * sizeof(SpriteProgram));
  SpriteProgram sprites[MAX_SPRITES];
      
  bool left = false;
  bool right = false;
  Z_POSITION zCactus = (100 << Z_POSITION_SHIFT);

  GraphicsManager gm(strip1, strip2);
  
  Z_POSITION minSegmentSize;
  Z_POSITION maxSegmentSize;

  CarInfo carInfo;
  carInfo.posX = 0; //SCREEN_WIDTH / 2;
  carInfo.posZ = 0;
  carInfo.speed = 0.f;

  uint16_t skyColor;
  //uint16_t trackPalette[2*COLOR_TRACK_SIZE];

  initDepthInfo(config, depthLevels, minSegmentSize, maxSegmentSize);
  initPalette(skyColor, trackPalette);

  
  uint8_t spriteCount(0);

  segments[0].xCurvature = random(-(5 << ROAD_CURVATURE_X_SHIFT), (5 << ROAD_CURVATURE_X_SHIFT));
  segments[0].zCurvature = random(-150, 400)/1000.f;
  segments[0].segmentStartZ = 0;

  segments[1].xCurvature = random(-(5 << ROAD_CURVATURE_X_SHIFT), (5 << ROAD_CURVATURE_X_SHIFT));
  segments[1].zCurvature = random(-150, 400)/1000.f;
  segments[1].segmentStartZ = 400 * (1 << Z_POSITION_SHIFT);//segments[0].segmentStartZ + random(minSegmentSize, maxSegmentSize);

  segments[2].xCurvature = random(-(5 << ROAD_CURVATURE_X_SHIFT), (5 << ROAD_CURVATURE_X_SHIFT));
  segments[2].zCurvature = random(-150, 400)/1000.f;
  segments[2].segmentStartZ = segments[1].segmentStartZ + random(minSegmentSize, maxSegmentSize);

  // Actual game loop

  while(true)
  { 
    while (!gb.update());

    spriteCount = 0;
    
    computeTurns(carInfo, depthLevels, depthLevelToX, segments);

    computeHills(carInfo, depthLevels, segments, lineToDepthLevel);
 

uint8_t yCactus = -1;
  // position cactus
  {
    Z_POSITION prevZ = Z_POSITION_MAX;
    for(unsigned int y = 0; y < SCREEN_HEIGHT && yCactus == (uint8_t)-1; ++y)
    {//SerialUSB.printf("%i zCactus %i depthLevels[lineToDepthLevel[y]].z%i \n", y, zCactus, depthLevels[lineToDepthLevel[y]].z);
      if(lineToDepthLevel[y] != SKY_Z)
      {
        
        if(depthLevels[lineToDepthLevel[y]].z+carInfo.posZ <= zCactus && prevZ > zCactus)
        {
          yCactus = y;
        }
        prevZ = depthLevels[lineToDepthLevel[y]].z+carInfo.posZ;
      }
    }
  }

if(yCactus != -1)
{
  SpriteProgram& sprite = sprites[spriteCount];
  
  SCALE_FACTOR scale = ScaleFactor[lineToDepthLevel[yCactus]];

  if(scale <= SCALE_FACTOR_QUARTER)
  {
    sprite.zoomPattern = 4;
  }
  else if(scale <= SCALE_FACTOR_HALF)
  {
    sprite.zoomPattern = 2;
  }
  else
  {
    sprite.zoomPattern = 1;
  }
    
  sprite.xStart = depthLevelToX[lineToDepthLevel[yCactus]] - ((scale * config.roadWidth) >> SCALE_FACTOR_SHIFT);
  sprite.width = CACTUS_WIDTH;
  
  uint16_t actualHeight = CACTUS_HEIGHT / sprite.zoomPattern;
  sprite.yStart = (yCactus >= actualHeight) ? yCactus - actualHeight : 0;
  sprite.yEnd = sprite.yStart + actualHeight - 1;
  
  sprite.buffer = CACTUS;
  ++spriteCount;
}

if(left)
{
  SpriteProgram& sprite = sprites[spriteCount];
  sprite.xStart = SCREEN_WIDTH/2 - CAR_LEFT_WIDTH/2;
  sprite.yStart = 120 - CAR_LEFT_HEIGHT;
  sprite.width = CAR_LEFT_WIDTH;
  sprite.yEnd = sprite.yStart + CAR_LEFT_HEIGHT - 1;
  sprite.buffer = CAR_LEFT;
  sprite.zoomPattern = 1;
}
else if(right)
{
  SpriteProgram& sprite = sprites[spriteCount];
  sprite.xStart = SCREEN_WIDTH/2 - CAR_RIGHT_WIDTH/2;
  sprite.yStart = 120 - CAR_RIGHT_HEIGHT;
  sprite.width = CAR_RIGHT_WIDTH;
  sprite.yEnd = sprite.yStart + CAR_RIGHT_HEIGHT - 1;
  sprite.buffer = CAR_RIGHT;
  sprite.zoomPattern = 1;
}
else
{
    SpriteProgram& sprite = sprites[spriteCount];
    sprite.xStart = SCREEN_WIDTH/2 - CAR_WIDTH/2;
  sprite.yStart = 120 - CAR_HEIGHT;
  sprite.width = CAR_WIDTH;
  sprite.yEnd = sprite.yStart + CAR_HEIGHT - 1;
  sprite.buffer = CAR;
  sprite.zoomPattern = 1;
}
  ++spriteCount;

  
  
  uint16_t* strip = gm.StartFrame();
  uint16_t* stripLine;
  uint16_t* stripCursor = strip;

  unsigned int yStrip = 1;
  //DepthInfo * currentZ = zMap;

  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip)
  {
    stripLine = stripCursor;
    uint8_t depthLevel = lineToDepthLevel[y];
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
      int16_t x = depthLevelToX[depthLevel];
      
      int altColor = (((uint16_t)(di.z + carInfo.posZ) >> (Z_POSITION_SHIFT + 2)) & 0x1);
      uint16_t* currentPalette = trackPalette + (COLOR_TRACK_SIZE) * altColor;
      int16_t col = 0;
      for(; col < x - di.lineRoadBumperWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_GRASS_INDEX];
      }
      for(; col < x - di.lineRoadWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < x - di.lineWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.lineWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_LINE_INDEX];
      }
      for(; col < x + di.lineRoadWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_ROAD_INDEX];
      }
      for(; col < x + di.lineRoadBumperWidth && col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_BUMPER_INDEX];
      }
      for(; col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = currentPalette[COLOR_TRACK_GRASS_INDEX];
      }
    }

    for(unsigned int spriteIndex = 0; spriteIndex < spriteCount; ++spriteIndex)
    {
      const SpriteProgram& sprite = sprites[spriteIndex];
      if(y >= sprite.yStart && y <= sprite.yEnd)
      {
        uint16_t offset = sprite.width * (y - sprite.yStart) * sprite.zoomPattern;
        int16_t xIndex = sprite.xStart;
        for(int16_t x = 0; x < sprite.width && xIndex <= SCREEN_WIDTH-1; x+=sprite.zoomPattern, ++xIndex)
        {
          uint16_t color = *(sprite.buffer + offset + x);
          if(xIndex > 0 && color != COLOR_565(0xFF, 0x00, 0xFF))
          {
            stripLine[xIndex] = color;
          }
        }
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

left = false;
right = false;
  if (gb.buttons.repeat(BUTTON_LEFT, 0))
  {
    ++carInfo.posX;
    left = true;
  }
  else if (gb.buttons.repeat(BUTTON_RIGHT, 0))
  {
    --carInfo.posX;
    right = true;
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
  

    carInfo.posZ += carInfo.speed * 256;

    if(carInfo.posZ > segments[1].segmentStartZ)
    {
      segments[0] = segments[1];
      segments[1] = segments[2];
      segments[2].xCurvature = random(-(5 << ROAD_CURVATURE_X_SHIFT), (5 << ROAD_CURVATURE_X_SHIFT));
      segments[2].zCurvature = random(-150, 400)/1000.f;
      segments[2].segmentStartZ = segments[1].segmentStartZ + random(minSegmentSize, maxSegmentSize);
    }

if(carInfo.posZ > zCactus)
{
  zCactus += (400 << Z_POSITION_SHIFT);
}

    if(gb.buttons.repeat(BUTTON_MENU, 0))
    {
      SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
    SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
    SerialUSB.printf("REMAIN: %i\n", allocFreeRam());
/*    SerialUSB.printf("Coin coin: %i\n", memory);
    for(unsigned int ii = 0; ii < DEPTH_LEVEL_COUNT; ++ii)
    {
      SerialUSB.printf("depth: %i %i\n", ii, (int32_t)(depthLevels[ii].scaleFactor * 16384));
    }*/
    /*for(unsigned int ii = 0; ii < 2 * COLOR_TRACK_SIZE; ++ii)
    {
      SerialUSB.printf("palette: %i %i\n", ii, trackPalette[ii]);
    }*/

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
