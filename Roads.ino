#include "GraphicsManager.h"

#include "CarSprites.h"
#include "BackgroundWest.h"
#include "BackgroundSuburb.h"
#include "BackgroundSkyline.h"
#include "Title.h"
#include "Dashboard.h"
#include "SoundEffects.h"

using namespace roads;

#define MEMORY_SEGMENT_SIZE 8192
uint8_t memory[MEMORY_SEGMENT_SIZE];
void* nextAvailableSegment;

int16_t capacitorCharge;
Z_POSITION remainingFuel;

const Gamebuino_Meta::Sound_FX* currentFx;

float accelFromSpeed(float speed)
{
  // max speed : MAX_SPEED_Z

  if(speed < 0.5)
  {
    return 0.01;
  }
  else if(speed < 1.)
  {
    return 0.005;
  }

  return 0.002;
  
}

void printDirectory(File dir, int numTabs) {
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
}

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

SdFile rdfile(filename, O_READ);

  // check for open error
  if (!rdfile.isOpen())
  {
    SerialUSB.printf("Failed opening file %s\n", filename);
    return;
  }

SerialUSB.printf("Size %i\n", rdfile.available());

  uint16_t* strip = gm.StartFrame();
  uint16_t* stripCursor = strip;

  for(unsigned int y = 0; y < 2/*SCREEN_HEIGHT/8*/; ++y)
  {
    for(unsigned int index = 0; index < STRIP_SIZE_BYTES; ++index)
    {
      stripCursor[index] = rdfile.read();
    }
    strip = gm.CommitStrip();
    stripCursor = strip;

  }

   gm.EndFrame();
   
  /*bool exists = SD.exists(filename);
  
  if(!exists)
  {
    SerialUSB.printf("Cannot find file %s\n", filename);
    {
      SerialUSB.printf("Content of /Roads\n");
      File root = SD.open("/Roads");
      printDirectory(root, 0);
      root.close();
    }
    {
      SerialUSB.printf("Content of /\n");
      File root = SD.open("/");
      printDirectory(root, 0);
      root.close();
    }
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
  f.close();*/
}

LevelConfig levelSelectionMenu(Level level) noexcept
{
  LevelConfig config;
  config.level = level;

  if(level == Level::Arizona)
  {
    config.xCurvature = 5;
    config.zCurvatureMin = -150;
    config.zCurvatureMax = 400;
    config.bumperWidth  = 6;
    config.roadWidth    = 200;
    config.lineWidth    = 4;
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = 0;
    config.movingObstaclesIndexStart = 0;
    config.movingObstaclesIndexEnd = 0;
  }
  else if(level == Level::Suburb)
  {
    config.xCurvature = 5;
    config.zCurvatureMin = -150;
    config.zCurvatureMax = 400;
    config.bumperWidth  = 8;
    config.roadWidth    = 180;
    config.lineWidth    = 4;
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = MAX_MOVING_OBSTACLES;
    config.movingObstaclesIndexStart = 3;
    config.movingObstaclesIndexEnd = 4;
  }
  else // Skyway
  {
    config.xCurvature = 8;
    config.zCurvatureMin = -200;
    config.zCurvatureMax = 600;
    config.bumperWidth  = 8;
    config.roadWidth    = 160;
    config.lineWidth    = 8;
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = MAX_MOVING_OBSTACLES;
    config.movingObstaclesIndexStart = 3;
    config.movingObstaclesIndexEnd = 4;
  }


  return config;
}

void initDepthInfo( const LevelConfig& config,
                    LevelContext& context,
                    Z_POSITION& minSegmentSize,
                    Z_POSITION& maxSegmentSize) noexcept
{
  SCALE_FACTOR scaleFactor;
  int32_t offset;
  for(unsigned int rowIndex = 0; rowIndex < DEPTH_LEVEL_COUNT; ++rowIndex)
  {
      float y = rowIndex * Y_E_METERS / DEPTH_LEVEL_COUNT;
      DepthInfo& depthInfo = context.depthLevels[rowIndex];
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

  minSegmentSize = (context.depthLevels[DEPTH_LEVEL_COUNT-1].z >> 1) + 1;
  maxSegmentSize = minSegmentSize + (minSegmentSize >> 1);
}

void initPalette(Level level, LevelContext& context) noexcept
{
  if(level == Level::Arizona)
  {
    context.trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(147, 52, 28); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX]  = COLOR_565(187, 126, 83);
    context.trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(0x83, 0x2e, 0x19); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX] = COLOR_565(0xa7, 0x71, 0x4a);
    context.trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(0xc8, 0xac, 0x98); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX] = COLOR_565(0xb6, 0x89, 0x7e);
    context.trackPalette[COLOR_TRACK_LINE_INDEX]   = context.trackPalette[COLOR_TRACK_ROAD_INDEX]; context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX] = context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX];  
  }
  else if(level == Level::Suburb)
  {
    context.trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(93, 130, 37); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX]  = COLOR_565(118, 160, 54);
    context.trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(100, 100, 100); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX] = context.trackPalette[COLOR_TRACK_BUMPER_INDEX];
    context.trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(142, 142, 142); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX] = COLOR_565(186, 186, 186);
    context.trackPalette[COLOR_TRACK_LINE_INDEX]   = COLOR_565(255, 255, 255); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX] = context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX];
  }
  else // Skyway
  {
    
  }
}

void computeTurns(CarInfo& carInfo, LevelContext& context) noexcept
{
  context.depthLevelToX[0] = SCREEN_WIDTH / 2 + carInfo.posX;
  float x = context.depthLevelToX[0];
  Z_POSITION previousZ = context.depthLevels[0].z;
  Z_POSITION currentZ = previousZ;
  Z_POSITION absoluteZ = 0;
  int32_t deltaZ = 0;
  //centering = SCREEN_WIDTH / 2 + (carInfo.posX);
  int32_t totalOffset = 0;
  for(int16_t depthLevel = 1; depthLevel < DEPTH_LEVEL_COUNT; ++depthLevel)
  {
    currentZ = context.depthLevels[depthLevel].z;
    deltaZ = (currentZ - previousZ);
    absoluteZ = currentZ + carInfo.posZ;
    //float dzf = deltaZ / 256.f;
    
    if(absoluteZ < context.segments[1].segmentStartZ)
    {
      totalOffset += (context.segments[0].xCurvature * deltaZ);
    }
    else
    {
      if(absoluteZ < context.segments[2].segmentStartZ)
      {
        totalOffset += (context.segments[1].xCurvature * deltaZ);
      }
      else
      {
        totalOffset += (context.segments[2].xCurvature * deltaZ);
      }
    }

    x = SCREEN_WIDTH / 2 + (carInfo.posX + (totalOffset >> (ROAD_CURVATURE_X_SHIFT + Z_POSITION_SHIFT))) * context.depthLevels[depthLevel].scaleFactor;
    context.depthLevelToX[depthLevel] = x;
    previousZ = currentZ;
  }
}

void computeHills(CarInfo& carInfo, LevelContext& context) noexcept
{
    float prevZ = 0.f;
    float totalOffset = 0.f;

    float currZIndex = 0.f;
    //float totalCurvature = 0.f;

    int16_t zIndex = 0;
    int16_t y=0;

    while(zIndex < DEPTH_LEVEL_COUNT && y < SCREEN_HEIGHT && zIndex >= 0)
    {
      context.lineToDepthLevel[SCREEN_HEIGHT - 1 - y] = zIndex;

      const DepthInfo& di = context.depthLevels[zIndex];
      
      if(di.z + carInfo.posZ < context.segments[1].segmentStartZ)
      {
        totalOffset += context.segments[0].zCurvature * di.scaleFactor;
      }
      else
      {
        if(di.z + carInfo.posZ < context.segments[2].segmentStartZ)
        {
          totalOffset += context.segments[1].zCurvature * di.scaleFactor;
        }
        else
        {
          totalOffset += context.segments[2].zCurvature * di.scaleFactor;
        }
      }

      currZIndex = y + totalOffset;
      zIndex = currZIndex + 0.5f;
      ++y;
    }
    
    while(y < SCREEN_HEIGHT)
    {
      context.lineToDepthLevel[SCREEN_HEIGHT - 1 - y] = SKY_Z;
      ++y;
    }
}

void createSceneryObject(LevelContext& context, SceneryObject& object, Z_POSITION zPos, const LevelConfig& config)
{
  int16_t posX = random(-30, 30);
  if(posX > 0)
  {
    object.posX = config.roadWidth/2 + 20 + posX;
  }
  else
  {
    object.posX = - config.roadWidth/2 - 20 - posX;
  }
  object.posZ = zPos + random(0, context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2);
  object.sprite = context.sprites + random(config.sceneryObjectsIndexStart, config.sceneryObjectsIndexEnd);
}

void createSceneryObjects(LevelContext& context, const LevelConfig& config)
{
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    createSceneryObject(context, object, 0, config);
  }
}

void updateSceneryObjects(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config)
{
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    if(object.posZ < carInfo.posZ)
    {
      createSceneryObject(context, object, carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-1].z, config);
    }
  }
}

void createStaticObstacle(LevelContext& context, StaticObstacle& object, Z_POSITION zPos, const LevelConfig& config)
{
  object.posX = random(- config.roadWidth/2, config.roadWidth/2);
  object.posZ = zPos + random(0, context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2);
  object.sprite = context.sprites + random(config.staticObstaclesIndexStart, config.staticObstaclesIndexEnd);
  object.validUntil = -1;
}

void createStaticObstacles(LevelContext& context, const LevelConfig& config)
{
    for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    createStaticObstacle(context, object, 0, config);
  }
}

void updateStaticObstacles(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config)
{
  for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    if(object.posZ < carInfo.posZ || (object.validUntil != -1 && object.validUntil > gb.frameCount))
    {
      createStaticObstacle(context, object, carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-5].z, config);
    }
  }
}

void createMovingObstacle(LevelContext& context, MovingObstacle& object, Z_POSITION zPos, const LevelConfig& config)
{
  object.posX = //random(- config.roadWidth/2, config.roadWidth/2);
                //(random(0, 2)  ? -object.sprite->width : 0);
                 random(-30, 30) + object.sprite->width/2;//random(- config.roadWidth/2, config.roadWidth/2 - object.sprite->width/2);
  object.posZ = zPos + random(0, context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2);
  object.speedZ = random(2, 10) * 0.1f;
  object.sprite = context.sprites + random(config.movingObstaclesIndexStart, config.movingObstaclesIndexEnd);
}

void createMovingObstacles(LevelContext& context, const LevelConfig& config)
{
    for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    createMovingObstacle(context, object, 0, config);
  }
}

void updateMovingObstacles( LevelContext& context, const CarInfo& carInfo, const LevelConfig& config)
{
  Z_POSITION threshold;
  for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    object.posZ += object.speedZ * 256;
    threshold = carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-5].z;
    if(object.posZ < carInfo.posZ)
    {
      createMovingObstacle(context, object, threshold, config);
    }
    if(object.posZ > threshold)
    {
      object.posZ = threshold;
    }
  }
}

uint8_t computeDrawable(LevelContext& context, int16_t posX, Z_POSITION posZ, SpriteDefinition* sprite, const CarInfo& carInfo, uint8_t index)
{
  uint8_t drawableLine = -1;
  Z_POSITION prevZ = Z_POSITION_MAX;
  
    for(unsigned int line = 0; line < SCREEN_HEIGHT && drawableLine == (uint8_t)-1; ++line)
    {
      if(context.lineToDepthLevel[line] != SKY_Z)
      {
        
        if(context.depthLevels[context.lineToDepthLevel[line]].z+carInfo.posZ <= posZ && prevZ > posZ)
        {
          drawableLine = line;
        }
        prevZ = context.depthLevels[context.lineToDepthLevel[line]].z+carInfo.posZ;
      }
    }

    if(drawableLine != -1)
  {
    Drawable& drawable = context.drawables[index];
    drawable.sprite = sprite;
    
    SCALE_FACTOR scale = ScaleFactor[context.lineToDepthLevel[drawableLine]];
    if(scale <= SCALE_FACTOR_EIGHTH)
    {
      drawable.zoomPattern = 8;
    }
    else if(scale <= SCALE_FACTOR_QUARTER)
    {
      drawable.zoomPattern = 4;
    }
    else if(scale <= SCALE_FACTOR_HALF)
    {
      drawable.zoomPattern = 2;
    }
    else
    {
      drawable.zoomPattern = 1;
    }

    drawable.xStart = context.depthLevelToX[context.lineToDepthLevel[drawableLine]] - ((scale * posX) >> SCALE_FACTOR_SHIFT);
    
    uint16_t actualHeight = drawable.sprite->height / drawable.zoomPattern;
    drawable.yStart = (drawableLine >= actualHeight) ? drawableLine - actualHeight : 0;
    drawable.yEnd = drawable.yStart + actualHeight - 1;

    ++index;
  }

  return index;
}

uint8_t computeDrawables(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config)
{
  uint8_t nextDrawableIndex = 0;
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    nextDrawableIndex = computeDrawable(context,
                                        object.posX,
                                        object.posZ,
                                        object.sprite,
                                        carInfo,
                                        nextDrawableIndex);
  }

  for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    nextDrawableIndex = computeDrawable(context,
                                        object.posX,
                                        object.posZ,
                                        object.sprite,
                                        carInfo,
                                        nextDrawableIndex);
  }

  for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    nextDrawableIndex = computeDrawable(context,
                                        object.posX,
                                        object.posZ,
                                        object.sprite,
                                        carInfo,
                                        nextDrawableIndex);
  }

 return nextDrawableIndex;
}

int8_t computeCollision(int16_t carXMin, int16_t carXMax, int16_t obstacleX, SpriteDefinition* obstacleSprite)
{
  if(gb.buttons.repeat(BUTTON_MENU, 0))
  {
    SerialUSB.printf("carXMin %i carXMax %i obstacleX %i obstacleWidth %i\n", carXMin, carXMax, obstacleX, obstacleSprite->width);
  }
  
  if(carXMin > (obstacleX))
  {
    return 0;
  }

  if(carXMax < (obstacleX - obstacleSprite->width))
  {
    return 0;
  }

  if(carXMin > obstacleX - obstacleSprite->width/2)
  {
    return COLLISION_LEFT;
  }

  if(carXMax < obstacleX - obstacleSprite->width/2)
  {
    return COLLISION_RIGHT;
  }

  return COLLISION_FRONT;
}

void updateCarInfo(const LevelContext& context, CarInfo& carInfo, const LevelConfig& config)
{
  currentFx = nullptr;
  const RoadSegment& segment = context.segments[0];
  
  bool accelerating   = gb.buttons.repeat(BUTTON_A, 0);
  bool braking        = gb.buttons.repeat(BUTTON_B, 0);
  bool turningLeft    = gb.buttons.repeat(BUTTON_LEFT, 0);
  bool turningRight   = gb.buttons.repeat(BUTTON_RIGHT, 0);
  bool offRoad        = (carInfo.posX < -config.roadWidth/2 || carInfo.posX > config.roadWidth/2);
  const uint16_t* lightPattern = nullptr;


  // Compute collisions
  int8_t collision = 0;
  {
    Z_POSITION diffZ = 0;
    int16_t carXMin = carInfo.posX - CAR_WIDTH/2;
    int16_t carXMax = carInfo.posX + CAR_WIDTH/2;
    for(uint8_t objectIndex = 0; objectIndex < config.sceneryObjectsCount && collision != COLLISION_FRONT; ++objectIndex)
    {
      const SceneryObject& object = context.sceneryObjects[objectIndex];
      diffZ = 8*(object.posZ-carInfo.posZ);
      if(diffZ < 0)
      {
        diffZ = -diffZ;
      }
    
      if(diffZ <= (1 << Z_POSITION_SHIFT))
      {
        collision = collision | computeCollision(carXMin, carXMax, object.posX, object.sprite);
      }
    }

    for(uint8_t objectIndex = 0; objectIndex < config.staticObstaclesCount && collision != COLLISION_FRONT; ++objectIndex)
    {
      StaticObstacle& object = context.staticObstacles[objectIndex];
      diffZ = object.posZ-carInfo.posZ;
      if(diffZ < 0)
      {
        diffZ = -diffZ;
      }
    
      if(diffZ <= (1 << Z_POSITION_SHIFT))
      {
        int8_t currentCollision = computeCollision(carXMin, carXMax, object.posX, object.sprite);
        collision = collision | currentCollision;
        if(currentCollision)
        {
          object.validUntil = gb.frameCount + OBJECT_VALIDITY;
        }
      }
    }

   

    for(uint8_t objectIndex = 0; objectIndex < config.movingObstaclesCount && collision != COLLISION_FRONT; ++objectIndex)
    {
      MovingObstacle& object = context.movingObstacles[objectIndex];
      diffZ = object.posZ-carInfo.posZ;
      if(diffZ < 0)
      {
        diffZ = -diffZ;
      }
    
      if(diffZ <= (1 << Z_POSITION_SHIFT))
      {
        int8_t currentCollision = computeCollision(carXMin, carXMax, object.posX, object.sprite);
        collision = collision | currentCollision;
      }
    }

              // If skyway, collision against boundaries
      if(config.level == Level::Skyway)
      {
        if(carInfo.posX < -config.roadWidth/2)
        {
          collision = collision | COLLISION_LEFT;
          turningRight = false;
          turningLeft = true;
          carInfo.posX = -config.roadWidth/2;
        }
        else if(carInfo.posX > config.roadWidth/2)
        {
          collision = collision | COLLISION_RIGHT;
          turningRight = true;
          turningLeft = false;
          carInfo.posX = config.roadWidth/2;
        }
      }

  } // end compute collisions

  float accelerationValue(0);
  if(accelerating)
  {
    accelerationValue = accelFromSpeed(carInfo.speedZ);
  }
  else if(braking)
  {
    accelerationValue = -0.015;
  }
  else
  {
    accelerationValue = -0.005;
  }

  float accelX(segment.xCurvature * carInfo.speedZ / 200.), accelZ(0.f);
  //SerialUSB.printf("AccelX1 %i\n", accelX*10000);
  if(accelX > 0.3)
  {
    accelX = 0.3;
  }
  if(accelX < -0.3)
  {
    accelX = -0.3;
  }
  if(accelerating)
  {
    if(turningLeft)
    {
      accelX += carInfo.speedZ / 2;
      accelZ = accelerationValue * 0.8f;
    }
    else if(turningRight)
    {
      accelX += -carInfo.speedZ  / 2;
      accelZ = accelerationValue * 0.8f;
    }
    else
    {
      accelX += -carInfo.speedX/2; // center car
      accelZ = accelerationValue;
    }
  }
  else
  {
    if(turningLeft)
    {
      accelX += carInfo.speedZ / 2;
      accelZ = accelerationValue * 0.8f;
    }
    else if(turningRight)
    {
     accelX += -carInfo.speedZ / 2;
      accelZ = accelerationValue * 0.8f;
    }
    else
    {
      accelX += -carInfo.speedX/2; // center car
      accelZ = accelerationValue;
    }
  }

  carInfo.sprite = CarSprite::Front;
  if(carInfo.speedZ)
  {
    if(turningLeft)
      carInfo.sprite = CarSprite::Left;
    else if(turningRight)
      carInfo.sprite = CarSprite::Right;
  }

  if(offRoad)
  {
    accelX /= 2;
    accelZ /= 2;
  }

  switch(collision)
  {
    case COLLISION_LEFT:
        lightPattern = LIGHT_COLLISION_LEFT;
        carInfo.speedZ /= 2;
        carInfo.speedX += 0.5;
        break;

    case COLLISION_RIGHT:
        lightPattern = LIGHT_COLLISION_RIGHT;
        carInfo.speedZ /= 2;
        carInfo.speedX -= 0.5;
        break;

    case COLLISION_FRONT:
        lightPattern = LIGHT_COLLISION_FRONT;
        carInfo.speedZ /= 2;
        break;

    default:
      break;
  }

  if(collision)
  {
    currentFx = collisionSfx;
  }

  carInfo.speedX += accelX;
  carInfo.speedZ += accelZ;
  
  if(offRoad)
  {
    if(carInfo.speedZ > 0.7)
    {
      carInfo.speedZ = 0.7;
    }
  }

  float maxSpeedZ = (offRoad ? MAX_SPEED_Z/5 : MAX_SPEED_Z);
  if(carInfo.speedZ <= 0)
  {
    carInfo.speedZ = 0;
  }
  else if(carInfo.speedZ >= MAX_SPEED_Z)
  {
    carInfo.speedZ = MAX_SPEED_Z;
  }

  if(carInfo.speedZ == MAX_SPEED_Z)
  {
      capacitorCharge += 1;
  }
  else
  {
    capacitorCharge -= 5;
    if(capacitorCharge < 0)
    {
      capacitorCharge = 0;
    }
  }

/*if(!currentFx)
{
  if(carInfo.speedZ <= 0.7)
  {
    currentFx = engineLow;
  }
  else if(carInfo.speedZ <= 1.3)
  {
    currentFx = engineMid;
  }
  else
  {
    currentFx = engineHigh;
  }
} */

  carInfo.posX += carInfo.speedX;
  carInfo.posZ += carInfo.speedZ * 256;
  remainingFuel -= carInfo.speedZ * 256;

  if(lightPattern)
  {
    carInfo.lights = lightPattern;
    //gb.lights.drawImage(0, 0, lightPattern);
  }
  else
  {
    int8_t step = 0;
    if(capacitorCharge < 100)
    {
      step = ((capacitorCharge / 20) % 5);
    }
    else if(capacitorCharge < 200)
    {
      step = ((capacitorCharge / 10) % 5);
    }
    else if(capacitorCharge < 300)
    {
      step = ((capacitorCharge / 2) % 5);
    }

    switch(step)
    {
      case 1:
        if(!lightPattern)
          carInfo.lights = LIGHT_1;
        carInfo.fluxSprite = &context.speedSprites[1];
        break;

      case 2:
        if(!lightPattern)
          carInfo.lights = LIGHT_2;
        carInfo.fluxSprite = &context.speedSprites[2];
        //gb.lights.drawImage(0, 0, LIGHT_2);
        break;

      case 3:
        if(!lightPattern)
          carInfo.lights = LIGHT_3;
        carInfo.fluxSprite = &context.speedSprites[3];
        //gb.lights.drawImage(0, 0, LIGHT_3);
        break;

      case 4:
        if(!lightPattern)
          carInfo.lights = LIGHT_4;
        carInfo.fluxSprite = &context.speedSprites[4];
        //gb.lights.drawImage(0, 0, LIGHT_4);
        break;

      default:
        carInfo.lights = LIGHT_NONE;
        carInfo.fluxSprite = &context.speedSprites[0];
        //gb.lights.drawImage(0, 0, LIGHT_NONE);
        break;
    }
    
  }

  if(capacitorCharge >= 300)
    {
      SerialUSB.printf("FLUXING\n");
      carInfo.fluxed = true;
      capacitorCharge = 0;
    }

  gb.sound.fx(currentFx);

}

void drawSprites(uint16_t y, uint16_t* stripLine,  unsigned int drawableCount, LevelContext& context) noexcept
{
    for(unsigned int drawableIndex = 0; drawableIndex < drawableCount; ++drawableIndex)
    {
      const Drawable& drawable = context.drawables[drawableIndex];
      if(y >= drawable.yStart && y <= drawable.yEnd)
      {
        //uint16_t offset = drawable.sprite->width * (y - drawable.yStart) * drawable.zoomPattern;
        const uint16_t* spriteBufferWithOffset = drawable.sprite->buffer + (drawable.sprite->width * (y - drawable.yStart) * drawable.zoomPattern);
        int16_t xIndex = drawable.xStart;
        uint16_t color;
        for(int16_t x = 0; x < drawable.sprite->width && xIndex < SCREEN_WIDTH; x+=drawable.zoomPattern, ++xIndex)
        {
          if(xIndex >= 0)
          {
            color = (*spriteBufferWithOffset);
            spriteBufferWithOffset += drawable.zoomPattern;
            if(color != COLOR_565(0xFF, 0x00, 0xFF))
            {
              stripLine[xIndex] = color;
            }
          }
        }
      }
    }
}



void drawFrame(GraphicsManager& gm,
                LevelContext& context,
                unsigned int drawableCount,
                const CarInfo& carInfo,
                int16_t backgroundShift
                ) noexcept
{
    uint16_t* strip = gm.StartFrame();
  uint16_t* stripLine;
  uint16_t* stripCursor = strip;

  unsigned int yStrip = 1;

  int8_t actualShift(backgroundShift >> ROAD_CURVATURE_X_SHIFT);
  unsigned int backgroundY(0);

  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip)
  {
    stripLine = stripCursor;
    uint8_t depthLevel = context.lineToDepthLevel[y];

    backgroundY = y;
    
    if(depthLevel == SKY_Z)
    {
      // draw sky

      if(backgroundY >= context.backgroundHeight)
      {
        backgroundY = context.backgroundHeight-1;
      }
      uint16_t pixelsIndex = (backgroundY << 7) + (uint8_t)(actualShift);
      const uint8_t* colorIndexes = &context.background[pixelsIndex];
      uint32_t color32;
      uint32_t* stripCursor32 = (uint32_t*)stripCursor;
      for(uint16_t ii = 0; ii < SCREEN_WIDTH; ii+=2)
      {
        color32 = context.backgroundPalette[(*colorIndexes) >> 4] | ((uint32_t)context.backgroundPalette[(*colorIndexes) & 0x0F]) << 16;
        (*stripCursor32++) = color32;
        ++colorIndexes;
      }
      stripCursor += SCREEN_WIDTH;
    }
    else
    {
      DepthInfo& di = context.depthLevels[depthLevel];
      int16_t x = context.depthLevelToX[depthLevel];
      
      int altColor = (((uint16_t)(di.z + carInfo.posZ) >> (Z_POSITION_SHIFT + 2)) & 0x1);
      uint16_t* currentPalette = context.trackPalette + (COLOR_TRACK_SIZE) * altColor;
      int16_t col = 0;
      const uint16_t& grassColor = currentPalette[COLOR_TRACK_GRASS_INDEX];
      const uint16_t& bumperColor = currentPalette[COLOR_TRACK_BUMPER_INDEX];
      const uint16_t& lineColor = currentPalette[COLOR_TRACK_LINE_INDEX];
      const uint16_t& roadColor = currentPalette[COLOR_TRACK_ROAD_INDEX];
      uint32_t grassColor32 = ((uint32_t)(grassColor) << 16) | grassColor;
      uint32_t bumperColor32 = ((uint32_t)(bumperColor) << 16) | bumperColor;
      uint32_t lineColor32 = ((uint32_t)(lineColor) << 16) | lineColor;
      uint32_t roadColor32 = ((uint32_t)(roadColor) << 16) | roadColor;
      int16_t target = x - di.lineRoadBumperWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = grassColor;
      }
      target = x - di.lineRoadWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = bumperColor;
      }
      target = x - di.lineWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = roadColor;
      }
      target = x + di.lineWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = lineColor;
      }
      target = x + di.lineRoadWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = roadColor;
      }
      target = x + di.lineRoadBumperWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = bumperColor;
      }
      for(; col < SCREEN_WIDTH; ++col)
      {
        (*stripCursor++) = grassColor;
      }
    }

    drawSprites(y, stripLine, drawableCount, context);

    if(yStrip == STRIP_HEIGHT)
    {
      strip = gm.CommitStrip();
      stripCursor = strip;
      yStrip = 0;
    }
  }


  gm.EndFrame();
}

void drawFrameSkyway(GraphicsManager& gm,
                      LevelContext& context,
                      unsigned int drawableCount,
                      const CarInfo& carInfo,
                      int16_t backgroundShift
                      ) noexcept
{
    uint16_t* strip = gm.StartFrame();
  uint16_t* stripLine;
  uint16_t* stripCursor = strip;

  unsigned int yStrip = 1;
  int8_t actualShift(backgroundShift >> ROAD_CURVATURE_X_SHIFT);
  unsigned int backgroundY;
uint8_t pulse = gb.frameCount << 3;
  for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip)
  {
    stripLine = stripCursor;
    uint8_t depthLevel = context.lineToDepthLevel[y];

    backgroundY = y;
    {
      // draw sky

      if(backgroundY >= context.backgroundHeight)
      {
        backgroundY = context.backgroundHeight-1;
      }
      uint16_t pixelsIndex = (backgroundY << 7) + (uint8_t)(actualShift);
      const uint8_t* colorIndexes = &context.background[pixelsIndex];
      uint32_t color32;
      uint32_t* stripCursor32 = (uint32_t*)stripCursor;
      for(uint16_t ii = 0; ii < SCREEN_WIDTH; ii+=2)
      {
        color32 = context.backgroundPalette[(*colorIndexes) >> 4] | ((uint32_t)context.backgroundPalette[(*colorIndexes) & 0x0F]) << 16;
        (*stripCursor32++) = color32;
        ++colorIndexes;
      }
    }
    
    if(depthLevel == SKY_Z)
    {
      stripCursor += SCREEN_WIDTH;
    }
    else
    {
      DepthInfo& di = context.depthLevels[depthLevel];
      int16_t x = context.depthLevelToX[depthLevel];
      
      int altColor = ((((uint16_t)(di.z + carInfo.posZ) >> (Z_POSITION_SHIFT + 2)) & 0x3) == 0);
      uint16_t* currentPalette = context.trackPalette + (COLOR_TRACK_SIZE) * altColor;
      int16_t col = 0;
      const uint16_t& bumperColor = COLOR_565(pulse, pulse, pulse);
      const uint16_t& lineColor = COLOR_565(pulse, 0, 0);
      int16_t target = x - di.lineRoadBumperWidth;
      
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      if(target > col)
      {
        stripCursor += (target-col);
        col = target;
      }
      target = x - di.lineRoadWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = bumperColor;
      }
      if(altColor)
      {
        target = x - di.lineWidth;
        if(target >= SCREEN_WIDTH)
        {
          target = SCREEN_WIDTH - 1;
        }
        if(target > col)
        {
          stripCursor += (target-col);
          col = target;
        }
        target = x + di.lineWidth;
        if(target >= SCREEN_WIDTH)
        {
          target = SCREEN_WIDTH - 1;
        }
        for(; col < target; ++col)
        {
          (*stripCursor++) = lineColor;
        }
      }
      target = x + di.lineRoadWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      if(target > col)
      {
        stripCursor += (target-col);
        col = target;
      }
      target = x + di.lineRoadBumperWidth;
      if(target >= SCREEN_WIDTH)
      {
        target = SCREEN_WIDTH - 1;
      }
      for(; col < target; ++col)
      {
        (*stripCursor++) = bumperColor;
      }
      if(SCREEN_WIDTH > col)
      {
        stripCursor += (SCREEN_WIDTH-col);
        col = SCREEN_WIDTH;
      }
    }

    drawSprites(y, stripLine, drawableCount, context);

    if(yStrip == STRIP_HEIGHT)
    {
      strip = gm.CommitStrip();
      stripCursor = strip;
      yStrip = 0;
    }
  }


  gm.EndFrame();
}

/*const uint16_t startSound[] = {0x0005,0x338,0x3FC,0x254,0x1FC,0x25C,0x3FC,0x368,0x123};

const Gamebuino_Meta::Sound_FX my_sfx[] = {
  {Gamebuino_Meta::Sound_FX_Wave::NOISE,1,70,0,0,240,1},
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,0,0,-3,50,5},
  {Gamebuino_Meta::Sound_FX_Wave::NOISE,0,70,0,0,224,1},
};*/

void titleLoop(/*const LevelConfig& config*/const uint8_t* title, const uint16_t* palette, uint16_t width, uint16_t height) noexcept
{
  resetAlloc();
  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  GraphicsManager gm(strip1, strip2);

  uint16_t startY = (SCREEN_HEIGHT - height)/2;

  uint8_t animationPhase = 0; //  0: sliding in, 1: waiting for button press, 2: sliding right
  int16_t offset(-160);

  while(true)
  { 
    while (!gb.update());

   if(gb.buttons.repeat(BUTTON_A, 0) && animationPhase == 1)
   {
     animationPhase = 2;
   }

    uint32_t color32;

    uint16_t* strip = gm.StartFrame();

    unsigned int yStrip = 1;

    uint16_t* stripCursor = strip;
    //const uint8_t* colorIndexes = title;
    
    for(unsigned int y = 0; y < SCREEN_HEIGHT; ++y, ++yStrip)
    {
        uint32_t* stripCursor32 = (uint32_t*)stripCursor;
       
        if(y >= startY && y < startY + height)
        {
          const uint8_t* colorIndexes = &title[(y - startY) * 80];
          switch(animationPhase)
          {
            case 0:
            {
              colorIndexes -= offset/2;
              for(uint16_t ii = 0; ii < SCREEN_WIDTH + offset; ii+=2)
              {
                color32 = palette[(*colorIndexes) >> 4] | ((uint32_t)palette[(*colorIndexes) & 0x0F]) << 16;
                (*stripCursor32++) = color32;
                ++colorIndexes;
              }
              for(uint16_t ii = SCREEN_WIDTH + offset; ii < SCREEN_WIDTH; ii+=2)
              {
                (*stripCursor32++) = 0;
              }
            }
            break;

            case 2:
            {
              for(uint16_t ii = 0; ii < offset; ii+=2)
              {
                (*stripCursor32++) = 0;
              }
              for(uint16_t ii = offset; ii < SCREEN_WIDTH; ii+=2)
              {
                color32 = palette[(*colorIndexes) >> 4] | ((uint32_t)palette[(*colorIndexes) & 0x0F]) << 16;
                (*stripCursor32++) = color32;
                ++colorIndexes;
              }
            }
            break;

            default:
            {
              for(uint16_t ii = 0; ii < SCREEN_WIDTH; ii+=2)
              {
                color32 = palette[(*colorIndexes) >> 4] | ((uint32_t)palette[(*colorIndexes) & 0x0F]) << 16;
                (*stripCursor32++) = color32;
                ++colorIndexes;
              }
            }
            break;
          }


        }
        else
        {
          for(uint16_t ii = 0; ii < SCREEN_WIDTH; ii+=2)
          {
            (*stripCursor32++) = 0;
          }
        }

       stripCursor += SCREEN_WIDTH;

        
      if(yStrip == STRIP_HEIGHT)
      {
        strip = gm.CommitStrip();
        stripCursor = strip;
        yStrip = 0;
      }
    }
    
    gm.EndFrame();

    switch(animationPhase)
    {
      case 0:
        offset+=4;
        if(offset == 0)
        {
          animationPhase = 1;
        }
        break;

      case 2:
        offset+=8;
        if(offset == 160)
        {
          return;
        }
        break;

      default:
        break;
    }
  }
}

int gameLoop(LevelConfig& config) noexcept
{
  //gb.sound.fx(my_sfx);
  
  resetAlloc();
  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);

  LevelContext context;
  context.depthLevels       = (DepthInfo*)        mphAlloc(DEPTH_LEVEL_COUNT * sizeof(DepthInfo));
  context.lineToDepthLevel  = (uint8_t*)          mphAlloc(SCREEN_HEIGHT);
  context.depthLevelToX     = (int16_t*)          mphAlloc(DEPTH_LEVEL_COUNT * sizeof(int16_t));
  context.trackPalette      = (uint16_t*)         mphAlloc(2 * COLOR_TRACK_SIZE * sizeof(uint16_t));
  context.segments          = (RoadSegment*)      mphAlloc(3 * sizeof(RoadSegment));
  context.carSprites        = (SpriteDefinition*) mphAlloc(3 * sizeof(SpriteDefinition));
  context.fuelSprites       = (SpriteDefinition*) mphAlloc(2 * sizeof(SpriteDefinition));
  context.speedSprites      = (SpriteDefinition*) mphAlloc(5 * sizeof(SpriteDefinition));
  context.sprites           = (SpriteDefinition*) mphAlloc(MAX_SPRITES * sizeof(SpriteDefinition));
  context.sceneryObjects    = (SceneryObject*)    mphAlloc(MAX_SCENERY_OBJECTS * sizeof(SceneryObject));
  context.staticObstacles   = (StaticObstacle*)   mphAlloc(MAX_STATIC_OBSTACLES * sizeof(StaticObstacle));
  context.movingObstacles   = (MovingObstacle*)   mphAlloc(MAX_MOVING_OBSTACLES * sizeof(MovingObstacle));
  context.drawables         = (Drawable*)         mphAlloc(MAX_DRAWABLES * sizeof(Drawable));

  if(config.level == Level::Arizona)
  {
    context.sprites[0].width = CACTUS_WIDTH;
    context.sprites[0].height = CACTUS_HEIGHT;
    context.sprites[0].buffer = CACTUS;
  
    context.sprites[1].width = BUSH_WIDTH;
    context.sprites[1].height = BUSH_HEIGHT;
    context.sprites[1].buffer = BUSH;
  
    context.sprites[2].width = BOULDER_WIDTH;
    context.sprites[2].height = BOULDER_HEIGHT;
    context.sprites[2].buffer = BOULDER;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = 0;
    config.movingObstaclesIndexStart = 0;
    config.movingObstaclesIndexEnd = 0;
  }
  else if(config.level == Level::Suburb)
  {
    context.sprites[0].width = TREE_WIDTH;
    context.sprites[0].height = TREE_HEIGHT;
    context.sprites[0].buffer = TREE;
  
    context.sprites[1].width = BUSH_WIDTH;
    context.sprites[1].height = BUSH_HEIGHT;
    context.sprites[1].buffer = BUSH;
  
    context.sprites[2].width = CAR_WIDTH;
    context.sprites[2].height = CAR_HEIGHT;
    context.sprites[2].buffer = CAR;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 2;
    config.staticObstaclesCount = 0;
    config.staticObstaclesIndexStart = 0;
    config.staticObstaclesIndexEnd = 2;
    config.movingObstaclesCount = 2;
    config.movingObstaclesIndexStart = 2;
    config.movingObstaclesIndexEnd = 3;
  }
  else // Skyway
  {
    context.sprites[0].width = CACTUS_WIDTH;
    context.sprites[0].height = CACTUS_HEIGHT;
    context.sprites[0].buffer = CACTUS;
  
    context.sprites[1].width = BUSH_WIDTH;
    context.sprites[1].height = BUSH_HEIGHT;
    context.sprites[1].buffer = BUSH;
  
    context.sprites[2].width = BOULDER_WIDTH;
    context.sprites[2].height = BOULDER_HEIGHT;
    context.sprites[2].buffer = BOULDER;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = 0;
    config.movingObstaclesIndexStart = 0;
    config.movingObstaclesIndexEnd = 0;
  }

  context.carSprites[0].width = CAR_WIDTH;
  context.carSprites[0].height = CAR_HEIGHT;
  context.carSprites[0].buffer = CAR;

  context.carSprites[1].width = CAR_LEFT_WIDTH;
  context.carSprites[1].height = CAR_LEFT_HEIGHT;
  context.carSprites[1].buffer = CAR_LEFT;

  context.carSprites[2].width = CAR_RIGHT_WIDTH;
  context.carSprites[2].height = CAR_RIGHT_HEIGHT;
  context.carSprites[2].buffer = CAR_RIGHT;

  context.fuelSprites[0].width = FUELF_WIDTH;
  context.fuelSprites[0].height = FUELF_HEIGHT;
  context.fuelSprites[0].buffer = FUELF;

  context.fuelSprites[1].width = FUELE_WIDTH;
  context.fuelSprites[1].height = FUELE_HEIGHT;
  context.fuelSprites[1].buffer = FUELE;

  /*context.sprites[3].width = CAR_WIDTH;
  context.sprites[3].height = CAR_HEIGHT;
  context.sprites[3].buffer = CAR;*/

  context.speedSprites[0].width = SPEEDO0_WIDTH;
  context.speedSprites[0].height = SPEEDO0_HEIGHT;
  context.speedSprites[0].buffer = SPEEDO0;

  context.speedSprites[1].width = SPEEDO1_WIDTH;
  context.speedSprites[1].height = SPEEDO1_HEIGHT;
  context.speedSprites[1].buffer = SPEEDO1;

  context.speedSprites[2].width = SPEEDO2_WIDTH;
  context.speedSprites[2].height = SPEEDO2_HEIGHT;
  context.speedSprites[2].buffer = SPEEDO2;

  context.speedSprites[3].width = SPEEDO3_WIDTH;
  context.speedSprites[3].height = SPEEDO3_HEIGHT;
  context.speedSprites[3].buffer = SPEEDO3;

  context.speedSprites[4].width = SPEEDO4_WIDTH;
  context.speedSprites[4].height = SPEEDO4_HEIGHT;
  context.speedSprites[4].buffer = SPEEDO4;
//  Z_POSITION zCactus = (100 << Z_POSITION_SHIFT);

  switch(config.level)
  {
    case Level::Arizona:
      context.background = BACKGROUND_ARIZONA;
      context.backgroundPalette = BACKGROUND_ARIZONA_PALETTE;
      context.backgroundHeight = BACKGROUND_ARIZONA_HEIGHT;
      break;

    case Level::Suburb:
      context.background = BACKGROUND_SUBURB;
      context.backgroundPalette = BACKGROUND_SUBURB_PALETTE;
      context.backgroundHeight = BACKGROUND_SUBURB_HEIGHT;
      break;

    default:
      context.background = BACKGROUND_SKYLINE;
      context.backgroundPalette = BACKGROUND_SKYLINE_PALETTE;
      context.backgroundHeight = BACKGROUND_SKYLINE_HEIGHT;
      break;
  }

  SpriteDefinition dotSprite;
  dotSprite.width = DOT_WIDTH;
  dotSprite.height = DOT_HEIGHT;
  dotSprite.buffer = DOT;

  GraphicsManager gm(strip1, strip2);
  
  Z_POSITION minSegmentSize;
  Z_POSITION maxSegmentSize;

  CarInfo carInfo;
  carInfo.sprite = CarSprite::Front;
  carInfo.posX = 0;
  carInfo.posZ = 0;
  carInfo.speedZ = 0.f;
  carInfo.speedX = 0.f;
  carInfo.fluxed = false;

  initDepthInfo(config, context, minSegmentSize, maxSegmentSize);
  initPalette(config.level, context);

  context.segments[0].xCurvature = random(-(config.xCurvature << ROAD_CURVATURE_X_SHIFT), (config.xCurvature << ROAD_CURVATURE_X_SHIFT));
  context.segments[0].zCurvature = random(config.zCurvatureMin, config.zCurvatureMax)/1000.f;
  context.segments[0].segmentStartZ = 0;

  context.segments[1].xCurvature = random(-(config.xCurvature << ROAD_CURVATURE_X_SHIFT), (config.xCurvature << ROAD_CURVATURE_X_SHIFT));
  context.segments[1].zCurvature = random(config.zCurvatureMin, config.zCurvatureMax)/1000.f;
  context.segments[1].segmentStartZ = 400 * (1 << Z_POSITION_SHIFT);//segments[0].segmentStartZ + random(minSegmentSize, maxSegmentSize);

  context.segments[2].xCurvature = random(-(config.xCurvature << ROAD_CURVATURE_X_SHIFT), (config.xCurvature << ROAD_CURVATURE_X_SHIFT));
  context.segments[2].zCurvature = random(config.zCurvatureMin, config.zCurvatureMax)/1000.f;
  context.segments[2].segmentStartZ = context.segments[1].segmentStartZ + random(minSegmentSize, maxSegmentSize);


  createSceneryObjects(context, config);
  createStaticObstacles(context, config);
  createMovingObstacles(context, config);

  int16_t backgroundShift = 0; /* sign.11.4 */

  capacitorCharge = 0;

  while(true)
  { 
    while (!gb.update());

    updateCarInfo(context, carInfo, config);

    /*switch((gb.frameCount >> 3) % 5)
    {
      case 0:
        gb.lights.drawImage(0, 0, LIGHT_NONE);
        break;

      case 1:
        gb.lights.drawImage(0, 0, LIGHT_1);
        break;

      case 2:
        gb.lights.drawImage(0, 0, LIGHT_2);
        break;

      case 3:
        gb.lights.drawImage(0, 0, LIGHT_3);
        break;

      case 4:
      default:
        gb.lights.drawImage(0, 0, LIGHT_4);
        break;
        
    }*/

// Update environment wrt to new car position

    if(carInfo.posZ > context.segments[1].segmentStartZ)
    {
      context.segments[0] = context.segments[1];
      context.segments[1] = context.segments[2];
      context.segments[2].xCurvature = random(-(5 << ROAD_CURVATURE_X_SHIFT), (5 << ROAD_CURVATURE_X_SHIFT));
      context.segments[2].zCurvature = random(-150, 400)/1000.f;
      context.segments[2].segmentStartZ = context.segments[1].segmentStartZ + random(minSegmentSize, maxSegmentSize);
    }
        backgroundShift += (context.segments[0].xCurvature * carInfo.speedZ) / 4;

// Compute environment geometry

    computeTurns(carInfo, context);
    computeHills(carInfo, context);

// Update drawables

    updateSceneryObjects(context, carInfo, config);
    updateStaticObstacles(context, carInfo, config);
    updateMovingObstacles(context, carInfo, config);

    uint8_t drawableCount = computeDrawables(context, carInfo, config);


  {
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = carInfo.fluxSprite;
    drawable.xStart = SCREEN_WIDTH - drawable.sprite->width;
    drawable.yStart = SCREEN_HEIGHT - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 1;
  }
  ++drawableCount;

    {

    float speedStep = MAX_SPEED_Z / SPEED_STEP_COUNT;
    int actualSpeedStep = 100;
    for(unsigned int step = 1; step <= SPEED_STEP_COUNT && actualSpeedStep == 100; ++step)
    {
      if((step * speedStep) > carInfo.speedZ)
      {
        actualSpeedStep = step;
      }
    }

    if(actualSpeedStep > SPEED_STEP_COUNT)
    {
      actualSpeedStep = SPEED_STEP_COUNT;
    }
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = &dotSprite;
    drawable.xStart = SPEEDOMETER_X[actualSpeedStep-1] - drawable.sprite->width;
    drawable.yStart = SPEEDOMETER_Y[actualSpeedStep-1] - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 1;
  }
  ++drawableCount;

    {
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = &context.fuelSprites[0];
    drawable.xStart = 0;
    drawable.yStart = SCREEN_HEIGHT - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 1;
  }
  ++drawableCount;



      {
        uint16_t fuelOffset = ((MAX_FUEL - remainingFuel) >> 17);
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = &context.fuelSprites[1];
    drawable.xStart = 0;
    drawable.yStart = SCREEN_HEIGHT - drawable.sprite->height - 4;
    drawable.yEnd = drawable.yStart + fuelOffset - 1;
    drawable.zoomPattern = 1;
            if(fuelOffset >= drawable.sprite->height - 4) // blink
        {
          if((gb.frameCount >> 4) & 0x01)
          {
            ++drawableCount;
          }
        }
        else
        {
          ++drawableCount;
        }

        if(fuelOffset >= drawable.sprite->height)
        {
          return 1; // game over
        }
  }

    {
    Drawable& drawable = context.drawables[drawableCount];
    switch(carInfo.sprite)
    {
      case CarSprite::Left:
        drawable.sprite = &context.carSprites[1];
        break;
      case CarSprite::Right:
        drawable.sprite = &context.carSprites[2];
        break;
      case CarSprite::Front:
      default:
        drawable.sprite = &context.carSprites[0];
        break;
    }
  
    drawable.xStart = SCREEN_WIDTH/2 - drawable.sprite->width/2;
    drawable.yStart = 120 - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 1;
  }
  ++drawableCount;

  gb.lights.drawImage(0, 0, carInfo.lights);

  if(config.level == Level::Skyway)
  {
    drawFrameSkyway(gm, context, drawableCount, carInfo, backgroundShift);
  }
  else
  {
    drawFrame(gm, context, drawableCount, carInfo, backgroundShift);
  }

  if(carInfo.fluxed)
  {
    return 0;
  }
  

/*if(carInfo.posZ > zCactus)
{
  zCactus += (400 << Z_POSITION_SHIFT);
}*/

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

  return 0;
  
}

void setup()
{
  gb.begin();
  // We aren't using the normal screen buffer, so initialize it to 0px × 0px.
  gb.display.init(0, 0, ColorMode::rgb565);

  // Just to push things to the limit for this example, increase to 40fps.
  gb.setFrameRate(40);

  // Init speedometer "hand" coordinates
  for(uint8_t index = 0; index < SPEED_STEP_COUNT; ++index)
  {
    float angle = MIN_SPEED_ANGLE + (MAX_SPEED_ANGLE - MIN_SPEED_ANGLE) * (index) / float(SPEED_STEP_COUNT);
    SPEEDOMETER_X[index] = (SCREEN_WIDTH - SPEED_HAND_CENTER_X + DOT_WIDTH/2 + SPEED_HAND_LENGTH * cos(angle) + 0.5);
    SPEEDOMETER_Y[index] = (SCREEN_HEIGHT - SPEED_HAND_CENTER_Y + DOT_HEIGHT/2 - SPEED_HAND_LENGTH * sin(angle) + 0.5);
  }

  SerialUSB.begin(9600);

  nextAvailableSegment = memory;
}

int runLevel(LevelConfig& config) noexcept
{
    const uint16_t * palette;
    uint16_t width, height;
    const uint8_t* title;
  
    switch(config.level)
    {
      case Level::Arizona:
        palette = TITLE_ARIZONA_PALETTE;
        width = TITLE_ARIZONA_WIDTH;
        height = TITLE_ARIZONA_HEIGHT;
        title = TITLE_ARIZONA;
        break;
  
      case Level::Suburb:
        palette = TITLE_SUBURB_PALETTE;
        width = TITLE_SUBURB_WIDTH;
        height = TITLE_SUBURB_HEIGHT;
        title = TITLE_SUBURB;
        break;
  
      default:
        palette = TITLE_SKYWAY_PALETTE;
        width = TITLE_SKYWAY_WIDTH;
        height = TITLE_SKYWAY_HEIGHT;
        title = TITLE_SKYWAY;
        break;
    }
  
    titleLoop(title, palette, width, height);

  return gameLoop(config);
}

void loop()
{
  while(true)
  {
    titleLoop(TITLE, TITLE_PALETTE, TITLE_WIDTH, TITLE_HEIGHT);

    remainingFuel = MAX_FUEL;
    
    LevelConfig config;
    if(gb.buttons.repeat(BUTTON_UP, 0))
    {
      config = levelSelectionMenu(Level::Suburb);
    }
    else if(gb.buttons.repeat(BUTTON_DOWN, 0))
    {
      config = levelSelectionMenu(Level::Skyway);
    }
    else
    {
      config = levelSelectionMenu(Level::Arizona);
    }
  
    if(runLevel(config) == 0) // level successful
    {
      config = levelSelectionMenu(Level::Suburb);
      if(runLevel(config) == 0) // level successful
      {
        config = levelSelectionMenu(Level::Skyway);
        if(runLevel(config) == 0) // level successful
        {
          titleLoop(SUCCESS, SUCCESS_PALETTE, SUCCESS_WIDTH, SUCCESS_HEIGHT);
        }
        else
        {
          titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT);
        }
      }
      else
      {
        titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT);
      }
    }
    else
    {
      titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT);
    }
      
  }
}
