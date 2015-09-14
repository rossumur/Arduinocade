//
//  joust.cpp
//
//  Created by Peter Barrett on 7/30/14.
//  Copyright (c) 2014 Peter Barrett. All rights reserved.
//

#include "config.h"
#include "sprites.h"
#include "music.h"
#include "video.h"

#include "jowst_tiles.h"
#include "jowst_music.h"

#ifdef BALLBLASTER_GAME
#error Please undefine BALLBLASTER_GAME in arduinocade/config.h
#endif

#define FACING_LEFT 1
#define RIDER 2
#define WALKING 4
#define LANDED 8
#define SPAWNING 0x10
#define COLLIDABLE 0x80

#define COLS 40
#define WIDTH (4*COLS)

#define MAX_OBJS 16
#define FLAP_FORCE 0x1A0
#define GRAVITY 0x8
#define FLAP_H_FORCE 0x100
#define WALK_FORCE 0x40
#define KEY_REPEAT_TICKS 20
#define MAX_FLYING_VELOCITY 0x200
#define MAX_WALKING_VELOCITY 0x80

#define FORCE_FLAP 0x1000

#define MAX(_x,_y) ((_x) > (_y) ? (_x) : (_y))
#define MIN(_x,_y) ((_x) < (_y) ? (_x) : (_y))

// 4x4 ordered dither
PROGMEM
const uint8_t _dither[]= {
    0, 8, 2, 10,
    12,4, 14,6,
    3 ,11,1, 9,
    15,7, 13,5
};

PROGMEM
const uint8_t _spawn_locations[] = {
    68,32,
    8,72,
    134,60,
    64,144
};

inline
short get_spawn(int8_t i)
{
    return pgm_read_byte(_spawn_locations + i)<<2;
}

// Generate a 4x4 ordered dither matrix for fading objects
// n 1-15, 0 = black, >15 white
void make_dither(int8_t n, uint8_t* dst)
{
    const uint8_t *p = _dither;
    for (uint8_t y = 0; y < 4; y++)
    {
        uint8_t a = 0;
        uint8_t m = 0x88;
        for (uint8_t x = 0; x < 4; x++)
        {
            if (pgm_read_byte(p++) < n)
                a |= m;
            m >>= 1;
        }
        *dst++ = a; // 1 byte*4
    }
}

typedef struct
{
    uint8_t kind;
    uint8_t rider;
    uint8_t flags;   // facing left, dead, walking
    uint8_t timer;
    uint8_t ai;
    short x,y;
    short dx,dy;
} JObj;

extern uint8_t _line_count;

//  Draw playfield by inserting 1 row of tiles at a time.
//  We have room for up to 4 rows of tiles, lots of space leftover for sprites

#define LINE_BUFFERS 4
#define COLS 40
VRowTiles _rows[LINE_BUFFERS];
uint8_t _tilebuf[COLS*LINE_BUFFERS];

enum GameState {
    SPLASH,             // Logo, no ball, theme playing
    PLAYER_SELECTION,
    WAVE_BEGIN,
    PLAYING,
    GAME_OVER,
};

class Joust : public Game
{
    // game state
    uint8_t _frames;
    uint8_t _spawn;
    uint8_t _waves;
    uint8_t _state;
    uint8_t _enemies;
    uint8_t _enemies_to_spawn;
    uint8_t _ai;

    JObj _objs[MAX_OBJS];  // up to n flappy things
    short _lfsr;

    uint8_t _player[2];
    uint8_t _lives[2];
    long    _score[2];
    uint16_t _joy[2];
    uint16_t _joylast[2];   // for flap events

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
        _objs[i].flags = 0;
    }
    
    uint16_t button_down(int8_t i)
    {
        return (_joy[i] ^ _joylast[i]) & _joy[i];
    }
    
    bool joy()
    {
        uint16_t* j = readJoy();
        for (int i = 0; i < 2; i++) {
            _joylast[i] = _joy[i];
            _joy[i] = j[i];
        }
        return _joy[0] | _joy[1];
    }

    void reset()
    {
        _ai = 0;
        _score[0] = 0;
        _score[1] = -1;
        _lives[0] = _lives[1] = 3;
        _joy[0] = _joy[1] = 0;
        _player[0] = _player[1] = 0xFF;
        _joylast[0] = _joylast[1] = 0;
        memset(_objs,0,sizeof(_objs));
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            _objs[i].kind = 0xFF;
    }

    virtual void init()
    {
        _lfsr = 1;
        _waves = 0;
        set_state(SPLASH);
    }

    void next_wave()
    {
        _spawn = 0;
        _ai = 60;
        _enemies = 0;
        _enemies_to_spawn = MIN(_waves+3,4);
        _waves++;
    }

    void set_state(uint8_t state)
    {
        switch (state) {
            case SPLASH:
                reset();
                music_play(_joust_start);
                break;
            case WAVE_BEGIN:
                spawn_player(0,3);
                if (_score[1] != -1)
                    spawn_player(1,2);
                next_wave();
                break;
            case GAME_OVER:
                music_play(_joust_start);
                break;
        }
        _state = state;
    }

    int8_t is_egg(JObj& o)
    {
        return (o.kind >= S_egg_0 && o.kind <= S_egg_crack);
    }

    void dead_enemy()
    {
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (is_egg(_objs[i]))
                return;
        if (!_enemies) {
            next_wave();
            _score[0] += _waves*5000;
            if (_score[1] != -1)
                _score[1] += _waves*5000;
            music_play(_start_2);
        }
    }

    void music()
    {
        if (!music_playing())  // Music ending may trigger state changes
        {
            switch (_state) {
                case SPLASH:
                    set_state(WAVE_BEGIN);
                    break;
                case GAME_OVER:
                    set_state(SPLASH);
                    break;
            }
        }
        music_kernel();
    }

    int8_t newObj()
    {
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind == 0xFF)
                return i;
        return -1;
    }

    // from egg
    void birth(short x, short y)
    {
        int8_t i = spawn_bird(S_bird2, S_rider2 + (v_framecounter & 2), x, y-10);
        _enemies++;
        if (i >= 0) {
            _objs[i].flags &= ~(SPAWNING | LANDED);
            _objs[i].ai = 120;
        }
    }

    int8_t spawn_enemy(int8_t where)
    {
        _enemies++;
        return spawn_bird(S_bird2, S_rider2 + (v_framecounter & 2), get_spawn(where*2), get_spawn(where*2 + 1));
    }

    int8_t spawn_player(int8_t p, int8_t where)
    {
        music_play(_emerge);
        int8_t i = spawn_bird(S_bird0 + p*4, S_rider0+p*2, get_spawn(where*2), get_spawn(where*2+1));
        _player[p] = i;
        return i;
    }

    int8_t spawn_bird(uint8_t kind, uint8_t rider, short x, short y)
    {
        int8_t i = newObj();
        if (i >= 0) {
            init(i,kind,rider,x,y);
            _objs[i].flags |= SPAWNING | LANDED | COLLIDABLE | RIDER;
            _objs[i].timer = 64;
        }
        return i;
    }

    // spawn an egg or explosion
    int8_t spawn(uint8_t kind, uint8_t ai, short x, short y, short dx, short dy)
    {
        int8_t i = newObj();
        if (i >= 0)
            init(i,kind,0,x,y,dx,dy);
        return i;
    }
    
    // ugly hacks to position fiddly sprites.
    void draw(JObj& o)
    {
        short x = o.x>>2;
        short y = o.y>>2;
        uint8_t bird = o.kind;
        uint8_t clip = MAX_SPRITE_Y;

        // SPAWNING in white
        if (o.flags & SPAWNING) {
            int8_t d = o.timer >> 2;
            y += d;
            clip = y + 18-d;
            if (o.timer == 0)
                o.flags &= ~(SPAWNING | LANDED);   // spawned
            sprite_add(GET_SPRITE(S_bird2_white),x,y,clip);
            return;
        }

        if (o.kind <= S_bird2) {
            if (o.flags & FACING_LEFT)
                bird++;
            if (o.timer)    // Flapping
                bird += 2;
        }
        sprite_add(GET_SPRITE(bird),x,y,clip);
        
        uint8_t rider = o.rider;
        if (o.flags & FACING_LEFT)
            x -= 6;
        if (!rider || !(o.flags & RIDER))
        {
            // No rider
        } else {
            int8_t h = 3;
            if (o.flags & FACING_LEFT)
                rider++;
            else if (o.kind == S_bird2) // ugly positioning heuristic
                h--;
            sprite_add(GET_SPRITE(rider),x+h,y,clip);
        }
        
        // Position legs. Nasty hacky heursistics
        if (o.flags & WALKING) {
            int8_t p = which_player(o);
            uint8_t w = (x & 3)*2;
            if (abs(o.dx) > MAX_WALKING_VELOCITY) {
                w = o.flags & FACING_LEFT ? 8:9;   // skid legs
                if (!(o.flags & FACING_LEFT))
                    x -= 3;
                if (p != -1 && !music_playing(1))
                    music_play(_skid,1);
            } else {
                if (o.flags & FACING_LEFT) {
                    w = 6-w;
                } else {
                    w += 1;
                    x -= 5;
                }
                if ((p != -1) && (abs(o.dx)>>4) && !music_playing(1))
                    music_play(_step,1);
            }
            sprite_add(GET_SPRITE(S_leg0_left + w),x+8,y+12,clip);
        }
    }
    
    short drag(short n, bool walking)
    {
        if (n < 0)
            return -drag(-n,walking);
        if (walking)
            return n - ((n+31)>>5);
        if (n > MAX_FLYING_VELOCITY)
            n = MAX_FLYING_VELOCITY;
        return n - ((n+127)>>7);
    }
    
    uint8_t clip_get_tile(short x, short y)
    {
        int8_t wx = x;
        while (wx < 0)
            wx += COLS;
        while (wx >= COLS)
            wx -= COLS;
        uint8_t t = get_tile(wx,y);
        if (t < 2)
            return 0;
        return t;
    }
    
    void ai(uint8_t i)
    {
        JObj& o = _objs[i];
        if (!o.ai--) {
            switch (o.kind)
            {
                case S_explode0:
                    o.kind = S_explode1;
                    o.ai = 8;
                    break;
                case S_explode1:
                    o.kind = S_explode2;
                    o.ai = 8;
                    break;
                case S_explode2:
                    o.kind = 0xFF;
                    o.ai = 0;
                    break;

                case S_egg_0:
                    o.kind = S_egg_1;
                    o.ai = 20;
                    break;
                case S_egg_1:
                    o.kind = S_egg_2;
                    o.ai = 20;
                    break;
                case S_egg_2:
                    o.kind = S_egg_crack;
                    o.ai = 30;
                    break;
                case S_egg_crack:
                    o.kind = 0xFF;
                    birth(o.x-4,o.y);
                    break;

                case S_bird0:
                case S_bird1:
                {
                    uint8_t r = randy();
                    o.ai = 30 + (r >> 4); // next action
                    if (!(o.flags & RIDER))
                        flap(i,FORCE_FLAP | (o.flags & FACING_LEFT ? LEFT : RIGHT)); // flee
                }
                    break;

                case S_bird2:
                    {
                        uint8_t r = randy();
                        o.ai = 30 + (r >> 4); // next action
                        if (o.flags & RIDER) {
                            switch (r & 0xF)
                            {
                                case 0:
                                case 1:
                                case 2:
                                    o.flags ^= FACING_LEFT;
                                    break;
                                case 3:
                                case 4:
                                case 5:
                                    break;
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                case 10:
                                    flap(i,FORCE_FLAP);
                                    break;
                                default:
                                    flap(i,FORCE_FLAP | (o.flags & FACING_LEFT ? LEFT : RIGHT));
                            }
                        } else {
                            flap(i,FORCE_FLAP | (o.flags & FACING_LEFT ? LEFT : RIGHT)); // flee
                        }
                    }
                    break;
                default:
                    o.ai = 60;
                    break;
            }
        }
    }

    // Coordinates are 4x oversampled
#define FRAC 4

    // fly offscreen
    int8_t flown_away(JObj& o)
    {
        if (o.flags & RIDER)
            return 0;

        switch (o.kind) {
            case S_bird0:   spawn_player(0,3); break;
            case S_bird1:   spawn_player(1,2); break;
            case S_bird2:
                if (_enemies)   // BUGBUG?
                    --_enemies;
                dead_enemy();
                break;
        }
        o.kind = 0xFF;
        return 1;
    }

    void step(uint8_t i)
    {
        ai(i);
        JObj& o = _objs[i];
        
        o.dx = drag(o.dx,o.flags & WALKING);
        o.x += (o.dx + 31) >> 6;
        o.y += o.dy >> 6;
        
        if (!(o.flags & LANDED))
            o.dy += GRAVITY;   // Gravity
        if (o.timer)
            o.timer--;
        
        // WRAP TODO
        int8_t wrapped = 0;
        if (o.x < -20) {
            o.x += WIDTH*FRAC+20;
            wrapped = 1;
        }
        while (o.x > WIDTH*FRAC) {
            o.x -= WIDTH*FRAC+20;
            wrapped = 1;
        }
        if (wrapped && flown_away(o))
            return;

        if (o.y < 0)
        {
            o.dy = 0;
            o.y = 0;
        }
        
        uint8_t cx = 5*FRAC;
        uint8_t cy = 10*FRAC;    // room for legs
        if (is_egg(o))
        {
            cy = 0;
            cx = 2*4;
        }

        short xx = o.x + cx;       // in color clocks
        short yy = o.y + cy;
        xx >>= 4;  //
        yy >>= 5;

        // Collide (X and Y)
        o.flags &= ~WALKING;
        if (clip_get_tile(xx,yy-1))
        {
            o.y += 8;               // bonk
            o.dy = 0;
        }
        if (clip_get_tile(xx,yy+1))
        {
            if (is_egg(o))
            {
                //printf("%d %d %d\n",xx,yy,o.dy);
                if (!(o.flags & LANDED)) {
                    o.dy = -o.dy >> 1;
                    // TODO
                    if (abs(o.dy) < 100 && abs(o.dx) < 100)
                    {
                        o.flags |= LANDED;
                        o.y = yy*8*FRAC - 0*FRAC;   // Landing
                        o.dy = 0;
                    }
                }
            } else {
                o.flags |= WALKING;
                o.y = yy*8*FRAC - 10*FRAC;   // Landing
                o.dy = 0;
            }
        }
        if (clip_get_tile(xx-1,yy))
        {
            o.x += 8;               // bounce
            o.flags &= ~FACING_LEFT;
            o.dx = -o.dx>>1;
        }
        if (clip_get_tile(xx+1,yy))
        {
            o.x -= 8;
            o.flags |= FACING_LEFT;
            o.dx = -o.dx>>1;
        }
    }
    
    void flap(uint8_t i, uint16_t key)
    {
        JObj& o = _objs[i];
        int8_t player = which_player(o);
        player = (player != -1) ? (button_down(player) & FIRE) : 0;

        if (o.flags & SPAWNING)
            return;

        if (player || (key & FORCE_FLAP))
        {
            if (key & LEFT) o.dx -= FLAP_H_FORCE;   // FLAP
            if (key & RIGHT) o.dx += FLAP_H_FORCE;
            o.timer = 10;
            o.dy -= FLAP_FORCE;
            o.flags &= ~WALKING;
            if (player)
                music_play(_flap,1);    // play on track 1 to avoid other effects
        }
        if (o.flags & WALKING)
        {
            if (abs(o.dx) < MAX_WALKING_VELOCITY) { // sliding?
                if (key & LEFT) o.dx = MAX(o.dx - WALK_FORCE,-MAX_WALKING_VELOCITY);
                if (key & RIGHT) o.dx = MIN(o.dx + WALK_FORCE,MAX_WALKING_VELOCITY);
            }
        }
        if (key & LEFT)
            o.flags |= FACING_LEFT;
        else if (key & RIGHT)
            o.flags &= ~FACING_LEFT;    // don't switch with 0 velocity
    }
    
    void kill(JObj& o)
    {
        o.timer = 10;
        o.flags &= ~(RIDER | COLLIDABLE);

        int8_t p = which_player(o);
        if (p != -1) {
            o.rider = 0;
            if (!--_lives[p])     // DEAD PLAYER
            {
                set_state(GAME_OVER);
                return;
            }
        } else {
            short rdx = 0x80-(randy()&0xFF);
            short rdy = randy()&0xFF;
            int8_t j = spawn(S_egg_0,0,o.x,o.y,o.dx + rdx,o.dy - rdy);
            if (j != -1) {
                _objs[j].timer = 30;    // wait for a bit before collecting
                _objs[j].ai = 255;      // crack in a while
            }

            // Explode
            j = spawn(S_explode0,0,o.x+8,o.y-4,0,0);
            if (j != -1) {
                _objs[j].flags |= LANDED;
                _objs[j].ai = 8;
            }
        }

        music_play(_joustkill);
    }

    //
    void boing(uint8_t i,uint8_t j)
    {
        short temp = _objs[j].dx;
        _objs[j].dx = _objs[i].dx;
        _objs[i].dx = temp;
        _objs[i].x += _objs[i].dx >> 6;
        _objs[j].x += _objs[j].dx >> 6;
        if (!music_playing())
            music_play(_thump);
       // printf("boing %d %d\n",_objs[i].dx,_objs[j].dx);
    }

    int8_t which_player(JObj& src)
    {
        switch (src.rider) {
            case S_rider0: return 0;
            case S_rider1: return 1;
            default: return -1;
        }
    }

    void score(int8_t player, short value)
    {
        _score[player] += value;
    }
    // Collide objects
    void joust()
    {
        for (uint8_t i = 0; i < MAX_OBJS; i++)
        {
            JObj& src = _objs[i];
            if (src.kind == 0xFF)
                continue;
            int8_t p = which_player(src);

            // Collide players with each other / bad guys
            if (!((src.flags | COLLIDABLE) && (p != -1)))
                continue;

            for (uint8_t j = 0; j < MAX_OBJS; j++)
            {
                JObj& dst = _objs[j];
                if (i == j || (dst.kind == 0xFF))
                    continue;

                // Fight i and j
                short dx = src.x - dst.x;
                if (abs(dx) >= 10*4)
                    continue;
                short dy = src.y - dst.y;
                if (abs(dy) >= 12*4)
                    continue;

                // consume egg
                if (p != -1 && (is_egg(dst)) && (dst.timer == 0))
                {
                    dst.kind = 0xFF;
                    music_play(_egg);
                    dead_enemy();   // check for last egg
                    score(p,200);
                    continue;
                }

                if (!(dst.flags & COLLIDABLE))
                    continue;

                //printf("%d: %d hit %d (%d %d)\n",v_framecounter,i,j,dx,dy);
                if (dy == 0)
                    boing(i,j);
                else
                    kill(dy < 0 ? dst : src);
            }
        }
    }
            
    // Read  sticks
    virtual void step()
    {
        // read input
        if (joy()) {
            if (_joy[1] && _score[1] == -1) {
                _score[1] = 0;
                spawn_player(1,2);
            }
            _lfsr ^= v_framecounter;
            if (!_lfsr)
                _lfsr = -1;
            flap(_player[0],_joy[0]);
            flap(_player[1],_joy[1]);
        }
                
        //  update positions, run object ai
        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind != 0xFF)
                step(i);

        // Game AI: spawn some enemies
        if (--_ai == 0) {
            if (_state == WAVE_BEGIN && _enemies_to_spawn)
            {
                --_enemies_to_spawn;
                spawn_enemy(_spawn++ & 3);
            }
            _ai = 60;
        }

        // Joust
        joust();

        music();
    }

    //
    virtual uint8_t get_tile(uint8_t x, uint8_t y)
    {
        int i = y*40+x;
        uint8_t t = pgm_read_byte(_map + i);
        return t;
    }

    uint8_t showcollide(uint8_t t,int x, int y, int m)
    {
        if (abs(x) <= 1 && abs(y) <= 1)
            return 2;
        return t;
    }

    void dissolve(uint8_t fade, uint8_t col, uint8_t left, uint8_t right)
    {
        uint8_t dither[4];
        make_dither(fade,dither);  // Ordered dither for fading (0 == 16 bits black, 15 == 1 bit black)
        for (uint8_t x = left; x < right; x++)
        {
            uint8_t* test = sprite_get_tile(x,col);
            for (uint8_t i = 0; i< 4; i++)
            {
                uint8_t a = dither[i];
                test[0] &= a;
                test[1] &= a;
                test[8] &= a;
                test[9] &= a;
                test += 2;
            }
        }
    }
    
    void dbg(uint16_t n, uint8_t* t)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            t[3-i] = 48 + (n % 10); // brutal
            n /= 10;
        }
    }

    void render_score(int8_t p, uint8_t* dst)
    {
        long s = _score[p];
        int8_t lives = MIN(_lives[p],4);
        memset(dst,24,9);
        if (s == -1)
            return;

        int8_t i = 7;
        while (i--) {
            int8_t d = s ? (s % 10) : 0;
            dst[i] = d+48;
            s = s/10;
            if (!s)
                break;
        }

        // draw up to 6 lives
        i = 9;
        int8_t t = 68 + (1-p)*2;
        while (i-- && (lives > 0)) {
            dst[i] = lives > 1 ? (t +1) : t;
            lives -= 2;
        }
    }

    virtual void draw()
    {
        uint8_t t = _line_count;

        // Render sprites
        sprite_init(_tiles,sizeof(_tiles) >> 4);

        // test fade in effect
        //dissolve((v_framecounter >> 1) & 0x1F,6,12,26); //

        for (uint8_t i = 0; i < MAX_OBJS; i++)
            if (_objs[i].kind != 0xFF)
                draw(_objs[i]);
        
        // Init tile line buffers
        for (uint8_t r = 0; r < LINE_BUFFERS; r++)
        {
            VRowTiles* row = _rows + r;
            row->row.mode = VMODE_TILES;
            row->rom_font = _tiles;
            row->rom_font_count = sizeof(_tiles)/16;
            row->tiles = _tilebuf + r*COLS;
            row->v_scroll = 0;
        }
        t = _line_count-t;

        // render score
        uint8_t score_0[9];
        uint8_t score_1[9];
        render_score(0,score_0);
        render_score(1,score_1);

        // Draw
        for (uint8_t r = 0; r < 24; r++)
        {
            VRowTiles* row = _rows + (r&(LINE_BUFFERS-1));             // 2 bufs
            while (video_queue_count() == LINE_BUFFERS)
                ;
            row->row.y = (r << 3)+7;
            
            for (uint8_t x = 0; x < COLS; x++)          // TODO...zero lines, memcpy etc
                row->tiles[x] = get_tile(x,r);            // don't need specials here

            // Patch special lines
            switch (r) {
                case 21:
                    for (int8_t x = 0; x < sizeof(score_0); x++)
                        row->tiles[x+9] = score_0[x];    // SCORE
                    for (int8_t x = 0; x < sizeof(score_1); x++)
                        row->tiles[x+21] = score_1[x];    // SCORE
                    break;
                case 23:
                    //dbg(t,row->tiles);
                    break;
            }

            //printf("Drawing row %d\n",r);
            row->ram_font = sprite_draw(row->tiles,r);  // Render sprites for this row
            
            // Zero trim
            uint8_t m = COLS;
            uint8_t* t = row->tiles;
            while (m--)
            {
                if (t[m] > 1)
                    break;
                t[m] = 0;
            }
            video_enqueue((VRow*)row);
        }
        
        // Probably perf

        //printf("exit at %d:%d\n",video_queue_count(),video_current_line());
        // don't need to wait
    }
};

Joust _joust;