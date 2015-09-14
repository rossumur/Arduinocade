
#ifndef __ARUINOCADE__H__
#define __ARUINOCADE__H__

// If you want to build BALLBLASTER
//#define BALLBLASTER_GAME

// PICK A CONTROLLER
#define ATARI_FLASHBACK
//#define RETCON
//#define APPLE_TV

#ifdef ARDUINO
#include <Arduino.h>
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#define SPRITEPTR const uint8_t*
#define GET_SPRITE(_n) _sprite_ptrs[_n]

#else   // Building in simulator
#define PROGMEM
#define prog_char char
#define pgm_read_byte(_x) *(_x)
#define pgm_read_word(_x) *(_x)
#define strcpy_P strcpy
#define memcpy_P memcpy
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <unistd.h>

#define SPRITEPTR const uint8_t*
#define GET_SPRITE(_n) (const uint8_t*)_sprite_ptrs[_n]

#endif

// joystick bits

#define FIRE_X  0x800 
#define FIRE_Y  0x400 
#define FIRE_Z  0x200

#define FIRE_C  0x080 // also START
#define FIRE_B  0x040 // also SELECT
#define FIRE_A  0x020 // also FIRE

#define RESET 	0x100
#define START 	0x080
#define SELECT 	0x040
#define FIRE 	0x020
#define RIGHT 	0x010
#define LEFT 	0x008
#define DOWN 	0x004
#define UP      0x002
#define MENU    0x001

uint16_t* readJoy();
extern uint8_t v_framecounter;
extern int video_current_line();
void flush_video(bool cb);

class Game
{
public:
    Game();
    virtual void init()=0;
    virtual void step()=0;
    virtual void draw() = 0;    // loop?
    virtual uint8_t get_tile(uint8_t x, uint8_t y)=0;
};

void video_start();     // TODO: Game member?
Game* current_game();   // TODO: Game member?

#endif
