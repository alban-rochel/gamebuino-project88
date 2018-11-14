#pragma once

#include <Gamebuino-Meta.h>

namespace Gamebuino_Meta
{
  static SPISettings tftSPISettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);
};

#define COLOR_R(r) ((uint16_t)(r & 0xF8))
#define COLOR_G_LOW(g) ((uint16_t)(g) << 13)
#define COLOR_G_HIGH(g) ((uint16_t)(g) >> 5)
#define COLOR_B(b) ((uint16_t)(b & 0xF8) << 5)

#define COLOR_565(r, g, b) (COLOR_R(r) | COLOR_G_LOW(g) | COLOR_G_HIGH(g) | COLOR_B(b))

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128
#define STRIP_HEIGHT 8
#define STRIP_SIZE_PIX (SCREEN_WIDTH * STRIP_HEIGHT)
#define STRIP_SIZE_BYTES (STRIP_SIZE_PIX * 2)

//#define FIXED_8_8 uint16_t
#define Z_POSITION int32_t /* sign.23.8 */
#define Z_POSITION_SHIFT 8

#define ROAD_CURVATURE_X int8_t /* sign.3.4 */
#define ROAD_CURVATURE_X_SHIFT 4

namespace roads
{
  struct LevelConfig
  {
    uint8_t bumperWidth;
    uint8_t roadWidth;
    uint8_t lineWidth;
  };
}

#define Z_E (-5.f)
#define Y_E_METERS (5.f)
#define DEPTH_LEVEL_COUNT (80)

#define SKY_Z (0xFF)

#define COLOR_TRACK_GRASS_INDEX 0
#define COLOR_TRACK_BUMPER_INDEX (COLOR_TRACK_GRASS_INDEX+1)
#define COLOR_TRACK_ROAD_INDEX (COLOR_TRACK_BUMPER_INDEX+1)
#define COLOR_TRACK_LINE_INDEX (COLOR_TRACK_ROAD_INDEX+1)
#define COLOR_TRACK_SIZE (COLOR_TRACK_LINE_INDEX)

#define MAX_SPRITES 10

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

struct CarInfo
{
  int16_t posX; // 0 is the center of the road
  //float posZ;
  Z_POSITION posZ;
  float speed;
};

struct DepthInfo
{
  Z_POSITION z;
  float scaleFactor = 0;        // TODO 8.8
  int16_t lineWidth = 0;  // TODO int8_t
  int16_t lineRoadWidth = 0;    // TODO int8_t
  int16_t lineRoadBumperWidth = 0;    // TODO int8_t
};

struct RoadSegment
{
  ROAD_CURVATURE_X xCurvature;
  /*float*/ int16_t zCurvature;
  Z_POSITION segmentStartZ;
};

struct SpriteProgram
{
  uint8_t xStart;
  uint8_t yStart;
  uint8_t width;
  uint8_t yEnd;
  const uint16_t* buffer;  
};
