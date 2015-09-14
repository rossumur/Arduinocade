//
//  titan.cpp
//
//  Created by Peter Barrett on 7/30/14.
//  Copyright (c) 2014 Peter Barrett. All rights reserved.
//

#include "config.h"

#include "music.h"
#include "sprites.h"
#include "video.h"

#include "titan.h"
#include "titan_tiles.h"

#ifdef BALLBLASTER_GAME
#error Please undefine BALLBLASTER_GAME in arduinocade/config.h
#endif

#define VULNERABLE true

#define COLS 40
#define WIDTH (4*COLS)

#define MAX_OBJS 16
#define GRAVITY 0x1
#define THRUST_H_FORCE 0x20
#define THRUST_V_FORCE 0x20
#define KEY_REPEAT_TICKS 20

#define MAX_FUEL 4096
#define HUD_HEIGHT 2
#define TOTAL_CELL_HEIGHT (16*16-(24-HUD_HEIGHT))    // 10 screens is very restrictive

uint16_t _v_scroll_cell = 0;    // verical scroll in 8x8 tiles
int8_t _v_scroll_line = 0;     // 0..7

uint16_t* readJoy();

// Sounds

static
void sound(const uint8_t* m, bool force = false)
{
    if (!music_playing() || force)
        music_play(m,0);
}

PROGMEM
const uint8_t s_effect_init[] = {
    M_WAVE,     (SINE << 4)   | CH1,    // engine hum
    M_ENVELOPE, (NONE << 4)   | CH1,
    M_VOLUME, CH1, 3,
    M_END
};

PROGMEM
const uint8_t s_boom_1[] = {
    M_WAVE, (RANDOM << 4) | CH4,
    M_NOTES_ON | CH4, 128+16, 	//
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_SLIDE, CH4, 1,
    M_DELAY_15,
    M_NOTES_ON | CH4, 128+2, 	//
    M_DELAY_15,
    M_NOTES_OFF | CH4,
};

PROGMEM
const uint8_t s_boom_2[] = {
    M_WAVE, (RANDOM << 4) | CH4,
    M_NOTES_ON | CH4, 128+20, 	//
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_SLIDE, CH4, 256-1,
    M_DELAY_15,
    M_NOTES_ON | CH4, 128+10, 	//
    M_DELAY_15,
    M_NOTES_OFF | CH4,
};

PROGMEM
const uint8_t s_boom_3[] = {
    M_WAVE, (RANDOM << 4) | CH3 | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_NOTES_ON | CH4, 60, 	// 
    M_NOTES_ON | CH3, 128+61, 	// 
    M_SLIDE, CH3, 256-1,
    M_SLIDE, CH4, 256-2,
    M_DELAY_30,
    M_NOTES_OFF | CH3,
    M_NOTES_OFF | CH4,
};

PROGMEM
const uint8_t s_fire[] = {
    M_WAVE, (RANDOM << 4) | CH2,
    M_ENVELOPE, (FADE << 4) | CH2,
    M_NOTES_ON | CH2, 80,
    M_SLIDE, CH2, 256-3,
    M_DELAY_2,
    M_WAVE, (SINE << 4) | CH2,
    M_ENVELOPE, (FADE << 4) | CH2,
    M_NOTES_ON | CH2, 80,
    M_SLIDE, CH2, 256-5,
    M_DELAY_3,
};

PROGMEM
const uint8_t s_klaxon[] = {
    M_WAVE, (SQUARE << 4) | CH3,
    M_NOTES_ON | CH3, 50, 	//
    M_SLIDE, CH3, 1,
    M_DELAY_15,
    M_WAVE, (SINE << 4) | CH3,
    M_DELAY_15,
    M_NOTES_OFF | CH3
};

PROGMEM
const uint8_t s_lasergate[] = {
    M_WAVE, (TRIANGLE << 4) | CH3,
    M_WAVE, (RANDOM << 4) | CH4,
    M_NOTES_ON | CH3 | CH4, 70, 1, 	//
    M_SLIDE, CH4, 5,
    M_DELAY_30,
    M_NOTES_OFF | CH3 | CH4
};

//================================================================================
//================================================================================

// Tile patch list: madness to accomodate no framebuffer
//#define MAX_PATCHES 32
#define MAX_PATCHES 40
typedef struct {
    uint8_t x,y,tile;
} Patch;
Patch _patch[MAX_PATCHES];
uint8_t _patch_count;

// World is built of segments 16 tiles high
// * work as entrances
enum {
    T_BLANK,
    T_ENTRY, // *
    T_RIGHT,
    T_LEFT, // *
    
    T_MIDDLE,
    T_LASER,
    T_WIGGLE, // *
    T_WIGGLE_MIDDLE,
    
    T_BOMB0,
    T_BOMB1,
    T_SHIPS0,
    T_SHIPS1,
    T_TRIPLE_LASER,

    T_TITLE = 15
};

//================================================================================
//================================================================================
// Level 0
PROGMEM
const uint8_t _level0[] = {
    T_TITLE,
    T_BLANK,
    T_ENTRY,
    T_RIGHT,

    T_LEFT,
    T_RIGHT,
    T_LEFT,
    T_MIDDLE,

    T_LASER,
    T_RIGHT,
    T_LEFT,
    T_LASER,

    T_WIGGLE,
    T_LASER,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_SHIPS0,
    T_SHIPS1,
    T_SHIPS0,

    T_BLANK,
    T_BOMB0,
    T_BOMB1,
};

PROGMEM
const uint8_t _level0_return[] = {
    T_TITLE,
    T_BLANK,
    T_ENTRY,
    T_RIGHT,

    T_LEFT,
    T_RIGHT,
    T_LEFT,
    T_MIDDLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_BOMB0,
    T_BOMB1,
};

//================================================================================
//================================================================================
// level 1

PROGMEM
const uint8_t _level1[] = {
    T_TITLE,
    T_BLANK,
    T_LEFT,
    T_RIGHT,
    T_WIGGLE,
    T_RIGHT,
    T_LEFT,
    T_LASER,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_BOMB0,
    T_BOMB1,
};

//================================================================================
//================================================================================
// level 2

PROGMEM
const uint8_t _level2[] = {
    T_TITLE,
    T_BLANK,
    T_WIGGLE,
    T_RIGHT,
    T_LEFT,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_SHIPS0,
    T_SHIPS1,
    T_SHIPS0,
    T_BLANK,

    T_LEFT,
    T_TRIPLE_LASER,
    T_RIGHT,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_SHIPS0,
    T_SHIPS1,
    T_SHIPS0,
    T_SHIPS1,
    T_BLANK,

    T_LEFT,
    T_TRIPLE_LASER,
    T_RIGHT,
    T_TRIPLE_LASER,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_BOMB0,
    T_BOMB1,
};

PROGMEM
const uint8_t _level2_return[] = {
    T_TITLE,
    T_BLANK,
    T_WIGGLE,
    T_RIGHT,
    T_LEFT,
    T_WIGGLE_MIDDLE,

    T_BLANK,

    T_LEFT,
    T_TRIPLE_LASER,
    T_RIGHT,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,

    T_LEFT,
    T_TRIPLE_LASER,
    T_RIGHT,
    T_TRIPLE_LASER,
    T_WIGGLE,
    T_WIGGLE_MIDDLE,

    T_BLANK,
    T_BOMB0,
    T_BOMB1,
};

//================================================================================
//================================================================================

const uint8_t* _level;
uint8_t _level_size;

//  world is broken into segments
inline
uint8_t get_segment(uint8_t y)
{
    return pgm_read_byte(_level + ((_v_scroll_cell + y) >> 4));
}

// TODO: cell vs tile
const uint8_t* get_row(uint8_t y)
{
    uint8_t seg = get_segment(y);
    return _map + ((seg<<4) + ((_v_scroll_cell + y) & 0xF)) * COLS;
}

// tileTransform
// map a tile index for animation
inline
uint8_t tileTransform(uint8_t t)
{
    if (t < 72)
        return t;

    if (t == 72 || t == 96 || t == 104)
        return t + ((v_framecounter >> 2) & 7); // slow 8 step animation tables

    // Laser gate
    if (t == 88)
    {
        if (v_framecounter & 0x40)
            return 1;
        return t + (v_framecounter & 7);        // fast 8 step animation
    }
    return t;
}

typedef struct
{
    uint8_t kind;
    uint8_t rider;
    uint8_t timer;
    uint8_t ai;
    short x,y;
    short dx,dy;
} JObj;

//================================================================================
//================================================================================
//  Explodables

enum {
    EXPLODABLE_NONE = 0,
    DOME,
    RED_FUEL,
    GREEN_FUEL,
    SMALL_FUEL,
    YELLOW_ROCKET,
    BLUE_ROCKET,
    LASER_GATE,
};

PROGMEM
const uint8_t _explodable_width[] = {
    0,2,3,3,1,2,2,11
};

PROGMEM
const uint8_t _explodable_height[] = {
    0,2,2,2,1,2,2,1
};

PROGMEM
const uint8_t _explodable_fuel[] = {
    0,0,255,160,80,0,255,0
};


PROGMEM
const uint8_t _explosion_tiles[] = {
    // 3x3
    112,113,114,
    128,129,130,
    
    115,116,117,
    131,132,133,
    
    118,119,120,
    134,135,136,

    // 2x2
    121,122,
    137,138,
    
    123,124,
    139,140,
    
    125,126,
    141,142,
    
    // 1x1
    127,143,
};

PROGMEM
const uint8_t _explosion_anim[] = {
    30,31,30,31,31,30,30,31,    // 1,1
    18,22,26,22,18,22,26,22,    // 2x2
    0,6,12,6, 0,12,0,6,         // 3x2
};

//================================================================================
// Generated by generate_explodables below

PROGMEM
const uint8_t _explodables[]={
    // segment 0
    // segment 1
    BLUE_ROCKET,6,3,
    YELLOW_ROCKET,9,3,
    DOME,32,3,
    DOME,36,3,
    DOME,16,12,
    DOME,19,12,
    // segment 2
    DOME,14,8,
    RED_FUEL,18,8,
    YELLOW_ROCKET,24,8,
    YELLOW_ROCKET,26,8,
    // segment 3
    SMALL_FUEL,7,0,
    SMALL_FUEL,9,0,
    SMALL_FUEL,11,0,
    RED_FUEL,13,4,
    GREEN_FUEL,20,6,
    DOME,24,6,
    // segment 4
    RED_FUEL,11,6,
    SMALL_FUEL,20,7,
    SMALL_FUEL,22,7,
    SMALL_FUEL,24,7,
    // segment 5
    LASER_GATE,14,7,
    LASER_GATE,14,10,
    // segment 6
    // segment 7
    DOME,22,3,
    GREEN_FUEL,14,7,
    // segment 8
    // segment 9
    // segment 10
    YELLOW_ROCKET,20,4,
    BLUE_ROCKET,12,5,
    YELLOW_ROCKET,36,5,
    YELLOW_ROCKET,6,7,
    BLUE_ROCKET,26,7,
    YELLOW_ROCKET,34,8,
    YELLOW_ROCKET,20,11,
    YELLOW_ROCKET,26,12,
    BLUE_ROCKET,12,14,
    // segment 11
    YELLOW_ROCKET,27,2,
    YELLOW_ROCKET,13,6,
    YELLOW_ROCKET,3,7,
    BLUE_ROCKET,33,7,
    BLUE_ROCKET,14,11,
    YELLOW_ROCKET,26,13,
    // segment 12
    LASER_GATE,14,3,
    LASER_GATE,14,8,
    LASER_GATE,14,13,
    // segment 13
    // segment 14
    // segment 15
};
PROGMEM
const uint8_t _explodables_index[]={
    0,	0,	18,	30,	48,	60,	66,	66,	72,	72,	72,	99,	117,	126,	126,	126,	126,
};

//================================================================================

#define SEGMENT_HEIGHT 16
#define MAX_EXPLODED 16
uint16_t _explodedlist[MAX_EXPLODED];
uint8_t _explodedcount = 0;

void get_explodable(const uint8_t* e, Explodable* ex)
{
    uint8_t k = ex->kind = pgm_read_byte(e);
    ex->x = pgm_read_byte(e+1);
    ex->y = pgm_read_byte(e+2);
    ex->width = pgm_read_byte(_explodable_width + k);
    ex->height = pgm_read_byte(_explodable_height + k);
}

bool detect_hit_one(const uint8_t* e, uint8_t x, uint8_t y)
{
    Explodable ex;
    get_explodable(e,&ex);
    return (ex.kind != LASER_GATE && x >= ex.x && (x < (ex.x + ex.width)) && (y >= ex.y) && (y < (ex.y + ex.height)));
}

//  did we hit something?
//  walk through explodable objects in the current segments
uint16_t detect_hit(uint8_t x, uint8_t y)
{
    uint16_t yy = _v_scroll_cell + y;
    uint8_t vseg = (yy >> 4);
    uint8_t seg = pgm_read_byte(_level + vseg);
    y = yy & (SEGMENT_HEIGHT-1);
    
    uint8_t from = pgm_read_byte(_explodables_index + seg);
    uint8_t to = pgm_read_byte(_explodables_index + seg + 1);
    for (uint8_t i = from; i < to; i += 3)
    {
        if (detect_hit_one(_explodables+i,x,y))
            return (vseg << 8) | i;             // vertical segment, index into explodables table
    }
    return 0;
}

//================================================================================
//================================================================================
//  Only run this code in simulator
//  Find explodables in map and emit a table

#ifdef GENERATE_EXPLODABLES
#include <vector>
using namespace std;

// look for explodable objects..
void pr(const char* name, int x, int y)
{
    printf("\t%s,%d,%d,\n",name,x,y);
}

// Dump objects by looking for their top left corners
// Create a data structure that lists explodables
void generate_explodables()
{
    vector<int> ptrs;
    int mark = 0;
    int slices = sizeof(_map)/(COLS*SEGMENT_HEIGHT);    //
    printf("PROGMEM\nconst uint8_t _explodables[]={\n");
    for (int s = 0; s < slices; s++) {
        printf("\t// segment %d\n",s);
        ptrs.push_back(mark*3);
        const uint8_t* m = _map + s*COLS*SEGMENT_HEIGHT;
        for (int y = 0; y < SEGMENT_HEIGHT; y++) {
            for (int x = 0; x < COLS; x++) {
                int t = m[y*COLS + x];
                switch (t) {
                    case 32:        // capitol
                        pr("DOME",x,y);
                        mark++;
                        break;
                    case 34:        // green fuel
                        pr("GREEN_FUEL",x,y);
                        mark++;
                        break;
                    case 37:        // tiny fuel
                        pr("SMALL_FUEL",x,y);
                        mark++;
                        break;
                    case 38:
                        pr("BLUE_ROCKET",x,y);
                        mark++;
                        break;
                    case 40:
                        pr("YELLOW_ROCKET",x,y);
                        mark++;
                        break;
                    case 43:        // red fuel
                        pr("RED_FUEL",x,y);
                        mark++;
                        break;
                    case 88:
                        if (m[y*COLS + x-1] != 88) {
                            pr("LASER_GATE",x,y);
                            mark++;
                        }
                        break;
                }
            }
        }
    }
    ptrs.push_back(mark*3);
    printf("};\n");
    
    printf("PROGMEM\nconst uint8_t _explodables_index[]={\n");
    for (int i = 0; i < ptrs.size(); i++)
        printf("\t%d,",ptrs[i]);
    printf("\n};\n");
}
#endif

//================================================================================
//================================================================================

enum {
    STATE_TITLE,
    STATE_DOWN,
    STATE_UP,
    STATE_BOMB,
    STATE_DEAD,
    STATE_NEXT_LEVEL,
    STATE_END
} GameState;

/*
extern uint8_t _dbg[4];
static inline void drawdbg(uint8_t* t, int n)
{
    t[1] = 144 + (n&0xF);
    n >>= 4;
    t[0] = 144 + (n&0xF);
}
void drawdebug(uint8_t* t)
{
    for (int8_t i = 0; i < 4; i++)
    {
        drawdbg(t,_dbg[i]);
        t[2] = 1;
        t += 3;
    }
    t[0] = 0;
}
*/

class Titan : public Game
{
    JObj _objs[MAX_OBJS];
    short _lfsr;
    uint16_t _fuel;
    
    uint8_t _game_state;
    uint8_t _game_timer;
    uint8_t _missile;
    uint8_t _level_index;
    
public:
    
    short randy()
    {
        int16_t lfsr = _lfsr;
        _lfsr = lfsr << 1;
        if (lfsr < 0)
            _lfsr ^= 0x2D;
        return _lfsr;   //
    }
    
    void init(uint8_t i,uint8_t k,uint8_t r,int x, int y, short dx = 0, short dy = 0)
    {
        _objs[i].kind = k;
        _objs[i].rider = r;
        _objs[i].x = x;
        _objs[i].y = y;
        _objs[i].dx = dx;
        _objs[i].dy = dy;
    }

    void sl(const uint8_t* d, int len)
    {
        _level = d;
        _level_size = len;
    }

    void set_level_data(uint8_t level)
    {
        switch (level) {
            case 0: sl(_level0,sizeof(_level0)); break;
            case 1: sl(_level0_return,sizeof(_level0_return)); break;
            case 2:
            case 3: sl(_level1,sizeof(_level1)); break;
            case 4: sl(_level2,sizeof(_level2)); break;
            case 5: sl(_level2_return,sizeof(_level2_return)); break;
            default:
                sl(_level0,sizeof(_level0));
        }
    }

    void start_level(uint8_t level)
    {
        _v_scroll_cell = 0;     // vertical scroll in 8x8 tiles, logo
        _v_scroll_line = 0;     // 0..7
        _explodedcount = 0;

        _missile = 0;
        _fuel = MAX_FUEL;
        
        memset(_objs,0,sizeof(_objs));
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            _objs[i].kind = 0xFF;
        
        init(0,S_ship, 0, 72*4, 112*4);
        set_level_data(level);

        //_v_scroll_cell = _level_size*16 - 26;
    }
    
    virtual void init()
    {
        _lfsr = 1;
        sound(s_effect_init);
        
#ifdef GENERATE_EXPLODABLES
        generate_explodables();
#endif
        _level_index = 0;
        set_state(STATE_NEXT_LEVEL);
    }
    
    int8_t newObj()
    {
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind == 0xFF)
                return i;
        return -1;
    }
    
    // spawn an egg or explosion
    int8_t spawn(uint8_t kind, uint8_t ai, short x, short y, short dx, short dy)
    {
        int8_t i = newObj();
        if (i >= 0)
            init(i,kind,0,x,y,dx,dy);
        return i;
    }

    void sparks(short x, short y ,int8_t n)
    {
        n++;
        while (n--) {
            short r = randy();
            int8_t rdx = r;
            if (!rdx)
                rdx = randy();
            int8_t rdy = r>>8;
            r = randy();
            spawn(S_spark_0 + (r & 3),((r >> 2)&7)+4,x,y,rdx >> 1,rdy - 42);
        }
    }

    void draw(JObj& o)
    {
        short x = o.x>>2;
        short y = o.y>>2;
        y += _v_scroll_line & 7;
        sprite_add(GET_SPRITE(o.kind),x,y);
    }
    
    short drag(short n)
    {
        if (n < 0)
            return -drag(-n);
        return n >> 1;
    }
    
    uint8_t clip_get_tile(short x, short y)
    {
        int8_t wx = x;
        while (wx < 0)
            wx += COLS;
        while (wx >= COLS)
            wx -= COLS;
        
        uint8_t t = tileTransform(pgm_read_byte(get_row(y) + wx));
        if (t < 2)
            return 0;
        return t;
    }
    
    // Blow something up, return its size
    int8_t shoot(uint8_t x, uint8_t y, uint8_t tile)
    {
        uint16_t hit = detect_hit(x,y);
        if (hit && (_explodedcount < MAX_EXPLODED))
        {
            uint8_t i;
            for (i = 0; i < _explodedcount; i++)
                if (_explodedlist[i] == hit)
                    return 0;                           // Already hit
            _explodedlist[_explodedcount++] = hit;      // New hit (TODO score)
            int8_t kind = pgm_read_byte(_explodables + (uint8_t)(hit));
 
            _fuel += pgm_read_byte(_explodable_fuel + kind);
            if (_fuel > MAX_FUEL)
                _fuel = MAX_FUEL;

            // booom is size dependent
            int width = pgm_read_byte(_explodable_width + kind);
            if (kind != LASER_GATE) { // LASERGATE
                const uint8_t* s = s_boom_3;
                switch (width) {
                    case 1: s = s_boom_1; break;
                    case 2: s = s_boom_2; break;
                }
                sound(s);
                return width;
            }
        }
        return 0;
    }
    
    void step(uint8_t i)
    {
        JObj& o = _objs[i];
        
        if (i == 0) {   // Only the ship
            o.dx = drag(o.dx);
            o.dy = drag(o.dy);
        }

        // update sparks
        if (o.kind >= S_spark_0 && o.kind <= S_spark_3)
        {
            if ((o.x < -8) || (o.x > WIDTH*4) || (o.y > 192*4))
            {
                o.kind = 0xFF;
                return;
            }
            o.dy += 5;    // gravity
            if (!--o.ai) {
                o.ai = randy()&0x7;
                o.kind = (o.kind == S_spark_3) ? S_spark_0 : o.kind+1;
            }
        }

        o.x += o.dx >> 4;
        o.y += o.dy >> 4;
        
        // WRAP TODO
        if (o.x < -20)
            o.x += WIDTH*4+20;
        while (o.x > WIDTH*4)
            o.x -= WIDTH*4+20;
        
        // Missile
        if (o.kind == S_missile0 || o.kind == S_missile1)
        {
            if (++o.timer == 6)
            {
                o.timer = 0;
                o.kind = o.kind == S_missile0 ? S_missile1: S_missile0; // Generic animation TOOD
            }
            uint8_t mx = o.x >> 4;
            uint8_t my = (o.y >> 5)+1;
            if (my > 24) {
                o.kind = 0xFF;  // Offscreen
                return;
            }
            uint8_t t = clip_get_tile(mx,my);
            if (t)   // Color clocks
            {
                int8_t n = shoot(mx,my,t);
                if (n)
                    sparks(o.x,o.y,n);
                o.kind = 0xFF;  // Boom
                return;
            }
        }
        
        // ship
        // Coordinates are 4x oversampled
        // ship is 24*16
        if (i == 0)
        {
            if (o.y < 0) o.y = 0;
            if (o.y > 160*4) o.y = 160*4;
            
            uint8_t cx = 6*4;
            uint8_t cy = 8*4;
            short xx = o.x + cx;   // x is in color clocks
            short yy = o.y + cy;
            
            xx >>= 4;  //
            yy >>= 5;
            
            // Collide (X and Y)
            uint8_t t = clip_get_tile(xx,yy+1);
            if (t && (_v_scroll_cell > 16)) // Don't collide with title
            {
                uint8_t tt = get_tile(xx,yy+1);
                if (VULNERABLE && t == tt)  // can fly through explosions
                    set_state(STATE_DEAD);
            }

            // show blown up ship
            o.kind = _game_state == STATE_DEAD ? (S_ship_explosion0 + ((v_framecounter >> 3) & 1)) : S_ship;
        }
    }
    
    //
    void onKey(uint8_t i,uint8_t key)
    {
        if (i != 0)
            return;
        JObj& o = _objs[i];

        if (_game_state == STATE_DEAD)
            return; // Don't move when dead

        if (key & FIRE) {
            if (!music_playing(1)) {
                music_play(s_fire,1);   // play fire on track 1, limit speed
                bool right = _missile++ & 1;
                spawn(S_missile0,0,o.x + (right ? 44 : 0),o.y+16,0,0x100);
            }
        }
        uint8_t n = 255;
        if (key & UP)
            o.dy = -n;
        if (key & DOWN)
            o.dy = n;
        if (key & LEFT)
            o.dx = -n/2;
        if (key & RIGHT)
            o.dx = n/2;
    }
    
    // Insertion sort of patches
    // In screen space, called to render destroyed objects
    void add_patch(int8_t x, int8_t y, uint8_t tile)
    {
        if (x < 0 || y < 0 || y >= 25 || x >= COLS)
            return;
        if (_patch_count == MAX_PATCHES)
            return;
        uint8_t i = _patch_count++;
        while (i > 0 && (y < _patch[i-1].y))
        {
            _patch[i] = _patch[i-1];
            --i;
        }
        _patch[i].x = x;
        _patch[i].y = y;
        _patch[i].tile = tile;
    }
    
    void render_explosion(uint16_t e)
    {
        Explodable ex;
        get_explodable(_explodables + (e & 0xFF),&ex);
        int16_t y = ((e & 0xFF00) >> 4) - _v_scroll_cell + ex.y;    // screen space
        uint8_t right = ex.x+ex.width;
        
        //  Lookup an explosion animation
        const uint8_t* pix = _explosion_anim + (ex.width-1)*8;
        pix += ((v_framecounter >> 3) + ex.x) & 7;
        pix = _explosion_tiles + pgm_read_byte(pix);

        while (ex.height--)
        {
            for (uint8_t x = ex.x; x < right; x++)
                add_patch(x,y,pgm_read_byte(pix++));
            y++;
        }
    }

    // render explosions, remove if off top (or off bottom)
    void render_explosions()
    {
        _patch_count = 0;
        uint8_t i,j;
        uint8_t topseg = _v_scroll_cell >> 4;
        for (i = j = 0; i < _explodedcount; i++)
        {
            uint16_t e = _explodedlist[i];
            if (_game_state == STATE_UP) {
                if ((e >> 8) >= (topseg+2))  // Off bottom
                    continue;
            } else {
                if ((e >> 8) < topseg)  // Off top
                    continue;
            }
            render_explosion(e);
            _explodedlist[j++] = e;
        }
        _explodedcount = j;
    }

    void set_state(int8_t state)
    {
        if (state == _game_state)
            return;
        switch (state) {
            case STATE_DEAD:
                sparks(_objs[0].x,_objs[0].y,8);
                sound(s_boom_3,true);
                _game_timer = 60*3; // reflect
                break;
            case STATE_BOMB:
                set_level_data(_level_index++);
                _v_scroll_cell = _level_size*16-22; // STAY at BOTTOM
                _game_timer = 3*60;
                break;
            case STATE_NEXT_LEVEL:
                if (_level_index == 6)
                    _level_index = 0;   // 3 levels for now
                start_level(_level_index++);
                state = STATE_TITLE;
                _game_timer = 2*60;
                break;
        }
        _game_state = state;
    }

    void music()
    {
        if (!(v_framecounter & 3)) {
            uint8_t hum = 70 - ((_objs[0].y >> 6) + (v_framecounter & 4));
            if (_game_state == STATE_DEAD)
                hum = 0;
            music_note(CH1,hum,16);
        }
        if (_game_state == STATE_UP && !(v_framecounter & 0x3F))
            sound(s_klaxon,true);   // on ch3
        music_kernel();
    }

    bool fast_scroll()
    {
        uint8_t m = get_segment(2);
        if (m == T_SHIPS0 || m == T_SHIPS1)
            return true;
        m = get_segment(18);
        return (m == T_SHIPS0 || m == T_SHIPS1);
    }
    
    // Read keyboard
    virtual void step()
    {
        uint16_t* j = readJoy();
        uint16_t key = j[0];
        if (!key)
          key = j[1];              // use p1 or p2
        if (key) {
            _lfsr ^= v_framecounter;
            onKey(0,key);
        }
        
        //  update positions
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind != 0xFF)
                step(i);
        
        // Update game state
        if (_game_timer) {
            if (--_game_timer == 0)
            {
                switch (_game_state) {
                    case STATE_TITLE:
                        set_state(STATE_DOWN);
                        break;
                    case STATE_BOMB:
                        set_state(STATE_UP);
                        break;
                    case STATE_DEAD:
                        set_state(STATE_NEXT_LEVEL);
                        break;
                }
            }
        }
        
        // out of fuel?
        if (_game_state == STATE_DOWN || _game_state == STATE_UP) {
            if (_fuel)
                _fuel--;
            if (!_fuel)
                set_state(STATE_DEAD);
        }

        int8_t rate = 1 + (_level_index >> 1);  // gets nutty fast
        switch(_game_state) {
            case STATE_DOWN:
                if ((v_framecounter & 1) || fast_scroll())
                    _v_scroll_line += rate;
                if (_v_scroll_line >= 8)
                {
                    uint16_t bottom = _level_size*16-22;
                    if (++_v_scroll_cell >= bottom) // just reached the bomb at bottom
                        set_state(STATE_BOMB);
                    _v_scroll_line -= 8;
                }
                break;
            case STATE_UP:
                if ((v_framecounter & 1) || fast_scroll())
                {
                    _v_scroll_line -= rate;
                    if (_v_scroll_line < 0)
                    {
                        _v_scroll_line += 8;
                        if (_v_scroll_cell && --_v_scroll_cell == 0)
                            set_state(STATE_NEXT_LEVEL);
                    }
                }
                break;
        }
        music();
    }
    
    //  This should be reasonably fast
    //  Called by sprite engine to get underlying block
    //  only called for tiles intersecting sprites
    virtual uint8_t get_tile(uint8_t x, uint8_t y)
    {
        // check patch list first; they are sorted but brute force for now
        for (uint8_t i = 0; i < _patch_count; i++)
        {
            uint8_t py = _patch[i].y;
            if (py == y && _patch[i].x == x)
                return _patch[i].tile;
            if (py > y)
                break;
        }
        return tileTransform(pgm_read_byte(get_row(y) + x));
    }
    
    //  Draw playfield by inserting 1 row at a time.
    //  Rows are double buffered
    
#define LINE_BUFFERS 4
#define COLS 40
    
    VRowTiles _rows[LINE_BUFFERS];
    uint8_t _buf[COLS*LINE_BUFFERS];
    
    uint8_t showcollide(uint8_t t,int x, int y, int m)
    {
        if (abs(x) <= 1 && abs(y) <= 1)
            return 2;
        return t;
    }
    
    // Draw fuel bar at top of screen
    void drawfuel(uint8_t* t)
    {
        memset(t,1,COLS);
        uint8_t c;
        if (_game_state == STATE_DOWN)
        {
            c = 72; // fuel
            t[18] = 24;
            t[19] = 25;
            t[20] = 26;
        } else {
            c = 64; // time
            t[18] = 85;
            t[19] = 86;
            t[20] = 87;
        }
        
        uint8_t f = _fuel >> 8;
        uint8_t fx = _fuel;
        fx >>= 6;
        uint8_t* tm = t+17;
        t += 21;
        while (f--) {
            *tm-- = c;
            *t++ = c;
        }
        c += 4;
        *tm = c+fx;
        *t++= c-fx;
        *t++= 0;
    }
    
    virtual void draw()
    {
        // Patch in explosions
        render_explosions();

        // Render sprites
        sprite_init(_tiles,sizeof(_tiles) >> 4);
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind != 0xFF)
                draw(_objs[i]);
        
        // Init tile line buffers
        for (uint8_t r = 0; r < LINE_BUFFERS; r++)
        {
            VRowTiles* row = _rows + r; // 2 bufs
            row->row.mode = VMODE_TILES;
            row->rom_font = _tiles;
            row->rom_font_count = sizeof(_tiles)/16;
            row->tiles = _buf + r*COLS;
        }
        
        // Draw
        uint8_t patch = 0;
        uint8_t lasergates = 0;
        for (uint8_t r = 0; r < (_v_scroll_line ? 25 : 24); r++)
        {
            VRowTiles* row = _rows + (r&(LINE_BUFFERS-1));             // 2 bufs
            while (video_queue_count() == LINE_BUFFERS)
                ;
            
            uint8_t* t = row->tiles;
            if (r < HUD_HEIGHT) {
                // Draw Score etc
                row->row.y = (r << 3) + 7;
                row->v_scroll = 0;
                t[0] = 0;
                if (r == 0 && _game_state != STATE_TITLE)
                    drawfuel(t);
                else if (r == 1) {
                    //drawdebug(t);
                }
            } else {
                row->row.y = (r << 3) + 7 - _v_scroll_line;
                row->v_scroll = _v_scroll_line;
            
                // Draw playfield
                uint8_t rr = r-HUD_HEIGHT;
                const uint8_t* tiles = get_row(rr);
                for (uint8_t x = 0; x < COLS; x++) {        // TODO...zero lines, memcpy etc
                    uint8_t tile = pgm_read_byte(tiles++);
                    if (tile == 88)
                        lasergates = 1;
                    t[x] = tileTransform(tile);             // don't need specials here, will patch later

                    // activate bomb
                    if (t[x] >= 160 && _game_state == STATE_UP)
                        t[x] = 96+((v_framecounter >> 2) & 7);
                }

                // Patch in tiles
                while ((_patch[patch].y == rr) && (patch < _patch_count))
                {
                    t[_patch[patch].x] = _patch[patch].tile;
                    patch++;
                }
                
                // Render sprites for this row
                row->ram_font = sprite_draw(t,rr);
                
                //  Zero trim - saves time at end of lines
                uint8_t m = COLS;
                while (m--)
                {
                    if (t[m] > 1)
                        break;
                    t[m] = 0;
                }
            }
            video_enqueue((VRow*)row);
        }
        //printf("exit at %d:%d\n",video_queue_count(),video_current_line());

        if (lasergates && !(v_framecounter & 0x40))
            sound(s_lasergate);

        // Wait for last lines to be rendered
        while (video_current_line() > 0 && (video_current_line() < 192))
            flush_video(true);
    }
};

Titan _titan;