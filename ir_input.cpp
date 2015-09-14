
/* Copyright (c) 2010-2014, Peter Barrett
 **
 ** Permission to use, copy, modify, and/or distribute this software for
 ** any purpose with or without fee is hereby granted, provided that the
 ** above copyright notice and this permission notice appear in all copies.
 **
 ** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 ** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 ** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 ** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 ** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 ** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 ** SOFTWARE.
 */

#include "config.h"

uint8_t readKey();
void keyboardIREvent(uint8_t t, uint8_t v); // t is HSYNCH ticks, v is value

// IR Keyboard State
uint8_t _modifiers = 0;
uint8_t _keyDown = 0;
uint8_t _keyDownLast = 0;
uint8_t _keyUp = 0;

// shared
uint8_t _state = 0;
uint16_t _code = 0;

// maintain a real keystate
uint16_t _key16 = 0;
uint16_t _joy[2]= {0};
uint8_t _joy_timer[2] = {0};

extern uint8_t _dbg[4];  // Debug counters

//==========================================================
//==========================================================
// Crappy ebay remote
// NEC codes

PROGMEM
const uint8_t _remotez[] = {
    0xB2,0x2A,0x68, // power,source,mute
    0x32,0xA0,0x30, // record, up/ch+, timeshift
    0x50,0x02,0x78, // left/vol-,center/fullscreen,right/vol+
    0x48,0x40,0x38, // 0,down/ch-,recall
    0x90,0xB8,0xF8, // 1,2,3
    0xB0,0x98,0xD8, // 4,5,6
    0x88,0xA8,0xE8  // 7,8,9
};
  
//==========================================================
//==========================================================
// Apple remote NEC code

#ifdef APPLE_TV

// Silver apple remote, 7 Bit code
// should work with both silvers and white
#define APPLE_MENU 	0x40
#define APPLE_PLAY 	0x7A
#define APPLE_CENTER 	0x3A
#define APPLE_RIGHT 	0x60
#define APPLE_LEFT 	0x10
#define APPLE_UP 	0x50
#define APPLE_DOWN 	0x30

#define APPLE_RELEASE   0x20 // sent after menu and play?

//	generic repeat code
#define NEC_REPEAT	0xAAAA

/*
  9ms preamble ~142
  4.5ms 1 ~71 - start
  2.25ms ~35 - repeat
  
  32 bits
  0 – a 562.5µs/562.5µs   9ish wide
  1 – a 562.5µs/1.6875ms  27ish wide
*/

uint16_t* readJoy()
{
  if (_key16)
  {
    if (_key16 != NEC_REPEAT)
      _keyDown = (_key16 >> 8) & 0x7F;  // 7 bit code
    uint16_t k = 0;
    switch (_keyDown) {
        case APPLE_UP:     k = UP;     break;
        case APPLE_DOWN:   k = DOWN;   break;
        case APPLE_LEFT:   k = LEFT;   break;
        case APPLE_RIGHT:  k = RIGHT;  break;
        case APPLE_CENTER: k = FIRE;   break;
        case APPLE_MENU:   k = RESET;  break;
        case APPLE_PLAY:   k = SELECT; break;
    }
    _joy[0] = k;
    _joy_timer[0] = 8; // 108ms repeat period
    _key16 = 0;
  } else {
    // timeouts. In theory could support 2 remotes by rolling ID (play+menu)
    if (_joy_timer[0] && !--_joy_timer[0])
      _joy[0] = 0;
  }
  return _joy;
}

// NEC codes used by apple remote
void keyboardIREvent(uint8_t t, uint8_t v)
{
  if (!v) {
    if (t > 32)
      _state = 0;
  } else {
    if (t < 32)
    {
      _code <<= 1;
      if (t >= 12)
        _code |= 1;
      if (++_state == 32)
        _key16 = _code;    // Apple code in bits 14-8*
    } else {
       if (t > 32 && t < 40 && !_state)  // Repeat 2.25ms pulse 4.5ms start
          _key16 = NEC_REPEAT;
        _state = 0;
    }
  }
}

#endif

//==========================================================
//==========================================================
//  Atari Flashback 4 wireless controllers

#ifdef ATARI_FLASHBACK

// HSYNCH period is 44/315*455 or 63.55555..us
// 18 bit code 1.87khz clock
// 2.3ms zero preamble  // 36
// 0 is 0.27ms pulse    // 4
// 1 is 0.80ms pulse    // 13

// Keycodes...
// Leading bit is 1 for player 1 control..

#define PREAMBLE(_t) (_t >= 34 && _t <= 38)
#define SHORT(_t)    (_t >= 2 && _t <= 6)
#define LONG(_t)     (_t >= 11 && _t <= 15)

// Codes come in pairs 33ms apart
// Sequence repeats every 133ms
// bitmap is released if no code for 10 vbls (167ms) or 0x01 (p1) / 0x0F (p2)
// up to 12 button bits, 4 bits of csum/p1/p2 indication

// readJoy called 1 every loop ~60Hz
uint16_t* readJoy()
{
  uint8_t k = 0;
  if (_key16)
  {
    uint16_t m = _key16 >> 4;        // 12 bits of buttons
    uint8_t csum = _key16 & 0xF;     // csum+1 for p1, csum-1 for p2
    uint8_t s = m + (m >> 4) + (m >> 8);
    if (((s+1) & 0xF) == csum)
    {
      _joy[0] = m;
      _joy_timer[0] = 10;
    } else if (((s-1) & 0xF) == csum) {
      _joy[1] = m;
      _joy_timer[1] = 10;
    } //else bad checksum 
    _key16 = 0;
  } else {
    // timeouts
    if (_joy_timer[0] && !--_joy_timer[0])
      _joy[0] = 0;
    if (_joy_timer[1] && !--_joy_timer[1])
      _joy[1] = 0;
  }
  return _joy;
}

void keyboardIREvent(uint8_t t, uint8_t v)
{
  if (_state == 0)
  {
    if (PREAMBLE(t) && (v == 0))  // long 0, rising edge of start bit
    {
      _state = 1;
    }
  }
  else
  {
    if (v)
    {
      _code <<= 1;
      if (LONG(t))
      {
        _code |= 1;
      }
      else if (!SHORT(t))
      {
        _state = 0;  // framing error
        return;
      }
      
      if (++_state == 19)
      {
        _key16 = _code;
        _state = 0;
      }
    }
    else
    {
      if (!SHORT(t))
        _state = 0;  // Framing err
    }
  }
}

#endif



//==========================================================
//==========================================================
// RETCON controllers
// 75ms keyboard repeat
// Preamble is 0.80ms low, 0.5 high
// Low: 0.57ms=0,0.27,s=1, high 0.37
// 16 bits
// Preamble = 800,500/63.55555 ~ 12.6,7.87
// LOW0 = 8.97
// LOW1 = 4.25
// HIGH = 5.82

#ifdef RETCON

// number of 63.55555 cycles per bit
#define PREAMBLE_L(_t) (_t >= 12 && _t <= 14) // 12/13/14 preamble
#define PREAMBLE_H(_t) (_t >= 6 && _t <= 10)  // 8
#define LOW_0(_t)     (_t >= 8 && _t <= 10)   // 8/9/10
#define LOW_1(_t)     (_t >= 4 && _t <= 6)    // 4/5/6

__attribute__((section(".progmem.data")))
const uint16_t _jmap[] = {
  0x0400, UP,
  0x0200, DOWN,
  0x0100, LEFT,
  0x0080, RIGHT,
  
  0x1000, SELECT,
  0x0800, RESET, // menu
  
  0x0020, FIRE_X,
  0x0040, FIRE_Y,
  0x0002, FIRE_Z,
  
  0x2000, FIRE_A,
  0x4000, FIRE_B,
  0x0008, FIRE_C,
};

uint16_t* readJoy()
{
  if (_key16)
  {
    uint16_t m = 0;
    const uint16_t* jmap = _jmap;
    int16_t i = 12;
    while (i--)
    {
      if (pgm_read_word(jmap) & _key16)
         m |= pgm_read_word(jmap+1);
      jmap += 2;
    }
    _key16 = 0;
    if (_key16 & 0x8000)  // p2
    {
      _joy[1] = m;
      _joy_timer[1] = 6;
    } else {
      _joy[0] = m;
      _joy_timer[0] = 6;
    }
  } else {
    // timeouts
    if (_joy_timer[0] && !--_joy_timer[0])
      _joy[0] = 0;
    if (_joy_timer[1] && !--_joy_timer[1])
      _joy[1] = 0;
  }
  return _joy;
}

void keyboardIREvent(uint8_t t, uint8_t v)
{  
  if (_state == 0)
  {
    if (v == 0)  {   // start bit
      if (PREAMBLE_L(t))
        _state = 1;
      else
         _dbg[0]++;  // framing error
    }
  }
  else
  {
    if (!v) 
    {
      _code <<= 1;
      if (LOW_1(t))
        _code |= 1;
      if (_state++ == 16)
      {
        _key16 = _code;
        _dbg[1]++;  // count (repeat rate)
        _dbg[2] = _code >> 8;
        _dbg[3] = _code;
        _state = 0;
      }
    }
  }
}

#endif


//==========================================================
//==========================================================
//  Webtv keyboard
#if WEBTV_KEYBOARD

#define BAUDB   12  // Width of uart bit in HSYNCH
#define PREAMBLE(_t) (_t >= 36 && _t <= 40)   // 3.25 baud
#define SHORTBIT(_t) (_t >= 9 && _t <= 13)     // 1.5ms ish

#define MAX_KEY_REPEAT 8         // ~130ms in vbi
extern uint8_t v_vbicountdown;   // Some of these keyboards seem to have a very fast key repeat

// missing some keys, plenty for zork
__attribute__((section(".progmem.data")))
const prog_char _keymap[]  =
{
  // keys
  0x00,0x00,0x62,0x00,0x00,0x1D,0x00,0x00,0x00,0x1E,0x00,0x2F,0x00,0x1F,0x20,0x6E,
  0x60,0x00,0x35,0x00,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x36,
  0x00,0x00,0x76,0x2E,0x63,0x00,0x00,0x2C,0x78,0x00,0x00,0x00,0x7A,0x00,0x0A,0x6D,
  0x00,0x00,0x66,0x6C,0x64,0x00,0x00,0x6B,0x73,0x00,0x00,0x3B,0x61,0x00,0x5C,0x6A,
  0x00,0x00,0x74,0x00,0x00,0x00,0x00,0x5D,0x00,0x00,0x00,0x5B,0x09,0x00,0x08,0x79,
  0x00,0x00,0x34,0x39,0x33,0x00,0x00,0x38,0x32,0x00,0x00,0x30,0x31,0x00,0x00,0x37,
  0x00,0x00,0x67,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x27,0x00,0x00,0x00,0x68,
  0x00,0x00,0x72,0x6F,0x65,0x00,0x00,0x69,0x77,0x00,0x00,0x70,0x71,0x00,0x00,0x75,
  // shifted
  0x00,0x00,0x42,0x00,0x00,0x1D,0x00,0x00,0x00,0x1E,0x00,0x3F,0x00,0x1F,0x20,0x4E,
  0x7E,0x00,0x25,0x00,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x5F,0x00,0x00,0x00,0x5E,
  0x00,0x00,0x56,0x3E,0x43,0x00,0x00,0x3C,0x58,0x00,0x00,0x00,0x5A,0x00,0x0A,0x4D,
  0x00,0x00,0x46,0x4C,0x44,0x00,0x00,0x4B,0x53,0x00,0x00,0x3A,0x41,0x00,0x7C,0x4A,
  0x00,0x00,0x54,0x00,0x00,0x00,0x00,0x7D,0x00,0x00,0x00,0x7B,0x09,0x00,0x08,0x59,
  0x00,0x00,0x24,0x28,0x23,0x00,0x00,0x2A,0x40,0x00,0x00,0x29,0x21,0x00,0x00,0x26,
  0x00,0x00,0x47,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x22,0x00,0x00,0x00,0x48,
  0x00,0x00,0x52,0x4F,0x45,0x00,0x00,0x49,0x57,0x00,0x00,0x50,0x51,0x00,0x00,0x55,
};

#define KEYDOWN     0x4A
#define KEYUP       0x5E
#define SHIFT_LEFT  0x8C
#define SHIFT_RIGHT 0x4C

// 
uint8_t ps2_read_key();

uint8_t readKey()
{
  uint8_t k;
  k = ps2_read_key();
  if (k)
    return k;
    
  k = _keyUp;
  if (k) {
    _keyUp = 0;
    switch (k) {
      case SHIFT_LEFT:
      case SHIFT_RIGHT:
        _modifiers &= ~0x80;
        break;
    }
  }
  
  k = _keyDown;
  if (k)
  {
    _keyDown = 0;
    _keyDownLast = k;
    switch (k) {
      case SHIFT_LEFT:
      case SHIFT_RIGHT:
        _modifiers |= 0x80;
        return 0;
    }
    // Check Parity
    uint8_t c;
    uint8_t v = k;
    for (c = 0; v; c++)
      v &= v-1;
    if (c & 1)
    {
      k = pgm_read_byte(_keymap + _modifiers + (((uint8_t)k) >> 1));
      return k;
    }
  }
  return 0;
}

// WebTV UART like keyboard protocol
// 3.25 bit 0 start preamble the 19 bits
// 10 bit code for keyup,keydown, all keys released etc
// 8 bit keycode
// parity bit.
void keyboardIREvent(uint8_t t, uint8_t v)
{
  if (_state == 0)
  {
    if (PREAMBLE(t) && (v == 0))  // long 0, rising edge of start bit
      _state = 1;
  }
  else if (_state == 1)
  {
    _state = (SHORTBIT(t) && (v == 1)) ? 2 : 0;
  }
  else 
  {
      t += BAUDB>>1;
      uint8_t bits = _state-2;
      while ((t > BAUDB) && (bits < 16))
      {
          t -= BAUDB;
          _code = (_code << 1) | v;
          bits++;
      }
      if (bits == 16)
      {
        v = t <= BAUDB;
        uint8_t md = _code >> 8;
        _code |= v;   // Low bit is parity
        if (md == KEYDOWN)
        {
          if ((_keyDownLast != (uint8_t)_code) || (v_vbicountdown == 0)) // Limit key repeat rate
          {
            _keyDown = _code;
            v_vbicountdown = MAX_KEY_REPEAT;
          } else {
            v_vbicountdown = 0;
          }
        }
        else if (md == KEYUP)
        {
          _keyUp = _code;
        }
        _state = 0;
        return;
      }
      _state = bits+2;
    }
}

#endif
