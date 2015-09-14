//
//  main.cpp
//
//  Created by Peter Barrett on 2/17/13.
//  Copyright (c) 2013 Peter Barrett. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <SDL.h>
#include <SDL_audio.h>
#include <pthread.h>
#include "math.h"
#include "unistd.h"

__attribute__((weak))
void video_blanking(uint8_t line) {};

__attribute__((weak))
void video_begin_post(uint8_t phase) {}

// Player 1 up,down,left,right space
// Player 2 w z a d s

//================================================================================
//================================================================================
//  Select one of the following

#define BALLBLASTER_GAME
#ifdef BALLBLASTER_GAME
#include "ballblaster/ballblaster.ino"
#else
#include "pacman/pacman.ino"
//#include "jowst/jowst.ino"
//#include "titan/titan.ino"
#endif

//================================================================================
//================================================================================
#include "video.h"

SDL_Surface *screen;
#define SDL_WIDTH   640
#define SDL_HEIGHT  (VROWS*8*2)
#define SDL_DEPTH   32

#define VROWS_BUFS   8 // POWER OF 2
#define VROWS_MASK (VROWS_BUFS-1)

VRow* _vrows[VROWS_BUFS];
volatile uint8_t _vread;
volatile uint8_t _vwrite;
int v_current_line = 192;

//================================================================================
//================================================================================
//  ntsc color generation from phase pixels

// LINE BUFFER
#define CHROMA_WIDTH 160
uint8_t _buf[CHROMA_WIDTH/2*2]; // sLop

int bitat(int pos)
{
    if (pos < 0)
        pos = 0;
    if (pos >= CHROMA_WIDTH*4)
        pos = CHROMA_WIDTH*4-1;
    return _buf[pos >> 3] & (0x80 >> (pos & 7)) ? 1 : 0;
}

static int gamma(float f)
{
    if (f < 0)
        return 0;
    f = pow(f, 2.2f/2.0f);
    if (f > 1)
        return 255;
    return f*255;
}

#ifdef BALLBLASTER_GAME
#define BURST_PHASE 1.0f
#else
#define BURST_PHASE 1.5f
#endif

//  4 phases, 4 bits ,0x11 colorburst
int _rgb_table[256];
int _rgb_table_inited = false;
int make_rgb_table(int phz, int bits)
{    
    float y = 0;
    float i = 0;
    float q = 0;
    for (int p = 0; p < 4; p++)
    {
        float phase = M_PI/2*(BURST_PHASE + phz++);    // 4 phases, 4 bits ,0x11 colorburst
        float luma = (bits & 0x8) ? 0.94f : 0.06f;
        y  =  y + luma;
        i  =  i + luma * cos(phase);
        q  =  q + luma * sin(phase);
        bits <<= 1;
    }
    y /= 3;
    i /= 2.5;
    q /= 2.5; // punch up saturation a bit
    
    int r = gamma(y +  0.946882f*i +  0.623557f*q);
    int g = gamma(y + -0.274788f*i + -0.635691f*q);
    int b = gamma(y + -1.108545f*i +  1.709007f*q);
    return (b << 24) | (g << 16) | (r << 8) | 0xFF;
}

int get_rgb(int begin)
{
#if 0
    for (int i = 0; i < 16; i++)
    {
        uint32_t rgb = make_rgb_table(0,i) >> 8;
        uint8_t r = rgb;
        uint8_t g = rgb>>8;
        uint8_t b = rgb>>16;
        printf("\"#%02X%02X%02X\",\n",r,g,b);
    }
     for (int i = 0; i < 16; i++)
     {
         uint32_t rgb = make_rgb_table(0,i) >> 8;
         uint8_t r = rgb;
         uint8_t g = rgb>>8;
         uint8_t b = rgb>>16;
         printf("0x%02X%02X%02X,\n",r,g,b);
     }
#endif
    
    int bits = 0;
    int p = 4;
    while (p--)
        bits = (bits << 1) | bitat(begin++);
    int phz = begin & 3;                // 4 bits/4 phases
    
    if (!_rgb_table_inited) {
        _rgb_table_inited = true;
        for (int i = 0; i < 256; i++)
            _rgb_table[i] = make_rgb_table(i >> 4,i & 0xF);
    }
    return _rgb_table[(phz << 4) | bits];
}

#if 0
static void bars()
{
    for (int x = 0; x < CHROMA_WIDTH/2; x++)
    {
        int p = x/5;
        _buf[x] = p | (p << 4);
    }
}

static void c256()
{
    int color = v_current_line/12;
    color &= 0xF;
    for (int x = 0; x < CHROMA_WIDTH/2; x++)
    {
        int p = x/3;
        if (v_current_line & 1)
            _buf[x] = color | (p << 4);
        else
            _buf[x] = p | (color << 4);
    }
}
#endif

uint8_t _line_count = 0;
void render_line_ntsc(uint8_t line, int* dst)
{    
    //bars();
    //c256();
    //bb2(line);

    for (int pp = 0; pp < 4; pp++)
    {
        for (int x = 0; x < CHROMA_WIDTH; x++)
        {
            int center = x*4+pp;
            dst[center] = get_rgb(center - 2);
        }
    }
    _line_count++;
}


// TODO: 0..ACTIVE_LINES?
int video_current_line()
{
    return v_current_line;
}

uint8_t video_queue_count()
{
    return _vwrite-_vread;
}

void flush_video(bool cb = false);
void video_enqueue(VRow* row)
{
    while (_vwrite-_vread == VROWS_BUFS)
        ; // Queue is full
    _vrows[_vwrite++ & VROWS_MASK] = row;
    flush_video();
}

#ifdef BALLBLASTER_GAME

#define BUFFER_SIZE 1350
#define MAX_LINE_BYTES 64

extern volatile uint8_t* _read_rle;  // rle blit buffer
extern uint8_t _bb_buf[BUFFER_SIZE+MAX_LINE_BYTES];

void blit_rle()
{

    volatile uint8_t* src = _read_rle;
    //printf("R:%d\n",_read_rle-_bb_buf);
    int8_t run;
    uint8_t c,last;
    uint8_t x = 72;
    last = src[3];  // Single pixel run at start of line
l0:
    run = *src++;       // 6 2
    c = *src++;         // 8 2
   // printf(" %02X:%02X",(uint8_t)run,c);
    if (run > 0)        // 9 1
        goto even;      // 10 1:3
odd:
    last |= c & 0x0F;   // 11 2
    PIXEL_OUT(last);    // 13 2
    last = c;           // bad if next is odd
    if (!--x)           // 0 1
        goto l99;       // 1 1:3
    if (!(run &= 0x7F)) // 2 1
        goto l0;        // 3 1:3    // only bad here
    goto loop;          // 4 3

even:
    PIXEL_OUT(c);       // 13 2
    last = c & 0xF0;    // 15 1
    if (!--x)           // 0 1
        goto l99;       // 1 1:3
    if (!--run)         // 2 1
        goto l0;        // 3 1:3  // 16
    goto loop;          // 4 3
    
noploop:
    asm("nop"); // 4
    asm("nop"); // 5
    asm("nop"); // 6
    
loop:
    last = c & 0xF0;    // 7 2
    --x;                // 9 1
    asm("nop");         // 10
    asm("nop");         // 11
    asm("nop");         // 12
    PIXEL_OUT(c);       // 13 2
    asm("nop");         // 15
    if (--run)          // 0 1
        goto noploop;   // 1 1:3
    if (x)              // 2 1
        goto l0;        // 3 1:3
l99:
    _read_rle = src;    // No wrapping
}

//
// Strip in middle (good advice)
extern
uint8_t _cellmap[32*2];
extern
const uint8_t* _celltiles;

void blit_tiles(uint8_t line)
{
    const uint8_t* font = _celltiles + ((line & 7) << 8);   // 128 chars in tilemap
    uint8_t* src = _cellmap;
    if (line >= 8)
        src += 32;

    for (int i = 0; i < 4; i++)
        PIXEL_OUT(0x00);

    uint8_t n = 32;
    do
    {
        uint8_t c = *src++;
        const uint8_t* f = font+c+c;
        if (c == 0)                 // 2 or 5
            break;
        PIXEL_OUT(pgm_read_byte(f++));
        PIXEL_OUT(pgm_read_byte(f++));
    } while(--n);  // 4
}

__attribute__((weak))
void video_active(uint8_t line)
{
    if (line < 87 || line >= (87+18))   // 18 lines in the middle
    {
        //printf("%d: ",line);
        blit_rle();
        //printf("\n");
    }
    else {
        line -= 88;
        if (line < 16)
            blit_tiles(line);
        else
        {
            for (int i = 0; i < 80; i++)
                PIXEL_OUT(0);
        }
        video_begin_post(1);
    }
}
#endif

void render_line(uint8_t line, int* dst)
{
    //UDR0 = 0;
    VRow* r = _vrows[_vread & VROWS_MASK];    // get the next row
    VRowTiles* row = (VRowTiles*)r;
    if (line == r->y)
        _vread++;   // y where line is removed
    
    line = (line + row->v_scroll) & 7;
    const uint8_t* font = row->rom_font + (line << 1)*row->rom_font_count;
    const uint8_t* ram_font = row->ram_font + (line << 1);
    uint8_t ram_start = row->rom_font_count;
    
    uint8_t* src = row->tiles;
    uint8_t* d = _buf;
    
    // 40 character output
    uint8_t a = VCOLS;
    do
    {
        uint8_t c = *src++;
        if (c < ram_start)
        {
            if (c == 0)
            {
                memset(d,0,2*a);    // early out
                break;
            }
            const uint8_t* f = font+(c<<1);
            *d++ = *f++;
            *d++ = *f++;
        } else {
            c -= ram_start;
            //c <<= 2;
            const uint8_t* s = ram_font + (c<<4); // can live without shift (todo: > 16)
            *d++ = *s++;
            *d++ = *s++;
        }
    } while (--a);
    
    render_line_ntsc(line,dst);
}

void video_start()
{
}

//================================================================================
//================================================================================

bool _running;
bool running()
{
    return _running;
}

void setup();
void loop();

int gameThread(void* p)
{
    setup();
    while (_running)
        loop();
    _running = false;
    return 0;
}

unsigned int ir_data;
unsigned char ir_count;
int _phase = 0;
int _repeat = 0;
uint8_t v_framecounter = 0;
int _buf_x = 0;
void PIXEL_OUT(uint8_t p)
{
    _buf[_buf_x++] = p;
}

void music_kernel();
void flush_video(bool cb)
{
    //bool doubleLine = true;
    if(SDL_MUSTLOCK(screen))
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    
    if (v_current_line == 192)
    {
        //music_kernel();
        v_current_line = 0;
#ifdef BALLBLASTER_GAME
        _read_rle = _bb_buf;
#endif
        video_begin_post(0);
        for (int i = 0; i < 40; i++)
            video_blanking(i);
    }
    
    int rowBytes = screen->pitch/4;
    while ((_vread - _vwrite) || cb)
    {
        int* dst = (int*)screen->pixels + rowBytes*(v_current_line*2 + (v_framecounter & 1));
        if (cb) {
            _buf_x = 0;
#ifdef BALLBLASTER_GAME
            video_active(v_current_line);
#endif
            render_line_ntsc(v_current_line,dst);
            cb = false;
        } else
            render_line(v_current_line,dst);
        //if (doubleLine)
        //    memcpy(dst+rowBytes,dst,rowBytes*4);
        v_current_line++;
        if (v_current_line == 192)
            break;
    }
    
    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    
    if (v_current_line == 192)
    {
        v_framecounter++;
        _vread = _vwrite;
        SDL_Flip(screen);
    }
    
    /*
    if (_key && (++_repeat > 6))
    {
        ir_data = _key;
        ir_count = 32;
        _repeat = 0;
    }
     */
}

//============================================================================
//============================================================================
//  Audio sim

#define AUDIO_BUFFER 1024
#define AUDIO_MASK (AUDIO_BUFFER-1)
unsigned char _audio[AUDIO_BUFFER];
int _read = 0;
int _write = 0;
void AUDIO_OUT(int p)
{
    _audio[_write++ & AUDIO_MASK] = p;
}

extern uint8_t _mix;
void AUDIO_STEP();
void step_audio()
{
    for (int i = 0; i < 262; i++)
    {
        AUDIO_STEP();
        AUDIO_OUT(128-_mix);
    }
}

void audio_callback(void *userdata, Uint8 *stream, int len)
{
    step_audio();
    short* s = (short*)stream;
    len >>= 1;
    for (int i=0;i<len;i++)
    {
        if (_read >= _write)
            break;
        *s++ = (_audio[_read++ & AUDIO_MASK]-128) << 8;
      //  if ((_read & 0x3FFF) == 0)
      //      fprintf(stderr,"bfu %d\n",_write-_read);
    }
}

//============================================================================
//============================================================================
//  Joystickwsx sim

uint16_t _joy[2];
uint16_t* readJoy()
{
    return _joy;
}

void maskkey(bool down, int mask, uint8_t key)
{
    if (down)
        _joy[key] |= mask;
    else
        _joy[key] &= ~mask;
}

void keymask(int key, bool down)
{
    switch (key) {
        case SDLK_UP:       maskkey(down,UP,0); break;
        case SDLK_DOWN:     maskkey(down,DOWN,0); break;
        case SDLK_RIGHT:    maskkey(down,RIGHT,0); break;
        case SDLK_LEFT:     maskkey(down,LEFT,0); break;
        case SDLK_SPACE:    maskkey(down,FIRE,0); break;
            
        case SDLK_w:        maskkey(down,UP,1); break;
        case SDLK_z:        maskkey(down,DOWN,1); break;
        case SDLK_a:        maskkey(down,LEFT,1); break;
        case SDLK_d:        maskkey(down,RIGHT,1); break;
        case SDLK_s:        maskkey(down,FIRE,1); break;
        default:break;
    }
}

int main(int argc, char* argv[])
{
    SDL_Event event;
    
    int keypress = 0;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) return 1;
    
    if (!(screen = SDL_SetVideoMode(SDL_WIDTH, SDL_HEIGHT, SDL_DEPTH, 0)))
    {
        SDL_Quit();
        return 1;
    }
    
    _running = true;
    SDL_Thread *thread;
    thread = SDL_CreateThread(gameThread, NULL);
    
    SDL_AudioSpec desired = {0};
    SDL_AudioSpec obtained = {0};

    desired.freq= 15734; // NTSC line rate
    desired.format=AUDIO_S16;
    desired.channels=1;
    desired.samples=262;
    desired.callback=audio_callback;
    
    /* Open the audio device */
    if ( SDL_OpenAudio(&desired, &obtained) < 0 ){
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(-1);
    }
    SDL_PauseAudio(0);
    SDL_EnableKeyRepeat(1,30);

    while(!keypress)
    {
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    keypress = 1;
                    break;
                case SDL_KEYUP:
                    keymask(event.key.keysym.sym,0);
                    break;
                    
                case SDL_KEYDOWN:
                    keymask(event.key.keysym.sym,1);
                    break;
            }
        }
    }
    _running = false;
    SDL_WaitThread(thread, NULL);
    SDL_Quit();
    return 0;
}
