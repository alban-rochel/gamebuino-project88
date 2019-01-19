const Gamebuino_Meta::Sound_FX collisionSfx[] = {
  {Gamebuino_Meta::Sound_FX_Wave::NOISE,0,51,0,0,48,10},
}; /* 200 ms = 8 frames */

uint8_t collisionSfxDuration = 8; 

const Gamebuino_Meta::Sound_FX engineLow[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,100,0,0,1000,10}
}; /* 200 ms = 8 frames */

uint8_t engineLowDuration = 8; 

const Gamebuino_Meta::Sound_FX engineMid[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,150,0,0,900,10}
}; /* 200 ms = 8 frames */

uint8_t engineMidDuration = 8;

const Gamebuino_Meta::Sound_FX engineHigh[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,200,0,0,800,10}
}; /* 40 ms = 1.6 frame */

uint8_t engineHighDuration = 8;
