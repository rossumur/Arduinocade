//
//  blazer.cpp
//  CavernsSim
//
//  Created by Peter Barrett on 5/27/14.
//  Copyright (c) 2014 Peter Barrett. All rights reserved.
//

#include "config.h"
#include "music.h"
#include "ballblaster_tiles.h"

#ifndef BALLBLASTER_GAME
#error Please #define BALLBLASTER_GAME in arduinocade/config.h
#endif

// Nostalgia
// http://www.atarimania.com/game-atari-400-800-xl-xe-ballblazer_476.html

#define ASSERT(_x)
#define MIN(_a,_b) (((_a) < (_b)) ? (_a) : (_b))
#define MAX(_a,_b) (((_a) > (_b)) ? (_a) : (_b))

#ifdef ARDUINO
#define DBG_TOGGLE() PORTD ^= 0x80
#define DBG0() PORTD &= 0x7F
#define DBG1() PORTD |= 0x80
#else
#define DBG_TOGGLE()
#define DBG0()
#define DBG1()
#endif

#define L_HORIZON       50L    // Number of lines of sky
#define L_GRID_LINES    37L     // Number of lines of ground

#define L_CAMERA_Y 48L  // TODO 96 matches game better, ~25% more edges
#define L_CAMERA_Z 64L

//#define GODSEYE 1
#ifdef GODSEYE
#define L_CAMERA_Y 48L  // TODO
#define L_CAMERA_Z 64L
#endif

#define CELL_SIZE 0x100L // Makes math easy
#define GRID_Y 55L //
#define GRID_X 21L

#define PULSE 1234

#define MIN_X (CELL_SIZE/4)
#define MIN_Y (CELL_SIZE/4)
#define MAX_X ((GRID_X*CELL_SIZE) - MIN_X)
#define MAX_Y ((GRID_Y*CELL_SIZE) - MIN_Y)

#define SHIP0_OBJ   0
#define SHIP1_OBJ   1
#define BALL_OBJ    4

#define GOAL_THICKNESS  24
#define GOAL_HEIGHT     96

#define DIR_UP      0
#define DIR_LEFT    1
#define DIR_DOWN    2
#define DIR_RIGHT   3

#define WIDTH 144
#define HEIGHT (L_HORIZON+L_GRID_LINES) // Height of rendered windows

#define BUFFER_SIZE 1350    // 1414 total.
#define MAX_LINE_BYTES 64

#define GRID_BUF_SIZE (8*2*2)  // Can transform 7 point ships (7+1)
#define BALL_CAPTURE    0x80
#define SCORE_0         0x40
#define SCORE_1         0x20


#define SKY 4       // Deep blue
#define GROUND 1    // ground color

#define S0 11
#define S0_SHADOW 1

#define S1 13
#define S1_SHADOW 8


#define W 0     // Windshield
#define B 15    // Ball


// Lookup z and width of grid at a particular y
// Note different fields are offset in y
PROGMEM
const short _ytoz[]  = {
    768, // 4
    614, // 5
    512, // 6
    438, // 7
    384, // 8
    341, // 9
    307, // 10
    279, // 11
    256, // 12
    236, // 13
    219, // 14
    204, // 15
    192, // 16
    180, // 17
    170, // 18
    161, // 19
    153, // 20
    146, // 21
    139, // 22
    133, // 23
    128, // 24
    122, // 25
    118, // 26
    113, // 27
    109, // 28
    105, // 29
    102, // 30
    99, // 31
    96, // 32
    93, // 33
    90, // 34
    87, // 35
    85, // 36
    83, // 37
    80, // 38
    78, // 39
    76, // 40
    
    // odd field
    683, // 4
    559, // 5
    473, // 6
    410, // 7
    361, // 8
    323, // 9
    293, // 10
    267, // 11
    246, // 12
    228, // 13
    212, // 14
    198, // 15
    186, // 16
    176, // 17
    166, // 18
    158, // 19
    150, // 20
    143, // 21
    137, // 22
    131, // 23
    125, // 24
    120, // 25
    116, // 26
    112, // 27
    108, // 28
    104, // 29
    101, // 30
    98, // 31
    95, // 32
    92, // 33
    89, // 34
    87, // 35
    84, // 36
    82, // 37
    80, // 38
    78, // 39
    76, // 40
};

PROGMEM
const uint8_t _ytowidth[] = {
    21, // 4
    27, // 5
    32, // 6
    37, // 7
    43, // 8
    48, // 9
    53, // 10
    59, // 11
    64, // 12
    69, // 13
    75, // 14
    80, // 15
    85, // 16
    91, // 17
    96, // 18
    101, // 19
    106, // 20
    112, // 21
    117, // 22
    122, // 23
    128, // 24
    133, // 25
    139, // 26
    144, // 27
    149, // 28
    155, // 29
    161, // 30
    165, // 31
    171, // 32
    176, // 33
    182, // 34
    186, // 35
    193, // 36
    197, // 37
    202, // 38
    207, // 39
    213, // 40
    
    // odd field
    24, // 4
    29, // 5
    35, // 6
    40, // 7
    45, // 8
    51, // 9
    56, // 10
    61, // 11
    67, // 12
    72, // 13
    77, // 14
    83, // 15
    88, // 16
    93, // 17
    99, // 18
    104, // 19
    109, // 20
    115, // 21
    120, // 22
    125, // 23
    131, // 24
    137, // 25
    141, // 26
    146, // 27
    152, // 28
    158, // 29
    162, // 30
    167, // 31
    172, // 32
    178, // 33
    184, // 34
    188, // 35
    195, // 36
    200, // 37
    205, // 38
    210, // 39
    216, // 40
};


// Music and sounds
// TADA theme on goal
const uint8_t _tada[] PROGMEM = {
    M_WAVE,     (TRIANGLE << 4) | (1 | 2 | 4),
    M_ENVELOPE, (NONE << 4) | (1 | 2 | 4),
    
    M_NOTES_ON | 1 | 2 | 4, 69,76,81,
    M_DELAY_8,
    M_NOTES_ON | 1 | 2 | 4, 71,78,83,
    M_DELAY_16,
    
    M_NOTES_ON | 1 | 2 | 4, 69,76,81,
    M_DELAY_8,
    M_NOTES_ON | 1 | 2 | 4, 71,78,83,
    M_DELAY_16,
    
    M_SLIDE, 4, (uint8_t)-1,
    M_DELAY, 36,
    M_SLIDE, 4, 0, // slide channel 2 83->47
    M_DELAY_4,
    
    M_NOTES_ON | 1, 63,
    M_DELAY, 40,
    
    M_NOTES_OFF | 1 | 2 | 4,
    M_DELAY, 30,   // beat
    M_END
};

const uint8_t _theme_init[] PROGMEM = {
    M_NOTES_OFF | 1 | 2 | 4 | 8,
    M_WAVE,     (SAWTOOTH << 4) | 1 | 2 | 4,
    M_WAVE,     (RANDOM << 4) | 8,
    M_ENVELOPE, (KEYBOARD << 4) | 1 | 2 | 4,
    M_ENVELOPE, (HIHAT << 4) | 8,
    M_END
};

const uint8_t _win[] PROGMEM = {
    M_ENVELOPE, (HIT << 4) | 2,   // TODO. envelope shaping
    M_WAVE,     (SQUARE << 4)   | 2,      // Sled womp/bounce noise

    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,

    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    M_NOTES_ON | 2, 96,
    M_DELAY_5,
    
    M_DELAY,120,
    M_END
};

// Theme, 6.4 second loop
// Saw for 0-2, HiHat on 3
#define PREPLAY_OFFSET 124
const uint8_t _theme[] PROGMEM = {
    M_NOTES_OFF | 0x0F,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x28,0x34,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x2f,0x3b,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x25,0x31,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x23,0x2f,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    
    // preplay cue
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x28,0x34,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x2f,0x3b,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x07,0x25,0x38,0x41,
    M_DELAY_20,
    M_NOTES_OFF | 0x07,
    M_DELAY_4,
    M_NOTES_ON | 0x0f,0x23,0x36,0x3f,0x28,
    M_DELAY_20,
    M_NOTES_OFF | 0x0f,
    M_DELAY_4,
    M_NOTES_ON | 0x07,0x1c,0x38,0x40,
    M_DELAY_4,
    M_NOTES_OFF | 0x07,
    M_DELAY_2,
    M_NOTES_ON | 0x07,0x1c,0x36,0x3e,
    M_DELAY_4,
    M_NOTES_OFF | 0x07,
    M_DELAY_14,
    M_NOTES_ON | 0x0f,0x1c,0x38,0x3b,0x28,
    M_DELAY_20,
    M_NOTES_OFF | 0x0f,
    M_DELAY_4,
    M_END
};

// In game sound effects
// channel 0: Ball/Field buzzing
// channel 1: Ball firing
// channel 2: Sled womp noise
// channel 3: Tat-tat-tat sound

PROGMEM
const uint8_t _effect_init[] = {
    M_WAVE,     (SQUARE << 4)   | 1,    // Ball/Field buzzing
    M_ENVELOPE, (NONE << 4)     | 1,
    M_NOTES_ON | 1, 36,                 // start buzzing on channel 0 (~66hz)
    M_VOLUME, 1, 0,
    
    M_WAVE,     (RANDOM << 4)   | 2,     // Ball firing
    M_ENVELOPE, (FADE2 << 4)    | 2,
    
    M_WAVE,     (SQUARE << 4)   | 4,      // Sled womp/bounce noise
    M_ENVELOPE, (HIHAT << 4)    | 4,
    
    M_WAVE,     (RANDOM << 4)   | 8,     // Tat-tat-tat sound
    M_ENVELOPE, (FADE << 4)     | 8,
    
    M_END
};

PROGMEM
const uint8_t _ball_push[] = {
    M_NOTES_ON | 2, 80,     // channel 1
    M_SLIDE, 2, (uint8_t)-10,
    M_END
};

PROGMEM
const uint8_t _sled_womp[] = {
    M_NOTES_ON | 4, 20,      // channel 3
    M_NOTES_ON | 8, 120,     // channel 4
    M_SLIDE, 4, 10,
    M_END
};

PROGMEM
const uint8_t _tat[] = {
    M_NOTES_ON | 8, 120,      // channel 3
    M_END
};

PROGMEM
const uint8_t _bounce[] = {
    M_NOTES_ON | 4, 128+0,      // channel 3
    M_SLIDE, 4, 3,
    M_END
};

PROGMEM
const uint8_t _tick[] = {
    M_NOTES_ON | 4, 77,      // channel 3
    M_END
};

PROGMEM
const uint8_t _tock[] = {
    M_NOTES_ON | 4, 89,      // channel 3
    M_END
};


#define BALL_CAPTURED   0x80
#define BALL_PUSH       0x40

//============================================================
//============================================================
//  Game Rendering state

uint8_t _cellmap[32*2];                         // Buffer for status in middle of screen
uint8_t _grid_buf[GRID_BUF_SIZE];               // Buffer for one line of grid runs
uint8_t _bb_buf[BUFFER_SIZE+MAX_LINE_BYTES];    // buffer for screen runs   // MAX_LINE_BYTES not required
volatile uint8_t* _read_rle = _bb_buf;
uint8_t* _write_rle = _bb_buf;

extern uint8_t _line_count;
uint8_t _odd = 1;       // currently drawing odd field

//  Render parameters
struct {
    short z;
    short x;
    short px;
    short py;
    short gpx,gpy;              // Grid transformed coordinates
    uint8_t gridx,gridy;
    uint8_t orientation;        // Orientation of current observer
    uint8_t sky;
} _render_params;


// Game state machine
enum GameState {
    SPLASH,             // Logo, no ball, theme playing
    PLAYER_SELECTION,
    PREPLAY,            // Clock not running, left and right only (no ball)
    PLAYING,
    GOAL,
    WIN,
    POSTWIN,
};

typedef struct Obj
{
    uint8_t orientation;
    uint8_t flags;
    short x,y;
    short dx,dy;
    void orient(short* ddx, short* ddy);
    void move(short ddx, short ddy,bool pin = 0);
    uint8_t attract(Obj& ball, short* ddx, short* ddy, uint8_t goal_dir);
    void collide(bool goal);
} Obj;

struct {
    GameState state;  // 2 bytes for enum...GRR
    short goal_pos;
    short game_time;
    uint8_t tat_timer;
    uint8_t score0;
    uint8_t score1;
    uint8_t pushed_from;
    uint16_t lfsr;
    Obj ball;
    Obj ship[2];
} _game;

//  Edge renderer/stepper
#define TOTAL_EDGES (14+10) // Max total edges in a scene

// Edge Rendering state for current view
struct {
    uint8_t index;
    uint8_t* sorted;        // current sorted
    uint8_t ball_size;
    uint8_t ball_top;
    uint8_t ball_bottom;
    uint8_t active[14];     // Active edges used in render
    uint8_t stack[8];       // Layer stack for rendering z layers
    
    // Top View
    uint8_t count0;         // number of edges top list
    uint8_t ball0_radius;
    uint8_t sorted0[14];    // sorted by miny
    
    // Bottom View
    uint8_t count1;         // number of edges in bottom list
    uint8_t ball1_radius;
    uint8_t sorted1[14];    // sorted by miny
    
    // Combined edges
    int16_t x[TOTAL_EDGES];
    int8_t dx[TOTAL_EDGES];
    uint8_t miny[TOTAL_EDGES];
    uint8_t maxy[TOTAL_EDGES];
    uint8_t color[TOTAL_EDGES];
} edge;

void sound(const uint8_t* m)
{
    if (!music_playing())
        music_play(m);
}

//============================================================
//============================================================
//  Moveable and bouncable objects

void Obj::orient(short* ddx, short* ddy)
{
    short x = *ddx;
    short y = *ddy;
    switch (orientation)
    {
        case DIR_UP:
            *ddx = x;
            *ddy = y;
            break;
        case DIR_DOWN:
            *ddx = x;
            *ddy = -y;
            break;
        case DIR_LEFT:
            *ddx = -y;
            *ddy = x;
            break;
        case DIR_RIGHT:
            *ddx = y;
            *ddy = x;
            break;
    }
}

#define MAX_VELOCITY 0x1000

inline short drag(short n)
{
    if (n < 0)
        return -drag(-n);
    if (n)
        n -= (n >> 6) + 1;
    return n;
}
void Obj::move(short ddx, short ddy, bool pin)
{
    dx = drag(dx);  // signed? TODO
    dy = drag(dy);
    dx += ddx;
    dy += ddy;
    
    if (pin) {
        if (dx < -MAX_VELOCITY) dx = -MAX_VELOCITY;
        if (dx > MAX_VELOCITY) dx = MAX_VELOCITY;
        if (dy < -MAX_VELOCITY) dy = -MAX_VELOCITY;
        if (dy > MAX_VELOCITY) dy = MAX_VELOCITY;
    }
    
    x += dx >> 8;   //
    y += dy >> 8;   // dx,dy stored as 8:8
}

#define ATTRACT_DIST ((CELL_SIZE*3)>>1)

// Attact a ball to this rotofoil
uint8_t Obj::attract(Obj& ball, short* ddx, short* ddy, uint8_t goal_dir)
{
    short ax,ay;    // Attractor point
    ax = x - ball.x;
    ay = y - ball.y;
    
    // Orient the ship towards the ball
    uint8_t ox;
    
    bool mx = abs(ax) <= abs(ay);
    if (mx)
        ox = ay < 0 ? DIR_UP : DIR_DOWN;
    else
        ox = ax > 0 ? DIR_LEFT : DIR_RIGHT;
    
    // happy win - spin ship
    if ((_game.state == WIN) && (goal_dir == DIR_UP) == ((ball.flags & SCORE_0) != 0))
    {
        ox = (v_framecounter >> 3) & 3;   // TODO.. slow down
    }
    
    if (orientation != ox)
    {
        sound(_sled_womp);
        orientation = ox;
    }
    
    // Attract captured ball to front of rotofoil
    ay += goal_dir == DIR_UP ? ATTRACT_DIST : -ATTRACT_DIST;
    
    uint8_t buzz = 0xFF;
    if (abs(ax) < 0x200 && abs(ay) < 0x200 && (!(ball.flags & (SCORE_0 | SCORE_1))))
    {
        if (abs(ball.dx) > 0x4000 || abs(ball.dy) > 0x4000)
        {
            // Too fast for capture
        } else {
            buzz = (abs(ax)+abs(ay))>>1;    // Buzz if you have the ball
            if ((flags & BALL_CAPTURED) == 0)
                flags |= BALL_CAPTURED;         // TODO. will recapture immediately
        }
    }
    else
        flags &= ~(BALL_CAPTURED | BALL_PUSH);    // Dropped
    
    if (flags & BALL_CAPTURED)
    {
        if (flags & BALL_PUSH)
        {
            ax = x - ball.x;
            ay = y - ball.y;
            *ddx -= ax << 6;    // Fire (the) ball
            *ddy -= ay << 6;
            dx += ax << 3;      // Recoil rotofoil
            dy += ay << 3;
            flags &= ~(BALL_CAPTURED | BALL_PUSH);
            sound(_ball_push);
        } else {
            *ddx += ax;
            *ddy += ay << 2;    // Tighter in y TODO. push away from rotofoil center
            dx -= ax >> 2;      // Rock rotofoil
            dy -= ay >> 1;
        }
    }
    return buzz;
}

// Bounce or score
void Obj::collide(bool goal)
{
    uint8_t bounced = 0;
    if (x < MIN_X)
    {
        x = MIN_X;
        dx = -dx/2;
        bounced++;
    }
    
    if (x > MAX_X)
    {
        x = MAX_X;
        dx = -dx/2;
        bounced++;
    }
    
    if (y < MIN_Y)
    {
        if (goal || (flags & SCORE_1))
            flags |= SCORE_1;
        else {
            y = MIN_Y;
            dy = -dy/2;
            bounced++;
        }
    }
    if (y > MAX_Y)
    {
        if (goal || (flags & SCORE_0))
            flags |= SCORE_0;
        else {
            y = MAX_Y;
            dy = -dy/2;
            bounced++;
        }
    }
    if (bounced)
        sound(_bounce);
}

//============================================================
//============================================================
//  Cell drawing (lines in middle of screen

const uint8_t* _celltiles = _tiles;

#define SCORE_CIRCLE    11
#define SCORE_CIRCLE_0  12
#define SCORE_CIRCLE_1  14
//
// 32 bytes wide
// plot a n x 2 figure to dst
void plot(uint8_t x, uint8_t y, uint8_t width, uint8_t* dst)
{
    const uint8_t* src = _map + x + y*32;
    uint8_t i;
    for (i = 0; i < width; i++)
    {
        dst[i] = pgm_read_byte(&src[i]);
        dst[i+32] = pgm_read_byte(&src[i+32]);
    }
}

void plot_num(uint8_t n, uint8_t x)
{
    plot(n<<1,0,2,_cellmap+x);    // min
}

void clear_title()
{
    memset(_cellmap,1,sizeof(_cellmap));
}

void draw_title()
{
    clear_title();
    plot(0,4,20,_cellmap+6);
}

inline
void draw_score_dot(uint8_t dot, uint8_t x)
{
    if (dot != SCORE_CIRCLE && (v_framecounter & 0x10))
        dot++;
    if (x >= 5)
        x += 6;
    plot_num(dot,x*2);
}

void draw_num(uint16_t n, uint8_t x)
{
    uint8_t i = 3;
    while (i--)
    {
        plot_num(n%10,x + i*2);
        n /= 10;
    }
}

// Draw (and update) time
void draw_time()
{
    if (_game.state == POSTWIN && (v_framecounter & 0x40))
    {
        draw_title();
        return;
    }
    clear_title();
    
    // Draw score dots
    for (uint8_t n = 0; n < 10; n++)
    {
        uint8_t dot = SCORE_CIRCLE;
        if (_game.score0 > n)
            dot = SCORE_CIRCLE_0;
        else if (_game.score1 > (9-n))
            dot = SCORE_CIRCLE_1;
        draw_score_dot(dot,n);
    }
    
    // 1420/720
    int n = _game.game_time;            // in 60ths
    switch (_game.state) {
        case PREPLAY:
        case GOAL:
        case WIN:
            break;
        default:
            _game.game_time = n-1;
    }
    
    if (n < 0) {
        if (n & 0x20)
            plot(0,2,6,_cellmap+13);    // Flash OVERTIME
    }
    else
    {
        // Draw time remaining
        /*
         uint8_t tenths = (n/6) % 10;
         uint8_t sec = (n/60) % 10;
         uint8_t tens = (n/600) % 6;
         uint8_t mins = (n/3600);
         */
        
        n /= 6;
        uint8_t mins = n/600;
        n -= mins*600;
        uint8_t tens = n/100;
        n -= tens*100;
        uint8_t secs = n/10;
        uint8_t tenths = n - secs*10;
        
        if (mins == 0 && tenths == 0 && (secs + tens*10 <= 10))
            sound(secs & 1 ? _tock : _tick);
        
        plot_num(mins,11);
        plot(20,0,2,_cellmap+13);    // :
        plot_num(tens,14);
        plot_num(secs,16);
        plot(21,0,2,_cellmap+18);    // .
        plot_num(tenths,19);
    }
}

uint8_t _dbg[4];
void draw_debug()
{
    clear_title();
    for (uint8_t i = 0 ; i < 4; i++)
        draw_num(_dbg[i],2+i*7);
}

//============================================================
//============================================================
//  Grid drawing

uint8_t _emit_state = 0x0;
inline
uint8_t* emit(uint8_t x0, uint8_t x1, uint8_t color, uint8_t* dst)
{
    // Avoid a fast masking case in blit asm kernel
    // Back to back short odd runs take too many clocks
    // Rare case, also handle orphan single pixels
    if ((x0 & 1) && _emit_state)  // need to prepend
    {
        uint8_t last_color = _emit_state & 0x40 ? (_emit_state << 4) : (dst[-1] & 0xF0);
        _emit_state = 0;
        *dst++ = 1;
        *dst++ = last_color | color;
        if (++x0 == x1)
            return dst;
    }
    
    uint8_t run = 0;
    if (x0 & 1)
    {
        run = 0x80;
        x0++;
    }
    
    run |= (x1-x0)>>1;
    if (run) {
        *dst++ = run;
        *dst++ = color | (color << 4);
        if (run == 0x80)
            _emit_state = 0x80;
    } else {
        if (++x0 == x1)  //++x0 == x1
            _emit_state = color | 0x40; // single pixel on even, save for next time
    }
    return dst;
}

uint8_t *_grid = _grid_buf;
inline
uint8_t* emit_bg(uint8_t x0, uint8_t x1, uint8_t* dst)
{
    uint8_t *grid = _grid;
    while (x0 < x1)
    {
        while (grid[2] <= x0)
            grid += 2;
        uint8_t count = x1-x0;
        count = MIN(count,grid[2]-x0);
        dst = emit(x0,x0+count,grid[1],dst);
        x0 += count;
    }
    _grid = grid;
    return dst;
}

#define DRAWGRID(_color) *dst++ = x < 0 ? 0 : x; *dst++ = (_color);

PROGMEM
const uint8_t _skytab[] = {
    15,14,12,SKY,
    15,S0,S0,S0, 15,15,15,15, 15,S0,S0,S0, 15,S0,S0_SHADOW,S0_SHADOW,
    15,S1,S1,S1, 15,15,15,15, 15,S1,S1,S1, 15,S1,S1_SHADOW,S1_SHADOW,
};
//const uint8_t _sky_goal0[] PROGMEM = { S0,15,S0,S0_SHADOW };    // TODO
//const uint8_t _sky_goal1[] PROGMEM = { S1,15,S1,S1_SHADOW };

inline
void setup_grid(uint8_t y)
{
    // Sky above grid
    if (y <= L_HORIZON)
    {
        y = L_HORIZON-y;
        if (y > 3)
            y = 3;
        _grid_buf[1] = pgm_read_byte(_skytab + y + _render_params.sky);
        return; // TODO
    }
    
    //  Grid itself
    y = (y-L_HORIZON)+3;
    short z;
    uint8_t width;
#ifdef GODSEYE
    int yy = y;
    yy = (yy<<1) + _odd;
    z = ((L_CAMERA_Z*L_CAMERA_Y<<1) + (yy >> 1))/yy;  // Table
    width = (L_CAMERA_Z*CELL_SIZE + (z >> 1))/z;     // Width of cell at this z
    //printf("%d, // %d\n",z,y);
    //printf("%d, // %d\n",width,y);
#else
    y -= 4;
    if (_odd)
        y += (40-3);
    z = pgm_read_word(&_ytoz[y]);
    width = pgm_read_byte(&_ytowidth[y]);
#endif
    z = z + _render_params.gpy;
    
    uint8_t *dst = _grid_buf;
    *dst++ = 0;
    *dst++ = GROUND; // BG
    
    short x = 0;
    if (!(z < 0 || z >= (_render_params.gridy << 8)))
    {
        short px = _render_params.gpx;
        uint8_t fill = ((z >> 8)&1)^1;         // /CELL_SIZE add phase to fill
        x = WIDTH/2;
        uint8_t scroll = px & (CELL_SIZE-1);
        x -= ((uint16_t)width * scroll) >> 8;  // /CELL_SIZE Center (TODO check divide)
        
        int8_t ppx = px >> 8;
        while (x > 0 && ppx > 0)  // TODO. can spin here a bit
        {
            x -= width;
            --ppx;
        }
        fill ^= ppx&1;
        
        uint8_t gridx = _render_params.gridx;
        while (x < WIDTH && ppx < gridx)
        {
            if (ppx >= 0)
            {
                DRAWGRID(fill ? 0x2 : 0x7);
            }
            x += width;
            ppx++;
            fill ^= 1;
        }
    }
    if (x < WIDTH)
    {
        DRAWGRID(GROUND);
    }
    *dst++ = WIDTH;
    
    ASSERT(dst-_grid_buf <= sizeof(_grid_buf));
}

//============================================================
//============================================================
//   Balls

PROGMEM
const uint8_t _circle_steps[] = {
    0,    // 1
    2,2,    // 2
    2,2,2,    // 3
    2,4,0,2,    // 4
    4,2,2,2,0,    // 5
    4,2,2,2,2,0,    // 6
    4,4,2,2,0,2,0,    // 7
    4,4,2,2,2,0,2,0,    // 8
    4,4,4,2,0,2,0,2,0,    // 9
    6,4,2,2,2,2,0,2,0,0,    // 10
    6,4,2,4,0,2,2,0,2,0,0,    // 11
    6,4,4,2,2,2,0,2,0,2,0,0,    // 12
    6,6,2,2,2,2,2,2,0,0,2,0,0,    // 13
    6,6,2,4,2,2,0,2,2,0,0,2,0,0,    // 14
    6,6,4,2,2,2,2,2,0,2,0,0,2,0,0,    // 15
    6,6,4,2,4,2,0,2,2,0,2,0,0,2,0,0,    // 16
    8,4,4,4,2,2,2,2,0,2,0,2,0,2,0,0,0,    // 17
    8,6,4,2,2,2,2,2,2,2,0,2,0,0,2,0,0,0,    // 18
    8,6,4,2,4,2,2,2,0,2,2,0,2,0,0,2,0,0,0,    // 19
    8,6,4,4,2,2,2,2,2,2,0,2,0,2,0,0,2,0,0,0,    // 20
    8,6,4,4,2,4,2,2,0,2,2,0,2,0,2,0,0,2,0,0,0,    // 21
    8,6,6,2,4,2,2,2,2,2,0,2,2,0,0,2,0,0,2,0,0,0,    // 22
    8,8,4,4,2,2,4,2,0,2,2,2,0,2,0,2,0,0,0,2,0,0,0,    // 23
};

PROGMEM
const uint8_t _circle_fib[] = {
    0,1,3,6,10,15,21,28,36,45,55,66,78,91,105,120,136,153,171,190,210,231,253
};

inline
void init_ball(uint8_t ballHeight)
{
    if (ballHeight > 44)    // TODO. bigger balls?
        ballHeight = 44;
    edge.ball_size = ballHeight;
    edge.ball_top = pgm_read_byte(_circle_fib+(edge.ball_size >> 1));
    edge.ball_bottom = ballHeight;
    edge.ball_size = (ballHeight+1)>>1;
}

inline
int8_t circlex(int8_t y)
{
    const uint8_t* p = _circle_steps + edge.ball_top;
    if (y < edge.ball_size)
        return pgm_read_byte(p+y);
    return -pgm_read_byte(p+(edge.ball_bottom-y));
}

void print_edge(uint8_t j)
{
    printf("%d: %04X %d:%d %02X %02X\n",j,edge.x[j],edge.miny[j],edge.maxy[j],edge.dx[j],edge.color[j]);
}

void dump(uint8_t* list, uint8_t n)
{
    for (uint8_t i = 0; i < n; i++)
    {
        print_edge(list[i]);
    }
    printf("\n");
}

//============================================================
//============================================================
//   Rasterization
//   Rastersize edges into rle format used by the video kernel

void rasterize()
{
    // init bg/grid
    uint8_t *d = _grid_buf;
    *d++ = 0;
    *d++ = 0;
    *d++ = WIDTH;
    *d++ = 0;
    
    uint8_t em = 0;
    uint8_t miny = 0xFF;
    if (edge.index)
        miny = edge.miny[edge.sorted[0]];
    
    uint8_t activeCount = 0;
    int8_t y = 8;
    while (y--)
        edge.stack[y] = 0;
    
    //printf("%d of %d:%d\n",edge.index,edge.count0,edge.count1);
    //dump(edge.sorted,edge.index);
    
    uint8_t* dst = _write_rle;
    for (y = 0; y < HEIGHT; y++)                // Render from top to bottom of shape
    {
        setup_grid(y);
        _grid = _grid_buf;
        
        //printf("line %d\n",y);
        
        // Activate all the curves that start at this y
        while (miny <= y)
        {
            uint8_t e = edge.sorted[em++];
            edge.active[activeCount++] = e;
            if (em < edge.index)            // last one
                miny = edge.miny[edge.sorted[em]];
            else
                miny = 0xFF;
            //printf("Adding ");
            //print_edge(e);
        }
        
        //  Curves
        uint8_t j = 0;
        for (uint8_t i = 0; i < activeCount; i++)
        {
            uint8_t e = edge.active[i];
            if (edge.maxy[e] <= y)				// Remove at maxy
            {
                //printf("Removing ");
                //print_edge(edge.active[activeCount-1]);
                continue;
            }
            
            int8_t dx = edge.dx[e];
            int8_t c;
            if (!(dx & 2))
                c = dx >> 2;                     // Step edge
            else
            {
                uint8_t v = y-edge.miny[e];     // Render circle
                c = circlex(v);
                if (dx & 1)
                    c = -c;
                if (!v && (v_framecounter & 2))         // Flutter it
                    c += 3;
            }
            short x = edge.x[e] += c;
            
            // Insertion Sort
            int8_t k = j++;
            uint8_t* a = edge.active;
            for (; k > 0 && x < edge.x[a[k-1]]; k--)
                a[k] = a[k-1];
            a[k] = e;
        }
        activeCount = j;
        
        //printf("Active List:%d\n",activeCount);
        //for (uint8_t i = 0; i < activeCount; i++)
        //    print_edge(edge.active[i]);
        
        // Render active curves
        uint8_t x = 0;
        uint8_t color = 0;
        for (uint8_t i = 0; i < activeCount; i++)
        {
            uint8_t e = edge.active[i];
            short x16 = edge.x[e] >> 2;					// New edge
            if (x16 < 0)
                x16 = 0;
            if (x16 > WIDTH)
                x16 = WIDTH;
            uint8_t xx = x16;
            
            // color has layer information in top nybble
            // 8 Z ordinals
            uint8_t old_color = color;
            uint8_t c = edge.color[e];
            uint8_t layer =c >> 2;        // ewwww
            if (edge.dx[e] & 1)
            {
                // leading edge
                edge.stack[layer >> 2] = c;
                if (c > color)
                    color = c;   //
            } else {
                // trailing edge
                layer >>= 2;
                edge.stack[layer] = 0;
                if ((c & 0xF0) == (color & 0xF0))  // was layer in front?
                {
                    color = 0;
                    while (layer && !color)
                        color = edge.stack[--layer];   // pop down stack
                }
            }
            if ((color != old_color) && (xx > x))
            {
                if (old_color == 0)
                    dst = emit_bg(x,xx,dst);
                else
                    dst = emit(x,xx,old_color & 0x0F,dst);
                x = xx;
            }
        }
        
        if (x < WIDTH)
            dst = emit_bg(x,WIDTH,dst);
        
        ASSERT(dst <= (_bb_buf + BUFFER_SIZE));
    }
    _write_rle = dst;
    //if (_bb_phase && (_write_rle > _read_rle) && video_current_line() < 192)
    //  _dbg1++;  // Overrun
    // printf("%d remain\n",_bb_buf + sizeof(_bb_buf)-_write_rle);
}


//============================================================
//============================================================
//   Models/Edges

#define L1 (1 << 4) // GOAL
#define L2 (2 << 4) // BALL_FAR    (Far away)
#define L3 (3 << 4) // BODY
#define L4 (4 << 4) // WINDSHEILD
#define L5 (5 << 4) // BALL_CLOSE    (Close)

PROGMEM
const uint8_t _shipPal0[] = { S0,S0_SHADOW,S0,S0,S0_SHADOW,S0,S0,S0};
PROGMEM
const uint8_t _shipPal1[] = { S1,S1_SHADOW,S1,S1,S1_SHADOW,S1,S1,S1};
PROGMEM
const uint8_t _shipWindshieldPal0[] = { W,W,W,W };
PROGMEM
const uint8_t _ballPal[] = { B,B,B,B,15,15,15,15,B,B,B,B,15,15,15,15 };
PROGMEM
const uint8_t _goalPal0[] = { S0,S0,S0,S0,15,15,15,15, S0,S0,S0,S0, S0_SHADOW,S0_SHADOW,S0_SHADOW,S0_SHADOW };
PROGMEM
const uint8_t _goalPal1[] = { S1,S1,S1,S1,15,15,15,15, S1,S1,S1,S1, S1_SHADOW,S1_SHADOW,S1_SHADOW,S1_SHADOW };

//  ships are 1/2 a cell wide
//  Supports 1:1,1:2 diagonals for now (see dx calculation below)
PROGMEM
const int8_t _shipBodyRight[] = {
    7,
    -64,0,
    -32,-16,
    -64,-32,
    -64,-96,
    64,-32,
    32,-16,
    64,0,
    
    -3,
    5,
    1,
    4,
    -4,
    4,
    0x00,
};

PROGMEM
const int8_t _shipWindshieldRight[] = {
    3,
    -40,-42,        // glass
    -40,-82,
    40,-42,
    
    1,
    4,
    0x00
};

PROGMEM
const int8_t _shipBodyLeft[] = {
    7,
    -64,0,
    -32,-16,
    -64,-32,
    64,-96,
    64,-32,
    32,-16,
    64,0,
    
    -3,
    5,
    -3,
    0,
    -4,
    4,
    0x00
};

PROGMEM
const int8_t _shipWindshieldLeft[] = {
    3,
    -40,-42,    // glass
    40,-82,
    40,-42,
    
    -3,
    0,
    0x00
};

PROGMEM
const int8_t _shipBodyFront[] = {
    7,
    -64,0,
    -32,-16,
    -64,-32,
    0,-96,
    64,-32,
    32,-16,
    64,0,
    
    -3,
    5,
    -1,
    2,
    -4,
    4,
    0x00,
};

PROGMEM
const int8_t _shipWindshieldFront[] = {
    3,
    // glass
    -42,-42,
    0,-82,
    42,-42,
    
    -1,
    2,
    0x00,
};

// just a vertical line until render draws circle
PROGMEM
const int8_t _balls[] = {
    3,
    0,-48,
    0,-48-64,   // ball is 64 (tank is 96 high) 1/4 width of tank 256
    0,-48,
    
    1,
    0x00,
    0x00,
};

PROGMEM
const int8_t _goal[] = {
    4,
    -GOAL_THICKNESS/2,0,
    -GOAL_THICKNESS/2,-GOAL_HEIGHT,
    GOAL_THICKNESS/2,-GOAL_HEIGHT,
    GOAL_THICKNESS/2,0,
    
    1,
    0x00,
    0,
    0x00
};

// Map observer to observed orientation
PROGMEM
const uint8_t _orientation_map[] = {
    // DIR_UP
    DIR_DOWN,DIR_LEFT,DIR_UP,DIR_RIGHT,
    // DIR_LEFT
    DIR_RIGHT,DIR_DOWN,DIR_LEFT,DIR_UP,
    // DIR_DOWN
    DIR_UP,DIR_LEFT,DIR_DOWN,DIR_RIGHT,
    // DIR_RIGHT
    DIR_RIGHT,DIR_UP,DIR_LEFT,DIR_DOWN,
};

//#define BALL_TABLE
#ifdef BALL_TABLE
//============================================================
//============================================================
//  Generate table for drawing ball
// Calc circle quadrant
void Circle(int radius)
{
	int error = -radius;
	uint8_t x = radius;
	uint8_t y = 0;
    uint8_t circleBuf[32];
    
    while (x >= y)
	{
		error += y;
        circleBuf[radius-y] = x<<1;
        circleBuf[radius-x] = y<<1;
		error += ++y;
		if (error >= 0)
		{
			--x;
			error -= x<<1;
		}
	}
    
    // Center line? odd height case?
    int8_t i;
    x = 0;
    for (i = 0; i < radius; i++)
    {
        
        int8_t dx = circleBuf[i]-x;
        printf("%d,",dx);
        x = circleBuf[i];
    }
    printf("    // %d\n",radius);
}

void make_ball_table()
{
    for (uint8_t i = 1; i < 24; i++)
        Circle(i);
    
    int off = 0;
    for (uint8_t i = 1; i < 24; i++)
    {
        printf("%d,",off);
        off += i;
    }
    printf("\n");
}
#endif

//============================================================
//============================================================

class BallBlaster : public Game
{
    virtual void init()
    {
        _game.goal_pos = GRID_X*CELL_SIZE/2;
        _game.lfsr = 1;
#ifdef BALL_TABLE
        make_ball_table();
#endif
        set_state(SPLASH);
    }
    
    void init_obj(Obj& o, short x, short y)
    {
        memset(&o,0,sizeof(Obj));
        o.x = x;
        o.y = y;
    }
    
    void reset_round()
    {
        init_obj(_game.ball,MAX_X,GRID_Y*CELL_SIZE/2);
        init_obj(_game.ship[0],GRID_X*CELL_SIZE/2,GRID_Y*CELL_SIZE/4);
        init_obj(_game.ship[1],GRID_X*CELL_SIZE/2,GRID_Y*CELL_SIZE-GRID_Y*CELL_SIZE/4);
    }

    void reset()
    {
        reset_round();
        _game.game_time = 3*60*60;  // Selectable
        _game.score0 = _game.score1 = 5;
       //_game.game_time = 1*60;
       // _game.score0 = 0;
    }
    
    virtual uint8_t get_tile(uint8_t x, uint8_t y)
    {
        return 0;
    }
    
    virtual void step()
    {
        short ddx = 0;
        short ddy = 0;
        short ddx2 = 0;
        short ddy2 = 0;
        
        uint16_t* j = readJoy();
        
        uint16_t key = j[0];
        if (key) {
            if (_game.state == PREPLAY)
                key &= ~(UP|DOWN);  // No forward/back during PREPLAY
          if (key & FIRE) _game.ship[0].flags |= BALL_PUSH;
          if (key & UP) ddy = PULSE;
          if (key & DOWN) ddy = -PULSE;
          if (key & LEFT) ddx = -PULSE;
          if (key & RIGHT) ddx = PULSE;
        }
        key = j[1];
        if (key) {
            if (_game.state == PREPLAY)
                key &= ~(UP|DOWN);  // No forward/back during PREPLAY
          if (key & FIRE) _game.ship[1].flags |= BALL_PUSH;
          if (key & UP) ddy2 = PULSE;
          if (key & DOWN) ddy2 = -PULSE;
          if (key & LEFT) ddx2 = -PULSE;
          if (key & RIGHT) ddx2 = PULSE;
        }
        
        // orient pulse to ships direction
        _game.ship[0].orient(&ddx,&ddy);
        _game.ship[1].orient(&ddx2,&ddy2);
        
        // Calculate forces between ships
        short fx = _game.ship[0].x - _game.ship[1].x;
        short fy = _game.ship[0].y - _game.ship[1].y;
        if (abs(fx) < 0x100 || abs(fy) < 0x100) {
            long dst = (long)fx*fx + (long)fy*fy;
            if (dst < 0x10000) {
                uint8_t force = 0x100-(dst>>8);   // repulsive force
                fx = (long)fx*force;
                fy = (long)fy*force;
                ddx += fx;
                ddy += fy;
                ddx2 -= fx;
                ddy2 -= fy;
            }
        }
        
        // Attract ball to ships
        Obj& ball = _game.ball;
        
        if ((_game.ship[0].flags | _game.ship[1].flags) & BALL_PUSH) // Distance
            _game.pushed_from = ball.y >> 8;
        
        short ddx_ball=0,ddy_ball=0;
        uint8_t proximity0 = _game.ship[0].attract(ball,&ddx_ball,&ddy_ball,DIR_UP);
        uint8_t proximity1 = _game.ship[1].attract(ball,&ddx_ball,&ddy_ball,DIR_DOWN);
        
        // buzz if ball is close (captured?)
        if (_game.state == PLAYING)
        {
            uint8_t buzz = MIN(proximity0,proximity1);
            music_volume(0,(255-buzz)>>1);  // LFO would be nice here
        }
        
        if ((_game.ship[0].flags | _game.ship[1].flags) & BALL_CAPTURED)
        {
            _game.pushed_from = 0xFF;
            if (_game.ship[0].flags & BALL_CAPTURED)
                _game.ship[1].flags &= ~BALL_CAPTURED; // TODO. NOT FAIR
            
            //ddx_ball -= _ball.dx >> 6;  // damp against current velocity
            ddy_ball -= ball.dy >> 4;
        }
        
        // push ball
        ball.move(ddx_ball,ddy_ball);
        ball.collide(abs(abs(_game.goal_pos) - ball.x) < goal_width2());
        
        // Scored!
        uint8_t scored = ball.flags & (SCORE_0 | SCORE_1);
        if (scored && _game.state == PLAYING)
        {
            set_state(GOAL);
            uint8_t points = 1;
            points = 10;
            if (_game.pushed_from != 0xFF)
            {
                int8_t d = _game.pushed_from;   // Long shots score more
                if (scored & SCORE_0)
                    d = GRID_Y - d;
                points = 1+(d>>3);
                points = MIN(points,3);
                _game.pushed_from = 0xFF;
            }
            if (scored & SCORE_0) {
                _game.score0 += points;
                _game.score1 = MIN(_game.score1,10-_game.score0);
            } else {
                _game.score1 += points;
                _game.score0 = MIN(_game.score0,10-_game.score1);
            }

            if (_game.game_time < 0) {
                if (_game.score0 != _game.score1)
                    set_state(WIN);
            } else {
                if (_game.score0 >= 10 || _game.score1 >= 10)
                    set_state(WIN);
            }
        }
        
        _game.ship[0].move(ddx,ddy,1);
        _game.ship[0].collide(false);
        
        _game.ship[1].move(ddx2,ddy2,1);
        _game.ship[1].collide(false);
        
        // Move goals
        short gw = -goal_width2();
        if (_game.goal_pos > 0)
            gw += CELL_SIZE*GRID_X;
        if (_game.goal_pos < gw)
            _game.goal_pos += 2;
        else
            _game.goal_pos = -_game.goal_pos;
    }
    
    // 1/2 goal width
    short goal_width2()
    {
        //uint8_t goals =  (_game.ship0.flags & 0xF) + (_game.ship0.flags & 0xF);
        return CELL_SIZE;
    }
    
    inline
    short project(short xy)
    {
        // return xy >> 2;
        short s = (long)xy*_render_params.z >> 8;   // better or worse with rounding?
        return s >> 2;
        //return (long)L_CAMERA_Z*xy/_zz;
    }
    
    // Sloppy way of generating dir/steps
    void project_precalc(const int8_t* d, const uint8_t* palette)
    {
        printf("\n");
        uint8_t count = pgm_read_byte(d++);
        short* pix = (short*)_grid_buf;
        for (uint8_t i = 0; i < count; i++) {
            short xx = (int8_t)pgm_read_byte(d++);
            short yy = (int8_t)pgm_read_byte(d++) << 1;    // x is 2x wide pixels, compensate TODO in data
            *pix++ = xx;
            *pix++ = yy;
        }
        pix = (short*)_grid_buf;
        pix[count << 1] = pix[0];
        pix[(count << 1)+1] = pix[1];
        while (count--)
        {
            short y0 = pix[1];
            short y1 = pix[3];
            short dx = pix[2]-pix[0];
            int8_t dir = 0;
            if (y0 > y1)
            {
                dx = -dx;
                dir = 1;
                y0 = pix[3];
                y1 = pix[1];
            }
            if (y0-y1)
                printf("%d,\n",((dx*2/(y1-y0)) << 1) | dir);
            else
                printf("0x00,\n");
            pix += 2;
        }
    }
    
    // TODO. clip over the horizion
    void project(const int8_t* d, const uint8_t* palette, uint8_t layer)
    {
        //project_precalc(d,palette);
        short* pix = (short*)_grid_buf;
        uint8_t circle = d == _balls ? 2 : 0;
        
        // Over the horizon hack
        uint8_t voff = _render_params.z >> 2;
        if (voff < 16)
            voff = 4-(voff >> 2);
        else
            voff = 0;
        
        uint8_t count = pgm_read_byte(d++);
        for (uint8_t i = 0; i < count; i++) {
            short xx = (int8_t)pgm_read_byte(d++);
            short yy = (int8_t)pgm_read_byte(d++) << 1;    // x is 2x wide pixels, compensate TODO in data
            if (_odd)
                yy++;  // called before _odd is incremented
            *pix++ = WIDTH/2 + project(_render_params.x+xx);
            *pix++ = L_HORIZON + -3 + project(L_CAMERA_Y+yy) + voff;
        }
        pix = (short*)_grid_buf;
        pix[count << 1] = pix[0];
        pix[(count << 1)+1] = pix[1];
        
        // Force goals to be visible (at least 1 pix wide and same width)
        if (d == _goal+9)
        {
            if (pix[4] <= 0)
                return;
            uint8_t goal_width = project(GOAL_THICKNESS);
            if (goal_width == 0)
                goal_width = 1;
            uint8_t w = pix[0] + goal_width;
            pix[4] = pix[6] = w;
        }
        
        // Activate edges
        while (count--)
        {
            short y0 = pix[1];
            short y1 = pix[3];
            int8_t dx = (int8_t)pgm_read_byte(d++);
            uint8_t color = pgm_read_byte(palette++);
            if (y0 == y1) {
                pix += 2;
                continue;
            }
            
            uint8_t dir = dx & 1;
            short x;
            if (dir)
            {
                y0 = pix[3];    // -ve is 1 flag
                y1 = pix[1];
                x = pix[2];     // 1:4 subpixel pos
            } else
                x = pix[0];
            
            if (voff >= 2)
                y1 = MIN(y1,L_HORIZON); // Clip if over the horizon
            
            if (y1 <= 0 || y0 >= HEIGHT || y0 >= y1)
            {
                pix += 2;
                continue;       // Not visible
            }
            
            if (circle && dir)
                edge.ball_size = y1-y0;
            
            dx &= 0xFE;
            if (y0 < 0)
            {
                if (circle)
                {
                    // printf("DONT CLIP ME\n");
                    return;
                } else
                    x -= y0*dx;
                y0 = 0;
            }
            
            // Going to draw this edge.
            uint8_t i = edge.index;
            edge.x[i] = (x << 2) - (dx >> 1);
            edge.miny[i] = y0;
            edge.maxy[i] = y1 < 255 ? y1 : 255;
            edge.dx[i] = (dx << 2) | circle | dir;
            edge.color[i] = color | layer;
            
            // Insertion sort by miny
            i -= edge.count0;
            for (; i > 0 && y0 < edge.miny[edge.sorted[i-1]]; i--)
                edge.sorted[i] = edge.sorted[i-1];
            edge.sorted[i] = edge.index++;
            pix += 2;
        }
    }
    
    void flush()
    {
#ifndef ARDUINO
        for (int y = 0; y < 192/2; y++)
            flush_video(true);
#endif
    }
    
    inline
    void set_view(Obj& ship)
    {
        _render_params.px = ship.x;
        _render_params.py = ship.y;
        _render_params.orientation = ship.orientation;
        
        // Setup grid draw from this perspective
        switch (_render_params.orientation)
        {
            case DIR_UP:
            case DIR_DOWN:
                _render_params.gridx = GRID_X;
                _render_params.gridy = GRID_Y;
                _render_params.gpx = _render_params.px;
                break;
            default:
                _render_params.gridx = GRID_Y;
                _render_params.gridy = GRID_X;
                _render_params.gpx = _render_params.py;
        }
        
        switch (_render_params.orientation)
        {
            case DIR_UP:
                _render_params.gpy = _render_params.py;
                break;
            case DIR_DOWN:
                _render_params.gpy = (_render_params.gridy << 8)-_render_params.py;
                break;
            case DIR_LEFT:
                _render_params.gpy = (_render_params.gridy << 8)-_render_params.px;
                break;
            case DIR_RIGHT:
                _render_params.gpy = _render_params.px;
                break;
        }
    }
    
    // Clip, setup z,x in renderparams
    bool setup_object(Obj& obj)
    {
        short x,z;
        {
            // Setup camera from this perspective
            // Calculate relative z and x of object to be drawn
            short ox = obj.x - _render_params.px;
            short oy = obj.y - _render_params.py;
            switch (_render_params.orientation)
            {
                case DIR_UP:
                    z = oy;
                    x = ox;
                    break;
                case DIR_DOWN:
                    z = -oy;
                    x = ox;
                    break;
                case DIR_RIGHT:
                    x = oy;
                    z = ox;
                    break;
                case DIR_LEFT:
                    x = oy;
                    z = -ox;
                    break;
            }
        }
        if (z <= CELL_SIZE/2 || z >= 8192)
            return false;                       // Can't be seen (too close or behind camera or too far away)
        
        _render_params.z = 0x100L*L_CAMERA_Z*4L/z;    // Project with a multiply
        
        short left = project(x-CELL_SIZE/4);    // CELL_SIZE/2 is also max object width
        if (left >= WIDTH/2)
            return false;                       // Off right edge
        
        short right = project(x+CELL_SIZE/4);
        if (right <= -WIDTH/2)
            return false;                       // Off left edge
        
        _render_params.x = x;                   // probably visble. draw it
        return true;
    }
    
    void draw_goal(short y)
    {
        const uint8_t* p = y ? _goalPal0 : _goalPal1;
        p += v_framecounter & 0xC;  // palette
        
        Obj goal;
        goal.y = y;
        goal.x = _game.goal_pos < 0 ? -_game.goal_pos : _game.goal_pos;
        short gw = goal_width2();
        goal.x -= gw;
        if (setup_object(goal)) {
            if (_render_params.z < 10)
                return;
            project(_goal,p,L1);
        }
        goal.x += gw*2;
        if (setup_object(goal)) {
            if (_render_params.z < 10)
                return;
            project(_goal,p,L1);
        }
    }
    
    void draw_object(uint8_t which, Obj& obj, bool ballFar = false)
    {
        if (which == BALL_OBJ)
        {
            const uint8_t* p = _ballPal; // Animate ball
            if (_game.ship[0].flags)
                p = _goalPal1;
            if (_game.ship[1].flags)
                p = _goalPal0;
            project(_balls,p + (v_framecounter & 0xC),ballFar ? L2 : L5);
            return;
        }
        
        const uint8_t* pal = which ? _shipPal0 : _shipPal1;
        uint8_t index = pgm_read_byte(_orientation_map + (_render_params.orientation << 2) + obj.orientation);
        switch (index) {
            case DIR_LEFT:
                project(_shipBodyLeft,pal,L3);
                project(_shipWindshieldLeft,_shipWindshieldPal0,L4);
                break;
            case DIR_RIGHT:
                project(_shipBodyRight,pal,L3);
                project(_shipWindshieldRight,_shipWindshieldPal0,L4);
                break;
            case DIR_UP:
                project(_shipBodyFront,pal,L3);
                project(_shipWindshieldFront,_shipWindshieldPal0,L4);
                break;
            case DIR_DOWN:
                project(_shipBodyFront,pal,L3);
                break;
        }
    }
    
    inline
    void set_sky(uint8_t index) // Animate sky after goal
    {
        uint8_t a = v_framecounter & 0xC;
        _render_params.sky = 0;
        if ((_game.ball.flags & SCORE_0) && (index == 0))
            _render_params.sky = a + 20;
        if ((_game.ball.flags & SCORE_1) && (index == 1))
            _render_params.sky = a + 4;
    }
    
    // debugging
    void mark(uint8_t n)
    {
        while (n--)
        {
            DBG1();
            DBG0();
        }
    }
    
    virtual void testcard()
    {
        while (video_current_line() != 192)  // wait for it to render
            DBG_TOGGLE();
        uint8_t* dst = _bb_buf;
        for (int i = 0; i < (192-16); i++)
        {
            dst = emit(0,WIDTH,i>>3,dst);
        }
        flush();
        flush();
    }
    
    void play_theme(uint8_t offset = 0)
    {
        music_play(_theme_init);
        music_play(_theme+offset);
    }
    
    void set_state(GameState state)
    {
        switch (state)
        {
            case SPLASH:
                reset();
                play_theme();
                draw_title();
                break;
            case PREPLAY:
                play_theme(PREPLAY_OFFSET);
                reset_round();
                break;
            case PLAYING:
                sound(_effect_init);
                sound(_ball_push);
                _game.ball.dx = -32760;
                break;
            case GOAL:
                music_play(_tada);
                break;
            case WIN:
                music_play(_win);
                break;
            case POSTWIN:
                play_theme();
                break;
            default:
                ;
        }
        _game.state = state;
    }
    
    // Loop music
    void music()
    {
        music_kernel();
        
        // A little random
        int16_t lfsr = _game.lfsr;
        _game.lfsr = (lfsr << 1) ^ ((lfsr < 0) ? 0x2D : 0x00);
        
        // Tat tat tat timer
        if (_game.state == PLAYING)
        {
            if (++_game.tat_timer >= 6)
            {
                _game.tat_timer = 0;
                music_note(3,127,(_game.lfsr & 0xF) + 2);
            }
        }
        
        if (!music_playing()) {
            // Loop
            switch (_game.state) {
                case SPLASH:
                case PLAYER_SELECTION:
                case GOAL:
                    set_state(PREPLAY);
                    break;
                    
                case PREPLAY:
                    set_state(PLAYING);
                    break;
                    
                case WIN:
                    set_state(POSTWIN);
                    break;

                case POSTWIN:
                    reset();
                    draw_title();
                    set_state(PREPLAY);   // Reset game
                    break;

                default:
                    break;
            }
        }
    }
    
    void draw_ship_view(uint8_t ship)
    {
        Obj& s = _game.ship[ship];
        _render_params.px = s.x;
        _render_params.py = s.y;
        _render_params.orientation = s.orientation;
        
        if (ship) {
            edge.sorted = edge.sorted1;
            edge.ball0_radius = edge.ball_size;
        } else {
            edge.sorted = edge.sorted0;
            edge.index = edge.count0 = edge.count1 = edge.ball_size = 0;
        }
        
        Obj& obj = _game.ship[1-ship];
        short shipz = 0;
        if (setup_object(obj))
        {
            shipz = _render_params.z;
            draw_object(1-ship,obj); // Draw opposing ship
        }
        
        if (_game.state > PREPLAY)
        {
            if (setup_object(_game.ball))
                draw_object(BALL_OBJ,_game.ball,_render_params.z < shipz);
        }
        
        if (ship == 0) {
            if (_render_params.orientation == DIR_UP)
                draw_goal(GRID_Y*CELL_SIZE);
            edge.count0 = edge.index;
        }
        if (ship == 1) {
            if (_render_params.orientation == DIR_DOWN)
                draw_goal(0);
            edge.count1 = edge.index - edge.count0;
            edge.ball1_radius = edge.ball_size;
        }
    }
    
    int render(uint8_t i)
    {
        DBG1();
        set_view(_game.ship[i]);
        set_sky(i);
        if (i == 0)
        {
            edge.sorted = edge.sorted0;
            edge.index = edge.count0;
            init_ball(edge.ball0_radius);
        } else {
            edge.sorted = edge.sorted1;
            edge.index = edge.count1;
            init_ball(edge.ball1_radius);
        }
        rasterize();
        
        //  int a = (int)(_write_rle-_read_rle);
        // printf("%d,%d\n",_read_rle-_bb_buf,_write_rle-_bb_buf);
        int a = i == 0 ? _write_rle-_bb_buf:_write_rle-_read_rle;
        flush();
        DBG0();
        return a;
    }
    
    virtual void draw()
    {
        // testcard();
        // return;
        
        // Project objects
        uint8_t t = _line_count;
        //for (uint8_t i = 0; i < 2; i++)
        draw_ship_view(0);
        t = _line_count-t;
        //_dbg[0] = t;
        
        t = _line_count;
        //for (uint8_t i = 0; i < 2; i++)
        draw_ship_view(1);
        t = _line_count-t;
        _dbg[1] = t;
        
        while (video_current_line() != 192)  // wait for it to render
            DBG_TOGGLE();
        _odd ^= 1;
        
        //draw_debug();
#if 1
        // play music
        music();
        
        //  update time
        if (_game.game_time < 0 && _game.state == PLAYING)   // ran out of time - win or overtime
        {
            if (_game.score0 != _game.score1)
                set_state(WIN);
        }
        if (_game.state > PLAYER_SELECTION)   // TODO
            draw_time();
#endif
        
        // now in blanking POST/VBI/PRE
        
        _write_rle = _bb_buf;
        
        t = _line_count;
        render(0);
        t = _line_count-t;
        _dbg[2] = t;
        
        t = _line_count;
        render(1);
        t = _line_count-t;
        _dbg[3] = t;
        
        //int total = _write_rle -_bb_buf;
        //if (total > 1400)
        //    printf("total %d\n",total);
    }
};

/*
 TODO
 droid
 // Option cycle thru p1, time, p2
 
 // push field is on-axis
 //
 
 starts flat on maxtime for 3 seconds
 ball fires
 ship glow on (fire/ball acqu
 after win rapid orientation changes that slow over time
 
 // After win
 // show time/alternate splash ~ 1 seconds
 */

BallBlaster _ballblaster;
