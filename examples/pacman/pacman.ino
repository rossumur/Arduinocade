
/* Copyright (c) 2009-2015, Peter Barrett
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

// wukka
// Period of 300ms or 18 frames
// 5 on, 3 off
// 1000->500 84 STEP -3
// 500->1000 69 STEP 3

#include "config.h"
#include "music.h"
#include "sprites.h"
#include "video.h"

#include "pacman.h"
#include "pacman_music.h"

#ifdef BALLBLASTER_GAME
#error Please undefine BALLBLASTER_GAME in arduinocade/config.h
#endif

#ifndef u8
typedef unsigned char u8;
typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned short ushort;
typedef unsigned long u32;
#endif

extern uint16_t v_underflow;

enum GameState {
    ReadyState,
    PlayState,
    DeadGhostState, // Player got a ghost, show score sprite and only move eyes
    DeadPlayerState,
    EndLevelState
};

enum SpriteState
{
    PenState,
    RunState,
    FrightenedState,
    DeadNumberState,
    DeadEyesState,
    AteDotState,    // pacman
    DeadPacmanState
};

enum {
    MStopped = 0,
    MRight = 1,
    MDown = 2,
    MLeft = 3,
    MUp = 4,
};

#define BINKY 0
#define PINKY 1
#define INKY  2
#define CLYDE 3
#define PACMAN 4

#define PENX 16
#define PENY 11

PROGMEM
const uint8_t _initSprites[] =
{
    BINKY,  PENX,     PENY-2,   31, MLeft,
    PINKY,  PENX-1,   PENY,     79, MRight,
    INKY,   PENX,     PENY,     137, MLeft,
    CLYDE,  PENX+1,   PENY,     203, MLeft,
    PACMAN, PENX,     PENY+6,     0, MLeft,
};

#define PACMANICON 1
#define FRIGHTENEDPALETTE 5
#define DEADEYESPALETTE 6

//#define FPS 60
#define FPS 60
#define CHASE 0
#define SCATTER 1

#define PENGATE 16
#define DOT 17
#define PILL 29

// TODO PROGMEM

PROGMEM
const byte _opposite[] = { MStopped,MLeft,MUp,MRight,MDown };
#define OppositeDirection(_x) pgm_read_byte(_opposite + _x)

PROGMEM
const byte _scatterChase[] = { 7,20,7,20,5,20,5,0 };
PROGMEM
const byte _scatterTargets[] = { 2,0,25,0,0,35,27,35 }; // inky/clyde scatter targets are backwards
PROGMEM
const char _pinkyTargetOffset[] = { 4,0,0,4,-4,0,-4,4 }; // Includes pinky target bug
PROGMEM
const byte _pacAnim[] =
    {
        1,3,1,0,    // Right
        4,6,4,0,    // Down
        7,9,7,0,    // Left
        10,12,10,0, // Up
    };

class Sprite
{
public:
    short _x,_y;
    short lastx,lasty;
    byte cx,cy;         // cell x and y
    byte tx,ty;         // target x and y
    
    SpriteState state;
    byte  pentimer;     // could be the same
    
    byte who;
    byte speed;
    byte dir;
    byte phase;
    
    // Sprite bits
    byte palette2;  // 4->16 color map index
    byte bits;      // index of sprite bits
    signed char sy;
    
    void Init(const byte* s)
    {
        who = pgm_read_byte(s++);
        cx =  pgm_read_byte(s++);
        cy =  pgm_read_byte(s++);
        pentimer = pgm_read_byte(s++);
        dir = pgm_read_byte(s);
        _x = lastx = (short)cx*8-4;
        _y = lasty = (short)cy*8;
        state = PenState;
        speed = 0;
    }
    
    void Target(byte x, byte y)
    {
        tx = x;
        ty = y;
    }
    
    short Distance(byte x, byte y)
    {
        short dx = cx - x;
        short dy = cy - y;
        return dx*dx + dy*dy;   // Distance to target
    }
    
    //  once per sprite
    uint8_t SetupDraw(byte deadGhostIndex, ushort frightenedTimer)
    {
        int n = phase >> 3;
        if (who == PACMAN)
        {
            n = ((dir-1) << 2) | (n & 3);
            return pgm_read_byte(_pacAnim + n);
        }
        
        n &= 1;
        switch (state)
        {
            case DeadNumberState:
            case FrightenedState:
                n += S_ghost_blue0;
                if ((frightenedTimer < 3*FPS) && (frightenedTimer & 0x10))  // flashing white
                   n += 2;
                break;
            case DeadEyesState:
                n += 25;
                break;
            default: n += 13 + who*2;
        }
        return n;
    }
};


class Pacman : public Game
{
    Sprite _sprites[5];
    byte _dotMap[32*24/8];      // TODO Bounds
    byte _counter;   // DB
    ushort _dots;

    byte _ai;
    uint16_t _lastjoy;          // sticky last joystick

    GameState _state;
    long    _score;             // 7 digits of score
    char    _scoreStr[8];
    byte    _icons[14];         // Along bottom of screen
    
    ushort  _stateTimer;
    ushort  _frightenedTimer;
    byte    _frightenedCount;
    byte    _scIndex;           //
    ushort  _scTimer;           // next change of sc status
    
    // bool _inited;
    ushort _blocks;
    
public:
    byte GetTile(int cx, int cy)
    {
        return pgm_read_byte(_map + cy*32 + cx);
    }
    
    short Chase(Sprite* s, short cx, short cy)
    {
        while (cx < 0)      //  Tunneling
            cx += 30;
        while (cx >= 30)
            cx -= 30;
        
        byte t = GetTile(cx,cy);
        if (!(t == 1 || t == DOT || t == PILL || t == PENGATE))
            return 0x7FFF;
        
        if (t == PENGATE)
        {
            if (s->who == PACMAN)
                return 0x7FFF;  // Pacman can't cross this to enter pen
            if (!(InPen(s->cx,s->cy) || s->state == DeadEyesState))
                return 0x7FFF;  // Can cross if dead or in pen trying to get out
        }
        
        short dx = s->tx-cx;
        short dy = s->ty-cy;
        return dx*dx + dy*dy;   // Distance to target
    }
    
    void UpdateTimers()
    {
        // Update scatter/chase selector, low bit of index indicates scatter
        if (_scIndex < 8)
        {
            if (_scTimer-- == 0)
            {
                byte duration = pgm_read_byte(_scatterChase + _scIndex++);
                _scTimer = duration*FPS;
            }
        }
        
        //  Release frightened ghosts
        if (_frightenedTimer && !--_frightenedTimer)
        {
            for (byte i = 0; i < 4; i++)
            {
                Sprite* s = _sprites + i;
                if (s->state == FrightenedState)
                {
                    s->state = RunState;
                    s->dir = OppositeDirection(s->dir);
                }
            }
            _frightenedCount = 0;
        }
    }
    
    //  Target closes pill, run from ghosts?
    void PacmanAI()
    {
        Sprite* pacman;
        pacman = _sprites + PACMAN;
        
        //  Chase frightened ghosts
        Sprite* closestGhost = NULL;
        Sprite* frightenedGhost = NULL;
        short dist = 0x7FFF;
        for (byte i = 0; i < 4; i++)
        {
            Sprite* s = _sprites+i;
            short d = s->Distance(pacman->cx,pacman->cy);
            if (d < dist)
            {
                dist = d;
                if (s->state == FrightenedState)
                    frightenedGhost = s;
                closestGhost = s;
            }
        }
        if (frightenedGhost)
        {
            pacman->Target(frightenedGhost->cx,frightenedGhost->cy);
            return;
        }
        
        // Under threat; just avoid closest ghost
        if (dist < 16)
        {
            pacman->Target(pacman->cx*2 - closestGhost->cx,pacman->cy*2 - closestGhost->cy);
            return;
        }
        
        //  Go for the pill
        if (GetDot(2,4))
            pacman->Target(2,4);
        else if (GetDot(28,4))
            pacman->Target(28,4);
        else if (GetDot(2,17))
            pacman->Target(2,17);
        else if (GetDot(28,17))
            pacman->Target(28,17);
        else
        {
            // closest dot
            short dist = 0x7FFF;
            for (byte y = 4; y < 32; y++)
            {
                for (byte x = 1; x < 26; x++)
                {
                    if (GetDot(x,y))
                    {
                        short d = pacman->Distance(x,y);
                        if (d < dist)
                        {
                            dist = d;
                            pacman->Target(x,y);
                        }
                    }
                }
            }
            
            if (dist == 0x7FFF) // No dots, just avoid closest ghost
                pacman->Target(pacman->cx*2 - closestGhost->cx,pacman->cy*2 - closestGhost->cy);
        }
    }
    
    void Scatter(Sprite* s)
    {
        const byte* st = _scatterTargets + (s->who << 1);
        s->Target(pgm_read_byte(st),pgm_read_byte(st+1));
    }

    // sticky joystick read
    uint16_t joy()
    {
        uint16_t* j = readJoy();
        if (j[0])
            _lastjoy = j[0];
        return _lastjoy;
    }
    
    void UpdateTargets()
    {
        if (_state == ReadyState)
            return;

        // Switch out of attract mode
        if (_ai && *readJoy()) {
            init(); //
            _ai = 0;
        }

        Sprite* pacman = _sprites + PACMAN;
        if (_ai)
            PacmanAI();

        //  Ghost AI
        bool scatter = _scIndex & 1;
        for (byte i = 0; i < 4; i++)
        {
            if (i == PACMAN && !_ai)
                break;

            Sprite* s = _sprites+i;

            //  Deal with returning ghost to pen
            if (s->state == DeadEyesState)
            {
                if (s->cx == PENX && s->cy == PENY) // returned to pen
                {
                    s->state = PenState;        // Revived in pen
                    s->pentimer = 80;
                }
                else
                    s->Target(PENX,PENY);           // target pen
                continue;           //
            }
            
            //  Release ghost from pen when timer expires
            if (s->pentimer)
            {
                if (--s->pentimer)  // stay in pen for awhile
                    continue;
                s->state = RunState;
            }
            
            if (InPen(s->cx,s->cy))
            {
                s->Target(PENX,PENY-2); // Get out of pen first
            } else {
                if (scatter || s->state == FrightenedState)
                    Scatter(s);
                else
                {
                    // Chase mode targeting
                    byte tx = pacman->cx;
                    byte ty = pacman->cy;
                    switch (s->who)
                    {
                        case PINKY:
                        {
                            const char* pto = _pinkyTargetOffset + ((pacman->dir-1)<<1);
                            tx += (signed char)pgm_read_byte(pto);
                            ty += (signed char)pgm_read_byte(pto+1);
                        }
                            break;
                        case INKY:
                        {
                            const char* pto = _pinkyTargetOffset + ((pacman->dir-1)<<1);
                            Sprite* binky = _sprites + BINKY;
                            tx += (signed char)pgm_read_byte(pto)>>1;
                            ty += (signed char)pgm_read_byte(pto+1)>>1;
                            tx += tx - binky->cx;
                            ty += ty - binky->cy;
                        }
                            break;
                        case CLYDE:
                        {
                            if (s->Distance(pacman->cx,pacman->cy) < 64)
                            {
                                const byte* st = _scatterTargets + CLYDE*2;
                                tx = (signed char)pgm_read_byte(st);
                                ty = (signed char)pgm_read_byte(st+1);
                            }
                        }
                            break;
                    }
                    s->Target(tx,ty);
                }
            }
        }
    }
    
    //  Default to current direction
    byte ChooseDir(int dir, Sprite* s)
    {
        short choice[4];
        choice[0] = Chase(s,s->cx,s->cy-1);   // Up
        choice[1] = Chase(s,s->cx-1,s->cy);   // Left
        choice[2] = Chase(s,s->cx,s->cy+1);   // Down
        choice[3] = Chase(s,s->cx+1,s->cy);   // Right

        // Steer pacman as best we can
        if (!_ai && s->who == PACMAN)
        {
            uint16_t j = joy();
            if (choice[0] != 0x7FFF && (j & UP)) return MUp;
            if (choice[1] != 0x7FFF && (j & LEFT)) return MLeft;
            if (choice[2] != 0x7FFF && (j & DOWN)) return MDown;
            if (choice[3] != 0x7FFF && (j & RIGHT)) return MRight;
            if (choice[4-dir] == 0x7FFF)
                return MStopped;
            return dir;
        }
        
        // Don't choose opposite of current direction?
        
        short dist = choice[4-dir]; // favor current direction
        byte opposite = OppositeDirection(dir);
        for (byte i = 0; i < 4; i++)
        {
            byte d = 4-i;
            if (d != opposite && choice[i] < dist)
            {
                dist = choice[i];
                dir = d;
            }
        }
        if (dist == 0x7FFF)
            return opposite;
        return dir;
    }
    
    bool InPen(byte cx, byte cy)
    {
        if (cx < (PENX-2) || cx > (PENX+2)) return false;
        if (cy < PENY || cy > (PENY+1)) return false;
        return true;
    }
    
    byte GetSpeed(Sprite* s)
    {
        if (s->who == PACMAN)
            return _frightenedTimer ? 90 : 80;
        if (s->state == FrightenedState)
            return 40;
        if (s->state == DeadEyesState)
            return 100;
        if (s->cy == 11 && (s->cx <= 5 || s->cx > 31-5))
            return 40;  // tunnel
        return 75;
    }
    
    void MoveAll()
    {
        _counter++;
        UpdateTimers();
        UpdateTargets();
        
        //  Update game state
        if (_stateTimer)
        {
            if (--_stateTimer == 0)
            {
                switch (_state)
                {
                    case ReadyState:
                        _state = PlayState;
                        break;
                    case DeadGhostState:
                        _state = PlayState;
                        for (byte i = 0; i < 4; i++)
                        {
                            Sprite* s = _sprites + i;
                            if (s->state == DeadNumberState)
                                s->state = DeadEyesState;
                        }
                        break;
                    default:
                        ;
                }
            } else {
                if (_state == ReadyState)
                    return;
            }
        }
        
        for (byte i = 0; i < 5; i++)
        {
            Sprite* s = _sprites + i;
            
            //  In DeadGhostState, only eyes move
            if (_state == DeadGhostState && s->state != DeadEyesState)
                continue;
            
            //  Calculate speed
            s->speed += GetSpeed(s);
            if (s->speed < 100)
                continue;
            s->speed -= 100;
            
            s->lastx = s->_x;
            s->lasty = s->_y;
            s->phase++;
            
            int x = s->_x;
            int y = s->_y;
            
            if ((x & 0x7) == 0 && (y & 0x7) == 0)   // cell aligned
                s->dir = ChooseDir(s->dir,s);       // time to choose another direction

            switch (s->dir)
            {
                case MLeft:     x -= 1; break;
                case MRight:    x += 1; break;
                case MUp:       y -= 1; break;
                case MDown:     y += 1; break;
            }
            
            //  Wrap x because of tunnels
            while (x < 8)
                x += 28*8;
            while (x >= 29*8)
                x -= 28*8;
            
            s->_x = x;
            s->_y = y;
            s->cx = (x + 4) >> 3;
            s->cy = (y + 4) >> 3;
            
            if (s->who == PACMAN)
                EatDot(s->cx,s->cy);
        }
        
        //  Collide
        Sprite* pacman = _sprites + PACMAN;
        for (byte i = 0; i < 4; i++)
        {
            Sprite* s = _sprites + i;
            if (s->cx == pacman->cx && s->cy == pacman->cy)
            {
                if (s->state == FrightenedState)
                {
                    s->state = DeadNumberState;     // Killed a ghost
                    _state = DeadGhostState;        // pause for 2
                    _stateTimer = 2*FPS;
                    Score((1 << _frightenedCount)*100);
                    _frightenedCount++;
                    music_play(_got_ghost);
                }
                else
                    ;               // pacman died
            }
        }
    }
        
    void Score(int delta)
    {
        _score += delta;
        long score = _score;
        //score = v_underflow;
        uint8_t j = 8;
        do
        {
            _scoreStr[--j] = (score%10) + 48;
            score /= 10;
        } while(score);
        
        while (j--)
            _scoreStr[j] = 1;
    }
    
    bool GetDot(byte cx, byte cy)
    {
        return _dotMap[cy*4 + (cx >> 3)] & (0x80 >> (cx & 7));
    }
    
    void EatDot(byte cx, byte cy)
    {
        if (!GetDot(cx,cy))
            return;
        
        byte mask = 0x80 >> (cx & 7);
        _dotMap[cy*4 + (cx >> 3)] &= ~mask;
        _dots--;
        
        byte t = GetTile(cx,cy);
        if (t == PILL)
        {
            _frightenedTimer = 10*FPS;
            _frightenedCount = 1;
            for (byte i = 0; i < 4; i++)
            {
                Sprite* s = _sprites+i;
                if (s->state == RunState)
                {
                    s->state = FrightenedState;
                    s->dir = OppositeDirection(s->dir);
                }
            }
            effect(_pellet,true);
            Score(50);
        }
        else {
            effect(_wukka);
            Score(10);
        }
    }
    
    virtual void init()
    {
        _frightenedCount = 0;
        _frightenedTimer = 0;
        _ai = 1;   // autoplay
        _lastjoy = 0;

        const byte* s = _initSprites;
        for (int i = 0; i < 5; i++)
            _sprites[i].Init(s + i*5);
        
        _scIndex = 0;
        _scTimer = 1;
        
        // score an icons
        _score = 0;
        Score(0);
        memset(_icons,0,sizeof(_icons));
        _icons[0] = _icons[1] = _icons[2] = PACMANICON;
        
        //  Init dots from rom
        byte* map = _dotMap;
        _dots = 0;
        memset(_dotMap,0,sizeof(_dotMap));
        for (int i = 0; i < 32*24; i++) {
            byte t = pgm_read_byte(_map + i);
            if (t == DOT || t == PILL) {
                map[i>>3] |= (0x80 >> (i&7));
                _dots++;
            }
        }
        
        _state = ReadyState;
        _stateTimer = 5*FPS;
        music_play(_pacman_intro);
    }
    
    virtual void step()
    {
        MoveAll();
        sprite_init(_tiles,sizeof(_tiles) >> 4);
        for (int i = 0; i < 5; i++)
        {
            Sprite& s = _sprites[i];
            int n = s.SetupDraw(0,_frightenedTimer);
            const uint8_t* d = GET_SPRITE(n);
            sprite_add(d,s._x>>1,s._y);
        }
    }
    
    //
    virtual uint8_t get_tile(uint8_t x, uint8_t y)
    {
       //     return ((x+y)& 1) + 1;

        int i = y*32+x;
        uint8_t t = pgm_read_byte(_map + i);
        if (t == DOT || t == PILL)
        {
            if (!(_dotMap[i >> 3] & (0x80 >> (i&7))))
                t = 1;
            else if (t == PILL && (_counter & 0x20))
                t = 1;
        }
        return t;
    }
    
//  Draw playfield by inserting 1 row at a time.
//  Rows are double buffered

#define ROWS 40
    VRowTiles _rows[4];
    uint8_t _buf[ROWS*4]; // can be 32*2 TODO
    
    virtual void draw()
    {
        // Init tile line buffers
        for (uint8_t r = 0; r < 4; r++)
        {
            VRowTiles* row = _rows + r; // 2 bufs
            row->row.mode = VMODE_TILES;
            row->rom_font = _tiles;
            row->rom_font_count = sizeof(_tiles)/16;
            row->tiles = _buf + r*ROWS;
            row->v_scroll = 0;
        }

        // Draw
        for (uint8_t r = 0; r < 24; r++)
        {
            VRowTiles* row = _rows + (r&3);             // 2 bufs
            while (video_queue_count() == 4)
              ;
            row->row.y = (r << 3) + 7;
            for (uint8_t x = 0; x < 32; x++)
                row->tiles[x] = get_tile(x,r);
            
            // Patch special lines
            switch (r) {
                case 0:
                    for (uint8_t x = 0; x < 8; x++)
                        row->tiles[x] = _scoreStr[x];    // SCORE
                    break;
                case 13:
                    if (_state == ReadyState)
                        for (uint8_t x = 13; x < 19; x++)
                            row->tiles[x] = x + 58-13;   // READY
                    break;
            }
            row->ram_font = sprite_draw(row->tiles,r);  // Render sprites for this row
            video_enqueue((VRow*)row);
        }
        
        // don't need to wait
        music();
    }
    
    void effect(const uint8_t* e, bool force = false)
    {
        if (!music_playing(0) || force)
            music_play(e);      // wukka on 0
    }
    
    void siren()
    {
        const uint8_t* t = _siren_slow; // 282 dots in total?
        uint8_t n = 68;
        if (_dots < 282/4) {
            t = _siren_fast;
            n = 76;
        }
        else if (_dots < 282/2) {
            t = _siren_medium;
            n = 72;
        }
        music_note(CH3,n,8);
        music_play(t,1);
    }
    
    // Ghost sirens in CH3
    // puku-puku in CH4
    void music()
    {
        if (!music_playing(1))  // Siren
        {
            switch (_state) {
                case ReadyState:
                    break;
                //case DeadGhostState:
                    //music_play(_got_ghost,1);
                //    break;
                default:
                    if (_frightenedCount == 0)  // may stick.
                        siren();
                    else
                        music_play(_power,1);
            }
        }
        music_kernel();
    }
};

Pacman _pacman;