
/* Copyright (c) 2010, Peter Barrett
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
#include "zdVideo.h"

//#define DOOMED
#ifdef DOOMED

#ifndef u8
typedef unsigned char u8;
typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned short ushort;
typedef unsigned long u32;
#endif

extern
volatile const uint8_t* v_rle;

#ifndef ARDUINO
class Serial_
{
public:
    void print(int n)
    {
        printf("%d",n);
    }
    void println(int n)
    {
        print(n);
        print("\n");
    }
    void print(const char*c)
    {
        printf(c);
    }
    void println(const char* c)
    {
        print(c);
        print("\n");
    }
};
Serial_ Serial;
#endif

//#define HIRES

#ifdef HIRES
#define SCREEN_WIDTH 320
#else
#define SCREEN_WIDTH 144
#endif
#define SCREEN_HEIGHT 192

PROGMEM
const uint8_t _mask2[] = { 0xF0,0x0F };
PROGMEM
const uint8_t _mask4[] = { 0xC0,0x30,0x0C,0x03 };

extern const uint8_t _trig[64] PROGMEM;
const uint8_t _trig[64] =
{
    0,6,13,19,25,31,37,44,
    50,56,62,68,74,80,86,92,
    98,103,109,115,120,126,131,136,
    142,147,152,157,162,167,171,176,
    180,185,189,193,197,201,205,208,
    212,215,219,222,225,228,231,233,
    236,238,240,242,244,246,247,249,
    250,251,252,253,254,254,255,255
};

short SIN(uint8_t angle)
{
    if ((angle & 0x7F) == 0)
        return 0;
    uint8_t b = angle & 0x3F;
    if (angle & 0x40)
        b = 0x3F - b;
    int i = pgm_read_byte(_trig+b) + 1;
    if (angle & 0x80)
        return -i;
    return i;
}

short COS(uint8_t angle)
{
    return SIN(angle + 64);
}

#if 0
#define _S 1    // Steel Wall
#define _W 2    // Wood wall
#define WD 3    // wood door
#define _H 4    // Hex brick wall

#define _C 5    // Clinker brick wall
#define _A 5    // Ammo
#define _B 7    // Blue wall
#define SD 6    // Steel door
#endif

#define FLOOR 0x00

#define _S (0x44^FLOOR)    // Steel Wall
#define _W (0x11^FLOOR)    // Wood wall
#define WD (0x33^FLOOR)    // wood door
#define _H (0x22^FLOOR)    // Hex brick wall

#define _C (0x66^FLOOR)    // Clinker brick wall
#define _A (0x77^FLOOR)    // Ammo
#define _B (0xCC^FLOOR)    // Blue wall
#define SD (0xFF^FLOOR)    // Steel door

extern const u8 _map[64] PROGMEM;
const u8 _map[64] =
{
    _S,_S,_S,_S,_A,_W,_W,_W,
    SD, 0, 0,_S, 0, 0, 0,WD,
    _S,_S, 0,_S, 0,_W,_W,_W,
    _A, 0, 0, 0, 0, 0, 0,_C,
    _H,_B,_B, 0,_B,_B, 0,_C,
    _H, 0, 0, 0,_B,_C, 0,WD,
    WD, 0, 0, 0, 0,SD, 0,_C,
    _H,_H,_H,_H,_H,_C,_C,_C,
};

//  Range if uv is 0..2 in 16:16
//  Always positive

//  Not all that accurate, but fast
long multfix(long a, long b)
{
    u16 aa = a >> 8;
    return aa*b >> 8;
}

long multfix16(u16 a16, long b24)
{
    b24 >>= 8;
    return (a16*b24)>>8;
}

long reciprocalfix(u16 d)
{
    u8 count = 0;
    while (((short)d) > 0)
    {
        ++count;
        d <<= 1;
    }
    
    // Single n-r iteration
    long x = 0x02EA00 - d;
    x -= d;
    x = multfix(x, 0x20000L-multfix16(d,x));
    
    if (count>0)
        x = x << count;
    return x;
}

//  Range if uv is 0..2 in 16:16
//  Always positive
long RECIP(u16 uv)
{
    if (uv < 4)
        return 0x3FFFFFFF;
    //return 0x80000000/(uv>>1);   // Long divide to provide 16:16 result
    return reciprocalfix(uv);
}

//  dduv is always positive 16:16 number may be very large, might resonably be trimmed
long MUL8(u8 a, long dduv)
{
    return (dduv >> 8)*a;
}

// a little lerping
// QQTTTTTTFFFF
//  Q: Quadrant
//  T: Table lookup
//  F: Lerp fraction
short SIN16(u16 angle)
{
    short a = SIN(angle >> 4);
    short b = SIN((angle+15) >> 4);
    u8 f = angle & 0x0F;
    return a*(16-f) + b*f;  //+-16
}

short COS16(u16 angle)
{
    return SIN16(angle + (64 << 4));
}

#define JOY_X 120
#define JOY_Y 260   // below center

inline
void swap(uint16_t *a, uint16_t *b)
{
    uint16_t t=*a; *a=*b; *b=t;
}

void qsort_iterative(uint16_t arr[], int l, int h, int* stack)
{
    int8_t top = -1;
    stack[ ++top ] = l;
    stack[ ++top ] = h;
    while ( top >= 0 )
    {
        h = stack[ top--];
        l = stack[ top--];
        
        uint16_t x = arr[h];
        int p = l - 1;
        for (int j = l; j <= h-1; j++)
        {
            if (arr[j] <= x)
            {
                p++;
                swap (&arr[p], &arr[j]);
            }
        }
        swap (&arr[++p], &arr[h]);
        
        if (p-1 > l)
        {
            stack[ ++top ] = l;
            stack[ ++top ] = p - 1;
        }
        if (p+1 < h)
        {
            stack[ ++top ] = p + 1;
            stack[ ++top ] = h;
        }
    }
}

/*
 Alas, stack is a precious commodity
 void sort(uint16_t* arr, int beg, int end)
 {
 if (end > beg + 1)
 {
 int piv = arr[beg], l = beg + 1, r = end;
 while (l < r)
 {
 if (arr[l] <= piv)
 l++;
 else
 swap(&arr[l], &arr[--r]);
 }
 swap(&arr[--l], &arr[beg]);
 sort(arr, beg, l);
 sort(arr, r, end);
 }
 }
 */

//  Drawing state for SCENE in col form
uint16_t    hs[SCREEN_WIDTH];
u8          ts[SCREEN_WIDTH];

//#define min(_a,_b) (_a) < (_b) ? (_a) : (_b)

//============================================================
//============================================================

#ifdef HIRES
#define LINE_BYTES SCREEN_WIDTH/4
#else
#define LINE_BYTES SCREEN_WIDTH/2
#endif
#define DRAW_BUFFERS 4
#define DRAW_BUFFER_MASK (DRAW_BUFFERS-1)
#define SAMPLES_SIZE (3+SCREEN_WIDTH*3+1)

volatile uint8_t _errors = 0;
class DrawState
{
public:
    uint8_t* _draw;
    uint8_t* _show;
    uint8_t* _copySrc;
    uint8_t* _copyDst;
    
    uint8_t* _samples;
    uint8_t* _mark;
    int8_t _step;
    volatile uint8_t _pending;

    uint8_t _read;
    uint8_t _write;
    uint8_t _draw_y;
    
    uint8_t _buffer[LINE_BYTES*DRAW_BUFFERS];
    uint8_t* _lines[DRAW_BUFFERS];
    uint8_t _y[DRAW_BUFFERS];
    uint8_t _sample_buffers[SAMPLES_SIZE*2];  // double buffered
    
    DrawState()
    {
        _pending = 0;
        _samples = 0;
        for (int i = 0; i < DRAW_BUFFERS; i++)
            _lines[i] = _buffer + i*LINE_BYTES;
        reset();
    }
    
    uint8_t* other()
    {
      return _samples == _sample_buffers ? _sample_buffers + SAMPLES_SIZE : _sample_buffers;
    }
    
    uint8_t* get_sample_buffer()
    {
      while (_pending)
        ;
      return other();
    }
    
    void post(uint8_t* samples)
    {
        _pending = 0xFF;
    }
    
    void reset()
    {
        _read = _write = _draw_y = 0;
        _draw = _show = _copySrc = _copyDst = _lines[0];
        _y[0] = 0;
        memset(_lines[0],FLOOR,LINE_BYTES);
    }
    
    void blanking(uint8_t line)
    {
        if (line == 0)
        {
            reset();
            if (_pending)
            {
                _samples = other();
                _pending = 0;
            }
            _mark = (uint8_t*)_samples+3;
            _step = 3;
        } else {
            active(0);
            if (_copyDst != _copySrc)
                memcpy(_copyDst,_copySrc,LINE_BYTES);   // instead of blit
        }
    }
    
    void edit(uint8_t n = 4)
    {
        uint8_t* dst = _draw;
        uint8_t draw_y = _draw_y;
        if (_copyDst == dst || draw_y == 192)
            return;
        uint8_t* src = _mark;
        if (_step < 0)
            draw_y = 192 - draw_y;
        int8_t step = _step;
        do  {
          if (src[0] != draw_y)
            break;
          dst[src[1]] ^= src[2];  // 9
          src += step;            // 1
        } while(--n);
        _mark = src;
    }

    void active(uint8_t line, uint8_t n = 8)
    {
        //printf("line %d, drawing %d, %d in buffer\n",line,_draw_y,_write-_read);
        _copyDst = _copySrc;  // copy is always one shot
        if (!_samples)
            return; // don't really need this
        
        if (_draw_y != 192)
        {
            uint8_t* src = _mark;
            uint8_t draw_y = _draw_y;
            if (_step < 0)
                draw_y = 192 - draw_y;
            {
                uint8_t* dst = _draw;
                int8_t step = _step;
                do  {
                  if (src[0] != draw_y)
                    break;
                  dst[src[1]] ^= src[2];  // 9
                  src += step;            // 1
                } while(--n);
            }
            
            uint8_t y = src[0];
            if (y != draw_y)
            {
                if (y == 0xFF)
                {
                    _step = -3;
                    src -= 3;
                }
                if (_step < 0)
                    y = 192-src[0];

                if ((_write - _read) >= DRAW_BUFFERS-2)
                {
                    //printf("FULL\n");
                } else {
                    //printf("Copying %d [%d] to %d [%d]\n",_draw_y,_write& DRAW_BUFFER_MASK,y,(_write+1)& DRAW_BUFFER_MASK);
                    uint8_t i = ++_write & DRAW_BUFFER_MASK;
                    _copySrc = _draw;
                    _draw = _lines[i & DRAW_BUFFER_MASK];
                    _copyDst = _draw; // will copy next loop
                    _y[i] = y;
                    _draw_y = y;
                }
            }
            _mark = src;
        }
        
        uint8_t i = _read & DRAW_BUFFER_MASK;
        if (line >= _y[i] && (_write-_read))
        {
            //printf("Showing %d [%d]\n",_y[i],i);
            _show = _lines[i];
            _read++;
        }
    }
};
DrawState _drawState;

void video_blanking(uint8_t line)
{
    _drawState.blanking(line);
}

#ifdef ARDUINO
static void inline tick_alignx(uint8_t tick)
{
    __asm__ __volatile__ (
                          "sub	%[tick], %[tcnt1l]\n"
                          "loopx: subi	%[tick], 3\n"
                          "brcc	loopx\n"
                          "cpi      %[tick],-3\n"
                          "breq     donex\n"
                          "cpi      %[tick],-2\n"
                          "brne     .\n"
                          "donex:\n"
                          :: [tick] "a" (tick),
                          [tcnt1l] "a" (TCNT1L)
                          );
}
#endif

// 0..191
void video_active(uint8_t line)
{
    PAUSE_VIDEO();  // Stop the clock
      
    DrawState& state = _drawState;    
    state.edit(6);  // Do a bit of drawing
    
#ifdef ARDUINO
    uint8_t phase = (448-256)+16;
    while (TCNT1L < (448-256))
      ;
    tick_alignx(phase);
#endif
    
    uint8_t* show = state._show;
    uint8_t* src = state._copySrc;
    uint8_t* dst = state._copyDst;
    uint8_t x = LINE_BYTES;
    UNPAUSE_VIDEO();                // Restart clock
    do
    {
        uint8_t c = *show++;   // Blit and copy
        PIXEL_OUT(c);
        *dst++ = *src++;
        asm("nop");
        asm("nop");
        asm("nop");
    } while(--x);
    RELEASE_VIDEO();
    state.active(line+1,6);
};


//============================================================
//============================================================

class DoomState : public Game
{
    short _playerPosX;
    short _playerPosY;
    
    u16 _angle;
    signed char _moveRate;
    signed char _turnRate;
    
    short _x;
    short _y;
    
    u8 _phase;
    u8 _allglow;
    u16 _glow;
    
    uint8_t _frame;
    
public:
    short Init()
    {
        _angle = 0x2EBA;    // a cool place on the map
        _playerPosX = 0x3AC;
        _playerPosY = 0x344;
        _moveRate = _turnRate = 0;
        _x = -1;
        _phase = 0;
        _allglow = 0;
        _frame = 0;
        return 0;
    }
    
#define PLAYERWIDTH 0x20
    u8 InWall(short dx, short dy)
    {
        dx += _playerPosX;
        dy += _playerPosY;
        u8 x0 = (dx-PLAYERWIDTH)>>8;
        u8 y0 = (dy-PLAYERWIDTH)>>8;
        u8 x1 = (dx+PLAYERWIDTH)>>8;
        u8 y1 = (dy+PLAYERWIDTH)>>8;
        while (x0 <= x1)
        {
            for (u8 y = y0; y <= y1; y++)
            {
                u8 t = pgm_read_byte(&_map[y * 8 + x0]);
                if (t)
                    return t;
            }
            x0++;
        }
        return 0;
    }
    
    // Add a little acceleration to movement
    int damp(int i)
    {
        if (i < 0)
            return ++i;
        if (i > 0)
            i--;
        return i;
    }
    
    void move()
    {
        /*
         if (_x == -1)
         {
         _turnRate = damp(_turnRate);
         _moveRate = damp(_moveRate);
         } else {
         _turnRate = (JOY_X - _x) >> 2;
         _moveRate = (JOY_Y - _y) >> 2;
         }
         */
        
        if (_turnRate)
            _angle += _turnRate;
        
        //  Rather dumb wall avoidance
        while (_moveRate)
        {
            short dx = ((COS16(_angle) >> 5)*_moveRate) >> 7;
            short dy = ((SIN16(_angle) >> 5)*_moveRate) >> 7;
            u8 t = InWall(dx,dy);
            if (t)
            {
                if (t == 6)
                    _allglow = 0xFF;
                if (!InWall(0,dy))
                    dx = 0;
                else if (!InWall(dx,0))
                    dy = 0;
            }
            if (!InWall(dx,dy))
            {
                _playerPosX += dx;
                _playerPosY += dy;
                break;
            }
            _moveRate = damp(_moveRate);
        }
    }
    
    short Loop(signed char dx, signed char dy)
    {
        _turnRate = dx;
        _moveRate = dy;
        
        move();
        //glow();
        
        // cast all rays here
        // then draw on a second pass
        
        short sina = SIN16(_angle) << 2;
        short cosa = COS16(_angle) << 2;
        short u = cosa - sina;          // Range of u/v is +- 2 TODO: Fit in 16 bit
        short v = sina + cosa;
        short du = sina / (SCREEN_WIDTH>>1);     // Range is +- 1/24 - 16:16
        short dv = -cosa / (SCREEN_WIDTH>>1);
        
#ifdef HIRES
        memset(ts,0,sizeof(ts));
#endif
        for (u8 ray = 0; ray < SCREEN_WIDTH; ray++, u += du, v += dv)  // TODO LORES u8 ray
        {
            long duu = RECIP(abs(u));
            long dvv = RECIP(abs(v));
            signed char stepx = (u < 0) ? -1 : 1;
            signed char stepy = (v < 0) ? -8 : 8;
            
            // Initial position
            u8 mapx = _playerPosX >> 8;
            u8 mapy = _playerPosY >> 8;
            u8 mx = _playerPosX;
            u8 my = _playerPosY;
            if (u > 0)
                mx = 0xFF-mx;
            if (v > 0)
                my = 0xFF-my;
            long distx = MUL8(mx, duu);
            long disty = MUL8(my, dvv);
            
            u8 map = mapy*8 + mapx;
            u8 texture = 0;
            for (;;)
            {
                if (distx > disty)
                {
                    // shorter distance to a hit in constant y line
                    map += stepy;
                    texture = pgm_read_byte(&_map[map]);
                    if (texture)
                        break;
                    disty += dvv;
                } else {
                    // shorter distance to a hit in constant x line
                    map += stepx;
                    texture = pgm_read_byte(&_map[map]);
                    if (texture)
                        break;
                    distx += duu;
                }
            }
            
            //  Texture loop
            long hitdist;
            if (distx > disty)
            {
                hitdist = disty;
                if (stepy <= 0)
                {
                    map >>= 3;
                    texture = pgm_read_byte(&_map[map+1]);
                }
                //texture |= 0x80;    // TODO...color
            }
            else
            {
                hitdist = distx;
                if (stepx <= 0)
                {
                    map &= 7;
                    texture = pgm_read_byte(&_map[map+1]);
                }
            }
            
            //short hh = SCREEN_WIDTH*2*0x10000L/hitdist;
            short hh = reciprocalfix(hitdist >> 8) >> 16;
            if (hh > SCREEN_HEIGHT/2)
                hh = SCREEN_HEIGHT/2;
            
#ifdef HIRES
            hs[ray] = ((SCREEN_HEIGHT/2-hh) << 9) | ray; // y at x
            texture &= pgm_read_byte(_mask4+(ray & 3));
            ts[ray>>2] |= texture;  // color at x
#else
            hs[ray] = ((SCREEN_HEIGHT/2-hh) << 8) | ray; // y at x
            ts[ray] = texture;  // color at x
#endif
        }
        return 0;
    }
    
    virtual void init()
    {
        Init();
        //for (int i = 0; i < 10; i++)  // WTF?
        Loop(1,3);
    }
    
    virtual void step()
    {
        //printf("=========================frame %d\n",_frame++);
        Loop(1,3);
    }
    
    virtual uint8_t get_tile(uint8_t x, uint8_t y)  // No sprites
    {
        return 0;
    }
    
    
    virtual void draw()
    {
        uint8_t* buf = _drawState.get_sample_buffer();
        qsort_iterative(hs,0,SCREEN_WIDTH-1,(int*)buf);  //
        
        uint8_t* dst = buf;
        *dst++ = 0; // terminal Y
        *dst++ = 0;
        *dst++ = 0;
        uint8_t lastxm = 0;
        uint8_t xmap = 0;
        uint8_t lasty = 0;
#ifdef HIRES
        for (uint16_t i = 0; i < SCREEN_WIDTH; i++)
        {
            uint16_t n = hs[i];
            uint8_t y = n >> 9;
            uint16_t x = n & 0x1FF;
            uint8_t xm = x >> 2;
            uint8_t t = ts[xm];  // TODO
            
            if (((xm != lastxm) || (y != lasty)) && xmap)
            {
                *dst++ = lasty;
                *dst++ = lastxm;
                *dst++ = xmap;
                xmap = 0;
            }
            xmap |= t & pgm_read_byte(_mask4 + (x & 3));
            lasty = y;
            lastxm = xm;
        }
#else
        for (uint8_t i = 0; i < SCREEN_WIDTH; i++)
        {
            uint16_t n = hs[i];
            uint8_t y = n >> 8;
            uint8_t x = n;
            uint8_t t = ts[x];  // TODO
            
            uint8_t xm = x >> 1;
            if (((xm != lastxm) || (y != lasty)) && xmap)
            {
                *dst++ = lasty;
                *dst++ = lastxm;
                *dst++ = xmap;
                xmap = 0;
            }
            xmap |= t & pgm_read_byte(_mask2 + (x & 1));
            lasty = y;
            lastxm = xm;
        }
#endif
        if (xmap)
        {
            *dst++ = lasty;
            *dst++ = lastxm;
            *dst++ = xmap;
        }
        
        *dst++ = 0xFF; // midpoint Y
        _errors = 0x00;
        //printf("%d bytes\n",dst-buf);
        //}
        _drawState.post(buf);
        
        // printf("%d bytes for frame\n",dst-_bits);
        for (int y = 0; y < 192; y++)
            flush_video(true);
        
    };
};

DoomState _doomed;
Game* current_game()
{
    return &_doomed;
}

#endif
