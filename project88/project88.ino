#include "GraphicsManager.h"

#include "Car.h"
#include "WildWest.h"
#include "Suburb.h"
#include "SkyWay.h"
#include "Pinpin.h"
#include "Titles.h"
#include "Dashboard.h"
#include "SoundEffects.h"

using namespace roads;

#define HORIZON_OFFSET (200 << Z_POSITION_SHIFT)

#define MEMORY_SEGMENT_SIZE 10000
uint8_t memory[MEMORY_SEGMENT_SIZE];
void* nextAvailableSegment;

int16_t capacitorCharge;
Z_POSITION remainingFuel;

const Gamebuino_Meta::Sound_FX* collisionFx;
uint8_t collisionFxDuration;
const Gamebuino_Meta::Sound_FX* swooshFx;
uint8_t swooshFxDuration;

int bonusCount;

force_inline float accelFromSpeed(float speed) noexcept
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

void* mphAlloc(uint16_t size) noexcept
{
  if(nextAvailableSegment + size >= memory + MEMORY_SEGMENT_SIZE)
  {
    return NULL;
  }

  void* res = nextAvailableSegment;
  nextAvailableSegment += size;
  
  return res;
}

void resetAlloc() noexcept
{
  nextAvailableSegment = memory;
}

int32_t allocFreeRam() noexcept
{
  return memory + MEMORY_SEGMENT_SIZE - (uint8_t*)nextAvailableSegment;
}

extern SdFat SD;

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
  else if(level == Level::Skyway)
  {
    config.xCurvature = 6;
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
  else // Bonus
  {
    config.xCurvature = 6;
    config.zCurvatureMin = -200;
    config.zCurvatureMax = 600;
    config.bumperWidth  = 8;
    config.roadWidth    = 160;
    config.lineWidth    = 8;
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS / 2;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES / 2;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = MAX_MOVING_OBSTACLES / 2;
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
      depthInfo.lineWidth = (int32_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);
      offset += (config.roadWidth) >> 1;
      depthInfo.lineRoadWidth = (int32_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);
      offset += config.bumperWidth;
      depthInfo.lineRoadBumperWidth = (int32_t)((scaleFactor*offset) >> SCALE_FACTOR_SHIFT);

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
    context.trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(/*93, 130, 37*/113, 133, 54); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX]  = COLOR_565(/*118, 160, 54*/130, 154, 49);
    context.trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(100, 100, 100); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX] = context.trackPalette[COLOR_TRACK_BUMPER_INDEX];
    context.trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(142, 142, 142); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX] = COLOR_565(186, 186, 186);
    context.trackPalette[COLOR_TRACK_LINE_INDEX]   = COLOR_565(255, 255, 255); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX] = context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX];
  }
  else if(level == Level::Bonus)
  {
    context.trackPalette[COLOR_TRACK_GRASS_INDEX]  = COLOR_565(254, 191, 16); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_GRASS_INDEX]  = COLOR_565(254, 138, 16);
    context.trackPalette[COLOR_TRACK_BUMPER_INDEX] = COLOR_565(83, 94, 187); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_BUMPER_INDEX] = COLOR_565(199, 202, 233);
    context.trackPalette[COLOR_TRACK_ROAD_INDEX]   = COLOR_565(254, 207, 219); context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX] = COLOR_565(254, 162, 123);
    context.trackPalette[COLOR_TRACK_LINE_INDEX]   =  context.trackPalette[COLOR_TRACK_ROAD_INDEX]; context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_LINE_INDEX] = context.trackPalette[COLOR_TRACK_SIZE + COLOR_TRACK_ROAD_INDEX];
  }
  else // Skyway
  {
    
  }
}

force_inline void computeTurns(CarInfo& carInfo, LevelContext& context) noexcept
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

force_inline void computeHills(CarInfo& carInfo, LevelContext& context) noexcept
{
    float prevZ = 0.f;
    float totalOffset = 0.f;

    float currZIndex = 0.f;
    //float totalCurvature = 0.f;

    int32_t zIndex = 0;
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

void createSceneryObject(LevelContext& context, SceneryObject& object, Z_POSITION zPos, const LevelConfig& config) noexcept
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

void createSceneryObjects(LevelContext& context, const LevelConfig& config) noexcept
{
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    createSceneryObject(context, object, 0, config);
  }
}

force_inline void updateSceneryObjects(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config) noexcept
{
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    if(unlikely(object.posZ < carInfo.posZ))
    {
      createSceneryObject(context, object, carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2, config);
    }
  }
}

void createJerrican(LevelContext& context, Jerrican& object, Z_POSITION zPos, const LevelConfig& config) noexcept
{
    object.posX = random(- config.roadWidth/2, config.roadWidth/2);
    object.posZ = zPos + (1500 << Z_POSITION_SHIFT); // Every 1500m
    object.sprite = context.sprites + JERRICAN_SPRITE_INDEX;
    object.visible = true; 
}

force_inline void updateJerrican(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config) noexcept
{
  if(!context.jerrican)
    return;
  
  Jerrican& object = *(context.jerrican);
  if(unlikely(object.posZ < carInfo.posZ))
  {
    if(context.remainingJerricans)
    {
      createJerrican(context, object, carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-5].z/2, config);
      --context.remainingJerricans;
    }
    else
    {
      context.jerrican = nullptr;
    }
  }
}

void createBonusStar(LevelContext& context, BonusStar& object, const LevelConfig& config) noexcept
{
  object.posX = random(- config.roadWidth/2, config.roadWidth/2);
  object.posZ = (200 << Z_POSITION_SHIFT);
  object.sprite = context.sprites + BONUS_SPRITE_INDEX;
  object.speedZ = 1.f;
  object.visible = true;
}

force_inline void updateBonusStar(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config) noexcept
{
  if(!context.bonusStar)
    return;
    
  BonusStar& object = *(context.bonusStar);
  if(!object.visible)
    return;

  if(unlikely(object.posZ < carInfo.posZ))
  {
    object.visible = false;
    context.bonusStar = nullptr;
    return;
  }
    
  if(unlikely(object.posZ > carInfo.posZ + (1000 << Z_POSITION_SHIFT)))
  {
    object.visible = false;
  }
  object.posZ += object.speedZ * 256;
}

void createStaticObstacle(LevelContext& context, StaticObstacle& object, Z_POSITION zPos, const LevelConfig& config) noexcept
{
  object.posX = random(- config.roadWidth/2, config.roadWidth/2);
  object.posZ = zPos + random(0, context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2);
  object.sprite = context.sprites + random(config.staticObstaclesIndexStart, config.staticObstaclesIndexEnd);
  object.validUntil = -1;
}

void createStaticObstacles(LevelContext& context, const LevelConfig& config) noexcept
{
    for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    createStaticObstacle(context, object, 0, config);
  }
}

force_inline void updateStaticObstacles(LevelContext& context, const CarInfo& carInfo, const LevelConfig& config) noexcept
{
  for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    if(unlikely(object.posZ < carInfo.posZ || (object.validUntil != -1 && object.validUntil > gb.frameCount)))
    {
      createStaticObstacle(context, object, carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-5].z/2, config);
    }
  }
}

void createMovingObstacle(LevelContext& context, MovingObstacle& object, Z_POSITION zPos, const LevelConfig& config) noexcept
{
  object.posX = //random(- config.roadWidth/2, config.roadWidth/2);
                //(random(0, 2)  ? -object.sprite->width : 0);
                 random(-30, 30) + object.sprite->width/2;//random(- config.roadWidth/2, config.roadWidth/2 - object.sprite->width/2);
  object.posZ = zPos + random(0, context.depthLevels[DEPTH_LEVEL_COUNT-1].z/2);
  object.speedZ = random(2, 10) * 0.1f;
  object.sprite = context.sprites + random(config.movingObstaclesIndexStart, config.movingObstaclesIndexEnd);
}

void createMovingObstacles(LevelContext& context, const LevelConfig& config) noexcept
{
    for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    createMovingObstacle(context, object, 0, config);
  }
}

force_inline void updateMovingObstacles( LevelContext& context, const CarInfo& carInfo, const LevelConfig& config) noexcept
{
  Z_POSITION threshold;
  for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    object.posZ += object.speedZ * 256;
    threshold = carInfo.posZ + context.depthLevels[DEPTH_LEVEL_COUNT-5].z;
    if(unlikely(object.posZ < carInfo.posZ))
    {
      createMovingObstacle(context, object, threshold, config);
    }
    if(object.posZ > threshold)
    {
      object.posZ = threshold;
    }
  }
}

force_inline void insertDrawableIntoSortedList(Drawable& drawable, Drawable*& drawableList)
{
  Drawable* current = drawableList;

  if(unlikely(current == nullptr || current->yEnd > drawable.yEnd))
  {
    drawable.next = current;
    drawableList = &drawable;
    return;
  }

  while(current)
  {
    // precondition: current->yEnd <= drawable.yEnd
    if(current->next == nullptr || current->next->yEnd > drawable.yEnd)
    {
      // insert
      drawable.next = current->next;
      drawable.removable = true;
      current->next = & drawable;
      return;
    }
    current = current->next;
  }
}

force_inline void insertDrawableAtEndOfList(Drawable& drawable, Drawable*& drawableList)
{
  Drawable* current = drawableList;

  while(current)
  {
    if(current->next == nullptr)
    {
      // insert
      drawable.next = nullptr;
      drawable.removable = false;
      current->next = & drawable;
      return;
    }
    current = current->next;
  }
}

force_inline uint8_t computeDrawable(LevelContext& context, int16_t posX, Z_POSITION posZ, SpriteDefinition* sprite, const CarInfo& carInfo, Drawable*& drawableList, uint8_t index) noexcept
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
      drawable.yZoomPattern = 8;
    }
    else if(scale <= SCALE_FACTOR_QUARTER)
    {
      drawable.yZoomPattern = 4;
    }
    else if(scale <= SCALE_FACTOR_HALF)
    {
      drawable.yZoomPattern = 2;
    }
    else
    {
      drawable.yZoomPattern = 1;
    }
    drawable.zoomPattern = (scale + (1 <<11 )-1) >> (SCALE_FACTOR_SHIFT - 3);
    if(drawable.zoomPattern > 8)
    {
      drawable.zoomPattern = 8;
    }

    drawable.xStart = context.depthLevelToX[context.lineToDepthLevel[drawableLine]] - ((scale * posX) >> SCALE_FACTOR_SHIFT);
    
    uint32_t actualHeight = ((drawable.sprite->height * drawable.zoomPattern) >> 3);
    drawable.yStart = (drawableLine >= actualHeight) ? drawableLine - actualHeight : 0;
    drawable.yEnd = drawable.yStart + actualHeight - 1;

    insertDrawableIntoSortedList(drawable, drawableList);

    ++index;    
  }

  return index;
}

force_inline uint8_t computeDrawables(LevelContext& context, const CarInfo& carInfo, Drawable*& drawableList, const LevelConfig& config) noexcept
{
  uint8_t nextDrawableIndex = 0;

  Z_POSITION maxPos = carInfo.posZ + HORIZON_OFFSET;
  
  for(uint8_t index = 0; index < config.sceneryObjectsCount; ++index)
  {
    SceneryObject& object = context.sceneryObjects[index];
    if(object.posZ < maxPos)
    {
      nextDrawableIndex = computeDrawable(context,
                                          object.posX,
                                          object.posZ,
                                          object.sprite,
                                          carInfo,
                                          drawableList,
                                          nextDrawableIndex);
    }
  }

  for(uint8_t index = 0; index < config.staticObstaclesCount; ++index)
  {
    StaticObstacle& object = context.staticObstacles[index];
    if(object.posZ < maxPos)
    {
      nextDrawableIndex = computeDrawable(context,
                                          object.posX,
                                          object.posZ,
                                          object.sprite,
                                          carInfo,
                                          drawableList,
                                          nextDrawableIndex);
    }
  }

  for(uint8_t index = 0; index < config.movingObstaclesCount; ++index)
  {
    MovingObstacle& object = context.movingObstacles[index];
    if(object.posZ < maxPos)
    {
      nextDrawableIndex = computeDrawable(context,
                                          object.posX,
                                          object.posZ,
                                          object.sprite,
                                          carInfo,
                                          drawableList,
                                          nextDrawableIndex);
    }
  }

  {
    Jerrican& object = *(context.jerrican);
    if(object.visible && object.posZ < maxPos)
    {
      nextDrawableIndex = computeDrawable(context,
                                         object.posX,
                                         object.posZ,
                                         object.sprite,
                                         carInfo,
                                         drawableList,
                                         nextDrawableIndex);
    }
  }

    if(context.bonusStar)
    {
    BonusStar& object = *(context.bonusStar);
    if(object.visible && object.posZ < maxPos )
    {
      nextDrawableIndex = computeDrawable(context,
                                         object.posX,
                                         object.posZ,
                                         object.sprite,
                                         carInfo,
                                         drawableList,
                                         nextDrawableIndex);
    }
  }

 return nextDrawableIndex;
}

force_inline int8_t computeCollision(int16_t carXMin, int16_t carXMax, int16_t obstacleX, SpriteDefinition* obstacleSprite) noexcept
{
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

force_inline void updateCarInfo(const LevelContext& context, CarInfo& carInfo, const LevelConfig& config) noexcept
{
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
    
      if(unlikely(diffZ <= (1 << Z_POSITION_SHIFT)))
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

    // Bonus collisions
    {
      Jerrican& object = *(context.jerrican);
      diffZ = object.posZ-carInfo.posZ;
      if(diffZ < 0)
      {
        diffZ = -diffZ;
      }
    
      if(diffZ <= (4 << Z_POSITION_SHIFT) && object.visible)
      {
        int8_t currentCollision = computeCollision(carXMin, carXMax, object.posX, object.sprite);
        if(currentCollision)
        {
          object.visible = false;
          remainingFuel += (3000 << Z_POSITION_SHIFT);
          if(remainingFuel > MAX_FUEL)
            remainingFuel = MAX_FUEL;
          collisionFx = bonusSfx;
          collisionFxDuration = bonusSfxDuration;
          lightPattern = LIGHT_BONUS;
        }
      } // end jerrican

      if(context.bonusStar)
      {
        BonusStar& object = *(context.bonusStar);
        diffZ = object.posZ-carInfo.posZ;
        if(diffZ < 0)
        {
          diffZ = -diffZ;
        }
      
        if(diffZ <= (4 << Z_POSITION_SHIFT) && object.visible)
        {
          int8_t currentCollision = computeCollision(carXMin, carXMax, object.posX, object.sprite);
          if(currentCollision)
          {
            object.visible = false;
            ++ bonusCount;
            collisionFx = bonusSfx;
            collisionFxDuration = bonusSfxDuration;
            lightPattern = LIGHT_BONUS;
          }
        }
      } // end bonus star
    } // end bonus collisions
    
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
  else if(accelX < -0.3)
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
  
  if(collision && (collisionFxDuration == 0))
  {
    collisionFx = collisionSfx;
    collisionFxDuration = collisionSfxDuration;
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

  // Handle engine sound
  if(carInfo.engineFxDuration == 0)
  {
    carInfo.engineFx[0].period_start = 1000 - (carInfo.speedZ*500);
    carInfo.engineFxDuration = 8;
  }

  carInfo.posX += carInfo.speedX;
  carInfo.posZ += carInfo.speedZ * 256;
  remainingFuel -= carInfo.speedZ * 512;

  if(lightPattern)
  {
    carInfo.lights = lightPattern;
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
        if(swooshFxDuration == 0)
        {
          swooshFx = swoosh;
          swooshFxDuration = swooshDuration;
        }
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
    carInfo.fluxed = true;
    capacitorCharge = 0;
  }
}

force_inline void drawSprites(uint32_t y, uint16_t* stripLine,  /*unsigned int drawableCount*/Drawable*& drawableList, LevelContext& context) noexcept
{

  Drawable* current = drawableList;
    while(current)
    {
      if(y > current->yEnd && current->removable) // we'll never draw this one any more, pop from sorted list
      {
        drawableList = current;
      }
      else if(y >= current->yStart && y <= current->yEnd)
      {
        if(current->zoomPattern == 8)
        {
          // fast, no zoom
          const uint16_t* spriteBufferWithOffset = current->sprite->buffer + (current->sprite->width * ((y - current->yStart)));
          int32_t xIndex = current->xStart;
          int32_t coordInSprite = 0;
          if(xIndex < 0)
          {
            coordInSprite = -xIndex;
            xIndex = 0;
          }
          uint16_t color;
  
          for(; coordInSprite<current->sprite->width && xIndex < SCREEN_WIDTH; ++coordInSprite, ++xIndex)
          {
              color = (*spriteBufferWithOffset++);
              if(color != COLOR_565(0xFF, 0x00, 0xFF))
              {
                stripLine[xIndex] = color;
              }
          }
        }
        else
        { // with zoom
          const uint16_t* spriteBufferWithOffset = current->sprite->buffer + (current->sprite->width * ((y - current->yStart) * 8 / current->zoomPattern));
          int32_t xIndex = current->xStart;
          uint16_t color;
  
          for(int32_t coordInSprite = 0; coordInSprite<current->sprite->width; ++coordInSprite)
          {
            xIndex = current->xStart + ((coordInSprite * current->zoomPattern) >> 3);
            if(xIndex >= 0 && xIndex < SCREEN_WIDTH)
            {
              color = (spriteBufferWithOffset[coordInSprite]);
              if(color != COLOR_565(0xFF, 0x00, 0xFF))
              {
                stripLine[xIndex] = color;
              }
            }
          }
        } // end with zoom
      }
      current = current->next;
    }
}



force_inline void drawFrame(GraphicsManager& gm,
                            LevelContext& context,
                            //unsigned int drawableCount,
                            Drawable*& drawableList,
                            const CarInfo& carInfo,
                            int16_t backgroundShift,
                            GraphicsManager::TaskSet* taskSet
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
    uint32_t depthLevel = context.lineToDepthLevel[y];

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
        color32 = context.backgroundPalette[*colorIndexes];
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
      int32_t target = x - di.lineRoadBumperWidth;
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

    drawSprites(y, stripLine, drawableList, context);

    if(yStrip == STRIP_HEIGHT)
    {
      strip = gm.CommitStrip(taskSet);
      stripCursor = strip;
      yStrip = 0;
    }
  }


  gm.EndFrame();
}

force_inline void drawFrameSkyway(GraphicsManager& gm,
                                  LevelContext& context,
                                  //unsigned int drawableCount,
                                  Drawable*& drawableList,
                                  const CarInfo& carInfo,
                                  int16_t backgroundShift,
                                  GraphicsManager::TaskSet* taskSet
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
    uint32_t depthLevel = context.lineToDepthLevel[y];

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
        color32 = context.backgroundPalette[*colorIndexes];
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
      int32_t target = x - di.lineRoadBumperWidth;
      
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

    drawSprites(y, stripLine, /*drawableCount*/drawableList, context);

    if(yStrip == STRIP_HEIGHT)
    {
      strip = gm.CommitStrip(taskSet);
      stripCursor = strip;
      yStrip = 0;
    }
  }


  gm.EndFrame();
}

void titleLoop(const uint8_t* title, const uint32_t* palette, uint16_t width, uint16_t height, const Gamebuino_Meta::Sound_FX* music) noexcept
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
                color32 = palette[*colorIndexes];
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
                color32 = palette[*colorIndexes];
                (*stripCursor32++) = color32;
                ++colorIndexes;
              }
            }
            break;

            default:
            {
              for(uint16_t ii = 0; ii < SCREEN_WIDTH; ii+=2)
              {
                color32 = palette[*colorIndexes];
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
        strip = gm.CommitStrip(nullptr);
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
          if(music)
          {
            gb.sound.fx(music);
          }

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

void lightUpdateTask(void* carInfo)
{
    gb.lights.drawImage(0, 0, ((CarInfo*)carInfo)->lights);
}

void soundPlaybackTask(void* carInfo)
{
  if(collisionFx)
  {
    gb.sound.fx(collisionFx);
  }
  else
  {
    if(swooshFx)
    {
      gb.sound.fx(swooshFx);
    }
    /*if(((CarInfo*)carInfo)->engineFxDuration)
    {
      gb.sound.fx(((CarInfo*)carInfo)->engineFx);
    }*/
  }
  if(collisionFxDuration)
  {
    --collisionFxDuration;
  }
  if(swooshDuration)
  {
    --swooshDuration;
  }
  if(((CarInfo*)carInfo)->engineFxDuration)
  {
    --((CarInfo*)carInfo)->engineFxDuration;
  }
}

int gameLoop(LevelConfig& config) noexcept
{
  resetAlloc();
  uint16_t* strip1 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);
  uint16_t* strip2 = (uint16_t*) mphAlloc(STRIP_SIZE_BYTES);

  LevelContext context;
  context.depthLevels       = (DepthInfo*)        mphAlloc(DEPTH_LEVEL_COUNT * sizeof(DepthInfo));
  context.lineToDepthLevel  = (uint32_t*)         mphAlloc(SCREEN_HEIGHT * sizeof(uint32_t));
  context.depthLevelToX     = (int16_t*)          mphAlloc(DEPTH_LEVEL_COUNT * sizeof(int16_t));
  context.trackPalette      = (uint16_t*)         mphAlloc(2 * COLOR_TRACK_SIZE * sizeof(uint16_t));
  context.segments          = (RoadSegment*)      mphAlloc(3 * sizeof(RoadSegment));
  context.carSprites        = (SpriteDefinition*) mphAlloc(5 * sizeof(SpriteDefinition));
  context.fuelSprites       = (SpriteDefinition*) mphAlloc(2 * sizeof(SpriteDefinition));
  context.speedSprites      = (SpriteDefinition*) mphAlloc(5 * sizeof(SpriteDefinition));
  context.sprites           = (SpriteDefinition*) mphAlloc(MAX_SPRITES * sizeof(SpriteDefinition));
  context.sceneryObjects    = (SceneryObject*)    mphAlloc(MAX_SCENERY_OBJECTS * sizeof(SceneryObject));
  context.staticObstacles   = (StaticObstacle*)   mphAlloc(MAX_STATIC_OBSTACLES * sizeof(StaticObstacle));
  context.movingObstacles   = (MovingObstacle*)   mphAlloc(MAX_MOVING_OBSTACLES * sizeof(MovingObstacle));
  context.jerrican          = (Jerrican*)         mphAlloc(sizeof(Jerrican));
  context.bonusStar         = (BonusStar*)        mphAlloc(sizeof(BonusStar));
  context.drawables         = (Drawable*)         mphAlloc(MAX_DRAWABLES * sizeof(Drawable));
  context.remainingJerricans = 3;


  if(config.level == Level::Arizona)
  {
    context.sprites[0].width = CACTUS_WIDTH;
    context.sprites[0].height = CACTUS_HEIGHT;
    context.sprites[0].buffer = CACTUS;

    context.sprites[1].width = CACTUS2_WIDTH;
    context.sprites[1].height = CACTUS2_HEIGHT;
    context.sprites[1].buffer = CACTUS2;
  
    context.sprites[2].width = BUSH_WIDTH;
    context.sprites[2].height = BUSH_HEIGHT;
    context.sprites[2].buffer = BUSH;
  
    context.sprites[3].width = BOULDER_WIDTH;
    context.sprites[3].height = BOULDER_HEIGHT;
    context.sprites[3].buffer = BOULDER;

    context.sprites[4].width = BOULDER2_WIDTH;
    context.sprites[4].height = BOULDER2_HEIGHT;
    context.sprites[4].buffer = BOULDER2;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 5;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES;
    config.staticObstaclesIndexStart = 3;
    config.staticObstaclesIndexEnd = 5;
    config.movingObstaclesCount = 0;
    config.movingObstaclesIndexStart = 0;
    config.movingObstaclesIndexEnd = 0;
  }
  else if(config.level == Level::Suburb)
  {
    context.sprites[0].width = TREE_WIDTH;
    context.sprites[0].height = TREE_HEIGHT;
    context.sprites[0].buffer = TREE;

    context.sprites[1].width = TREE2_WIDTH;
    context.sprites[1].height = TREE2_HEIGHT;
    context.sprites[1].buffer = TREE2;
  
    context.sprites[2].width = BUSH_WIDTH;
    context.sprites[2].height = BUSH_HEIGHT;
    context.sprites[2].buffer = BUSH;
  
    context.sprites[3].width = CAR_SUBURB_WIDTH;
    context.sprites[3].height = CAR_SUBURB_HEIGHT;
    context.sprites[3].buffer = CAR_SUBURB;
    
    context.sprites[4].width = CAR_SUBURB2_WIDTH;
    context.sprites[4].height = CAR_SUBURB2_HEIGHT;
    context.sprites[4].buffer = CAR_SUBURB2;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = 0;
    config.staticObstaclesIndexStart = 0;
    config.staticObstaclesIndexEnd = 2;
    config.movingObstaclesCount = 2;
    config.movingObstaclesIndexStart = 3;
    config.movingObstaclesIndexEnd = 5;
  }
  else if(config.level == Level::Skyway)
  {
    context.sprites[0].width = LIGHTBULB_WIDTH;
    context.sprites[0].height = LIGHTBULB_HEIGHT;
    context.sprites[0].buffer = LIGHTBULB;
  
    context.sprites[1].width = POLICE_WIDTH;
    context.sprites[1].height = POLICE_HEIGHT;
    context.sprites[1].buffer = POLICE;

    context.sprites[2].width = CAR_SKYWAY_WIDTH;
    context.sprites[2].height = CAR_SKYWAY_HEIGHT;
    context.sprites[2].buffer = CAR_SKYWAY;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 1;
    config.staticObstaclesCount = 0;
    config.staticObstaclesIndexStart = 0;
    config.staticObstaclesIndexEnd = 2;
    config.movingObstaclesCount = 3;
    config.movingObstaclesIndexStart = 1;
    config.movingObstaclesIndexEnd = 3;
  }
  else // Bonus
  {
    context.sprites[0].width = CARROT_WIDTH;
    context.sprites[0].height = CARROT_HEIGHT;
    context.sprites[0].buffer = CARROT;
  
    context.sprites[1].width = RAINBOW_WIDTH;
    context.sprites[1].height = RAINBOW_HEIGHT;
    context.sprites[1].buffer = RAINBOW;
  
    context.sprites[2].width = PINPIN_STATIC_WIDTH;
    context.sprites[2].height = PINPIN_STATIC_HEIGHT;
    context.sprites[2].buffer = PINPIN_STATIC;

    context.sprites[3].width = PINPIN_CAR_WIDTH;
    context.sprites[3].height = PINPIN_CAR_HEIGHT;
    context.sprites[3].buffer = PINPIN_CAR;
    
    config.sceneryObjectsCount = MAX_SCENERY_OBJECTS / 2;
    config.sceneryObjectsIndexStart = 0;
    config.sceneryObjectsIndexEnd = 3;
    config.staticObstaclesCount = MAX_STATIC_OBSTACLES /2 ;
    config.staticObstaclesIndexStart = 2;
    config.staticObstaclesIndexEnd = 3;
    config.movingObstaclesCount = MAX_MOVING_OBSTACLES / 2;
    config.movingObstaclesIndexStart = 3;
    config.movingObstaclesIndexEnd = 4;
  }

  context.sprites[JERRICAN_SPRITE_INDEX].width = JERRICAN_WIDTH;
  context.sprites[JERRICAN_SPRITE_INDEX].height = JERRICAN_HEIGHT;
  context.sprites[JERRICAN_SPRITE_INDEX].buffer = JERRICAN;

  /*context.jerrican->posX = 0;
  context.jerrican->posZ = 0;
  context.jerrican->visible = false;
  context.jerrican->sprite = &(context.sprites[JERRICAN_SPRITE_INDEX]);*/

  context.sprites[BONUS_SPRITE_INDEX].width = BONUS_WIDTH;
  context.sprites[BONUS_SPRITE_INDEX].height = BONUS_HEIGHT;
  context.sprites[BONUS_SPRITE_INDEX].buffer = BONUS;

  /*context.bonusStar->posX = 0;
  context.bonusStar->posZ = 0;
  context.bonusStar->visible = false;
  context.bonusStar->sprite = &(context.sprites[BONUS_SPRITE_INDEX]);*/

  context.carSprites[0].width = CAR_WIDTH;
  context.carSprites[0].height = CAR_HEIGHT;
  context.carSprites[0].buffer = CAR;

  context.carSprites[1].width = CAR_LEFT_WIDTH;
  context.carSprites[1].height = CAR_LEFT_HEIGHT;
  context.carSprites[1].buffer = CAR_LEFT;

  context.carSprites[2].width = CAR_RIGHT_WIDTH;
  context.carSprites[2].height = CAR_RIGHT_HEIGHT;
  context.carSprites[2].buffer = CAR_RIGHT;

  context.carSprites[3].width = SMOKE_SMALL_1_WIDTH;
  context.carSprites[3].height = SMOKE_SMALL_1_HEIGHT;
  context.carSprites[3].buffer = SMOKE_SMALL_1;

  context.carSprites[4].width = SMOKE_SMALL_2_WIDTH;
  context.carSprites[4].height = SMOKE_SMALL_2_HEIGHT;
  context.carSprites[4].buffer = SMOKE_SMALL_2;

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
      context.background = BACKGROUND_WILDWEST;
      context.backgroundPalette = BACKGROUND_WILDWEST_PALETTE;
      context.backgroundHeight = BACKGROUND_WILDWEST_HEIGHT;
      break;

    case Level::Suburb:
      context.background = BACKGROUND_SUBURB;
      context.backgroundPalette = BACKGROUND_SUBURB_PALETTE;
      context.backgroundHeight = BACKGROUND_SUBURB_HEIGHT;
      break;

    case Level::Skyway:
      context.background = BACKGROUND_SKYWAY;
      context.backgroundPalette = BACKGROUND_SKYWAY_PALETTE;
      context.backgroundHeight = BACKGROUND_SKYWAY_HEIGHT;
      break;

    default:
      context.background = BACKGROUND_PINPIN;
      context.backgroundPalette = BACKGROUND_PINPIN_PALETTE;
      context.backgroundHeight = BACKGROUND_PINPIN_HEIGHT;
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
  carInfo.engineFx[0].type = Gamebuino_Meta::Sound_FX_Wave::SQUARE;
  carInfo.engineFx[0].continue_flag = 0;
  carInfo.engineFx[0].volume_start = 100;
  carInfo.engineFx[0].volume_sweep = 0;
  carInfo.engineFx[0].period_sweep = 0;
  carInfo.engineFx[0].period_start = 1000;
  carInfo.engineFx[0].length = 10;
  carInfo.engineFxDuration = 0;

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
  createJerrican(context, *(context.jerrican), 0, config);
  if(config.level == Level::Bonus)
  {
    context.bonusStar = nullptr;
  }
  else
  {
    createBonusStar(context, *(context.bonusStar), config);
  }
  
  int16_t backgroundShift = 0; /* sign.11.4 */

  capacitorCharge = 0;

  GraphicsManager::TaskSet taskSet;
  taskSet.tasks = (GraphicsManager::Task*)malloc(2 * sizeof(GraphicsManager::Task));
  taskSet.tasks[0].function = &soundPlaybackTask;
  taskSet.tasks[0].param = &carInfo;
  taskSet.tasks[1].function = &lightUpdateTask;
  taskSet.tasks[1].param = &carInfo;
  taskSet.taskCount = 2;
  
  while(true)
  { 
    while (!gb.update());
    taskSet.currentTask = 0;
    collisionFx = nullptr;
    swooshFx = nullptr;

    updateCarInfo(context, carInfo, config);

// Update environment wrt to new car position

    if(unlikely(carInfo.posZ > context.segments[1].segmentStartZ))
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
    updateJerrican(context, carInfo, config);
    updateBonusStar(context, carInfo, config);

Drawable* drawableList = nullptr;
    uint8_t drawableCount = computeDrawables(context, carInfo, drawableList, config);

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
    drawable.zoomPattern = 8;
    drawable.yZoomPattern = 1;
    insertDrawableIntoSortedList(drawable, drawableList);
  }
  ++drawableCount;

  if(carInfo.speedZ > 0) // dust
  {
    uint32_t spriteVersion = ((gb.frameCount & 0x2) >> 1);
    uint16_t xZoomPattern = 1;
    uint16_t yZoomPattern = 3;
    if(carInfo.speedZ > 0.6)
    {
      if(carInfo.speedZ > 1.)
      {
        xZoomPattern = 8;
        yZoomPattern = 0;
      }
      else
      {
        xZoomPattern = 4;
        yZoomPattern = 1;
      }
    }
    else if(carInfo.speedZ > 0.3)
    {
      xZoomPattern = 2;
      yZoomPattern = 2;
    }

    
    {    
      Drawable& drawable = context.drawables[drawableCount];
      drawable.sprite = &context.carSprites[3 + spriteVersion];
    
      drawable.xStart = SCREEN_WIDTH/2 + 15 - ((drawable.sprite->width * xZoomPattern ) >> 4);
      drawable.yStart = 122 - (drawable.sprite->height >> yZoomPattern);
      if(carInfo.sprite == CarSprite::Left)
      {
        drawable.xStart += 2;
        drawable.yStart -= 2;
      }
      drawable.yEnd = drawable.yStart + (drawable.sprite->height >> yZoomPattern) - 1;
      drawable.zoomPattern = xZoomPattern;
      drawable.yZoomPattern = (1 << yZoomPattern);
      insertDrawableIntoSortedList(drawable, drawableList);
      ++drawableCount;
    }

    {
      Drawable& drawable = context.drawables[drawableCount];
      drawable.sprite = &context.carSprites[4 - spriteVersion];
      
      drawable.xStart = SCREEN_WIDTH/2 - 17 - ((drawable.sprite->width * xZoomPattern ) >> 4);
      drawable.yStart = 122 - (drawable.sprite->height >> yZoomPattern);
      if(carInfo.sprite == CarSprite::Right)
      {
        drawable.xStart += 2;
        drawable.yStart -= 2;
      }
      drawable.yEnd = drawable.yStart + (drawable.sprite->height >> yZoomPattern) - 1;
      drawable.zoomPattern = xZoomPattern;
      drawable.yZoomPattern = (1 << yZoomPattern);
      insertDrawableIntoSortedList(drawable, drawableList);
      ++drawableCount;
    }
  } // end dust

  {
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = carInfo.fluxSprite;
    drawable.xStart = SCREEN_WIDTH - drawable.sprite->width;
    drawable.yStart = 0;//SCREEN_HEIGHT - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 8;
    drawable.yZoomPattern = 1;
    insertDrawableAtEndOfList(drawable, drawableList);
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
    drawable.zoomPattern = 8;
    drawable.yZoomPattern = 1;
    insertDrawableAtEndOfList(drawable, drawableList);
  }
  ++drawableCount;

    {
    Drawable& drawable = context.drawables[drawableCount];
    drawable.sprite = &context.fuelSprites[0];
    drawable.xStart = 0;
    drawable.yStart = 0;//SCREEN_HEIGHT - drawable.sprite->height;
    drawable.yEnd = drawable.yStart + drawable.sprite->height - 1;
    drawable.zoomPattern = 8;
    drawable.yZoomPattern = 1;
    insertDrawableAtEndOfList(drawable, drawableList);
  }
  ++drawableCount;



      {
        uint16_t fuelOffset = ((MAX_FUEL - remainingFuel) >> 17);
        if(fuelOffset < context.fuelSprites[1].height - 4 || ((gb.frameCount >> 4) & 0x01)) // blink
        {
          Drawable& drawable = context.drawables[drawableCount];
          drawable.sprite = &context.fuelSprites[1];
          drawable.xStart = 0;
          drawable.yStart = 3;
          drawable.yEnd = drawable.yStart + fuelOffset - 1;
          drawable.zoomPattern = 8;
          drawable.yZoomPattern = 1;

          ++drawableCount;

          if(fuelOffset >= drawable.sprite->height)
          {
            free(taskSet.tasks);
            return 1; // game over
          }
          
          insertDrawableAtEndOfList(drawable, drawableList);
        }
      

    }

  if(config.level == Level::Skyway)
  {
    drawFrameSkyway(gm, context, drawableList, carInfo, backgroundShift, &taskSet);
  }
  else
  {
    drawFrame(gm, context, drawableList, carInfo, backgroundShift, &taskSet);
  }

  
  if(carInfo.fluxed)
  {
    free(taskSet.tasks);
    return 0;
  }
  
    /*if(gb.buttons.repeat(BUTTON_MENU, 0))
    {
      SerialUSB.printf("CPU: %i\n", gb.getCpuLoad());
      SerialUSB.printf("MEM: %i\n", gb.getFreeRam());
      SerialUSB.printf("REMAIN: %i\n", allocFreeRam());
    }*/
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
    SPEEDOMETER_Y[index] = (28- SPEED_HAND_CENTER_Y + DOT_HEIGHT/2 - SPEED_HAND_LENGTH * sin(angle) + 0.5);
  }

  SerialUSB.begin(9600);

  nextAvailableSegment = memory;
}

int runLevel(LevelConfig& config) noexcept
{
    const uint32_t * palette;
    uint16_t width, height;
    const uint8_t* title;
  
    switch(config.level)
    {
      case Level::Arizona:
        palette = TITLE_WILDWEST_PALETTE;
        width = TITLE_WILDWEST_WIDTH;
        height = TITLE_WILDWEST_HEIGHT;
        title = TITLE_WILDWEST;
        break;
  
      case Level::Suburb:
        palette = TITLE_SUBURB_PALETTE;
        width = TITLE_SUBURB_WIDTH;
        height = TITLE_SUBURB_HEIGHT;
        title = TITLE_SUBURB;
        break;

      case Level::Skyway:
        palette = TITLE_SKYWAY_PALETTE;
        width = TITLE_SKYWAY_WIDTH;
        height = TITLE_SKYWAY_HEIGHT;
        title = TITLE_SKYWAY;
        break;
  
      default:
        palette = TITLE_PINPIN_PALETTE;
        width = TITLE_PINPIN_WIDTH;
        height = TITLE_PINPIN_HEIGHT;
        title = TITLE_PINPIN;
        break;
    }
  
    titleLoop(title, palette, width, height, titleJingle);

  return gameLoop(config);
}

void loop()
{
  while(true)
  {
    titleLoop(TITLE, TITLE_PALETTE, TITLE_WIDTH, TITLE_HEIGHT, titleMusic);

    remainingFuel = MAX_FUEL;
    bonusCount = 0;
    
    LevelConfig config;
    if(gb.buttons.repeat(BUTTON_UP, 0) && gb.buttons.repeat(BUTTON_B,0))
    {
      config = levelSelectionMenu(Level::Suburb);
    }
    else if(gb.buttons.repeat(BUTTON_DOWN, 0) && gb.buttons.repeat(BUTTON_B,0))
    {
      config = levelSelectionMenu(Level::Skyway);
    }
    else if(gb.buttons.repeat(BUTTON_RIGHT, 0) && gb.buttons.repeat(BUTTON_B,0))
    {
      config = levelSelectionMenu(Level::Bonus);
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
          titleLoop(SUCCESS, SUCCESS_PALETTE, SUCCESS_WIDTH, SUCCESS_HEIGHT, successMusic);
          if(bonusCount == 3)
          {
            config = levelSelectionMenu(Level::Bonus);
            runLevel(config);
          }
        }
        else
        {
          titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT, gameOverMusic);
        }
      }
      else
      {
        titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT, gameOverMusic);
      }
    }
    else
    {
      titleLoop(GAME_OVER, GAME_OVER_PALETTE, GAME_OVER_WIDTH, GAME_OVER_HEIGHT, gameOverMusic);
    }
      
  }
}
