/* Copyright (c) 2010-2015, Peter Barrett
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

// Video output, audio tone generation and IR keyboard scanning events

#include "config.h"
#include "video.h"

// PORT B
#define SYNC_PIN    1
#define IR_PIN      0

// PORT D
#define AUDIO_PIN       6
#define PS2_CLOCK_PIN   3
#define PS2_DATA_PIN    2
#define VIDEO_PIN       1

// Color clock frequency is 315/88
// CPU Mhz is 315/11
// 455/2 color clocks per line
// HSYNCH period is 44/315*455 or 63.55555..us
// Field period is 262*44/315*455 or 16651.5555us

// ~1820 cpu clocks per line at 28.6363Mhz
#define USEC(_x)         (((_x)*28.6363)-1)
#define TICKS_SCANLINE   (1824-1)
#define TICKS_HSYNC      USEC(4.7)
#define TICKS_LONG_HSYNC USEC(63.555-4.7)
#define TICKS_HBLANK     USEC((4.7+1.5))
#define TICKS_INTERRUPT  USEC(4.05)  // late as possible 4.2

#define TEXT_ROWS 24
#define TEXT_COLS 40
#define BLANK_LINES 40
#define ACTIVE_LINES (TEXT_ROWS*8)

#define STATE_VBLANK  0
#define STATE_PRE     1
#define STATE_ACTIVE  2
#define STATE_POST    3

// does not do halfline sync during blanking
//__attribute__((section(".progmem.data")))
PROGMEM
const uint8_t v_lines[] = { 3,BLANK_LINES,ACTIVE_LINES,262-(3+BLANK_LINES+ACTIVE_LINES) };

// Video State
volatile uint8_t v_state = 0;
volatile uint8_t v_count = 0;
uint8_t v_vbicountdown = 0;

// TODO
uint8_t v_framecounter;
uint16_t _linecount;

// Audio state
uint8_t a_count = 0;
uint8_t a_freq = 0;
uint8_t a_cycles = 0;

// IR state
uint8_t ir_last = 0;
uint8_t ir_count = 0;

//==========================================================
//==========================================================
//  Webtv keyboard

// Report changes on IR pin to keyboard handler
// t is HSYNCH ticks
void keyboardIREvent(uint8_t t, uint8_t v);

//==========================================================
//==========================================================
// PS2 keyboard

void enable_ps2();
void disable_ps2();

//==========================================================
//==========================================================
//  audio for keyboard/io feedback

void audio_beep(uint8_t freq, uint8_t cycles)
{
    a_freq = freq;
    a_cycles = cycles;
    a_count = freq;
}

//==========================================================
//==========================================================
// Video buffering state machine

#define VROWS_BUFS   4 // POWER OF 2
#define VROWS_MASK (VROWS_BUFS-1)

volatile VRow* _vrows[VROWS_BUFS];
volatile uint8_t _vread;
volatile uint8_t _vwrite;

// TODO: 0..ACTIVE_LINES?
int video_current_line()
{
    if (v_state == STATE_ACTIVE)
        return ACTIVE_LINES-v_count;
    return v_state == STATE_POST ? ACTIVE_LINES : -1;
}

uint8_t video_queue_count()
{
    return _vwrite-_vread;
}

void video_enqueue(VRow* row)
{
    while (_vwrite-_vread == VROWS_BUFS)
        ; // Queue is full
    _vrows[_vwrite++ & VROWS_MASK] = row;
}

//==========================================================
//==========================================================
//  Video ISR

static void inline tick_align(uint8_t tick)
{
    __asm__ __volatile__ (
                          "sub	%[tick], %[tcnt1l]\n"
                          "0: subi	%[tick], 3\n"
                          "brcc	0b\n"
                          "cpi      %[tick],-3\n"
                          "breq     1f\n"
                          "cpi      %[tick],-2\n"
                          "brne     .\n"
                          "1:\n"
                          :: [tick] "a" (tick),
                          [tcnt1l] "a" (TCNT1L)
                          );
}

void rle_blit(VRowRLE* row)
{
    UDR0 = 0;
    uint8_t run = row->skip;
    uint8_t x = row->width;
    const uint8_t* rle = row->rle;
    if (!rle)
        return;
    uint8_t color = 0;
    
    //  16 clocks thru all paths
    for(;;)
    {
        UDR0 = color;          // 2
        asm ("nop");           // 1
        if (!--run)            // 1+(1:3)
        {
            if (!x)              // 1+(1:3)
                break;
            run = rle[0];        // 2
            color = rle[1];      // 2
            rle += 2;            // 2 (stupid)
            x-= run;             // 1
            continue;            // 3
        }
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
        asm ("nop");            // 1
    }
}

#define BUFFER_SIZE 1350
#define MAX_LINE_BYTES 64
extern volatile uint8_t* _read_rle;
extern uint8_t _bb_buf[BUFFER_SIZE + MAX_LINE_BYTES];

// Sorry for the assembly
// Touchy little bit of code that draws runs color/run pairs
// maintains 16 clocks/pixel pair through all paths

void blit_rle()
{
  PIXEL_OUT(0x00);
  uint8_t* src = (uint8_t*)_read_rle;
  asm(
"        sts	0x00C6, r1\n"   // PIXELOUT(0)
"        sts	0x00C6, r1\n"   // PIXELOUT(0)
"        ldd	r18, Z+3\n"     // color = src[3]
"        ldi	r19, 0x01\n"    // run = 1
"        ldi	r25, 144/2\n"   // x = 72
"        sts	0x00C6, r1\n"   // PIXELOUT(0)
"        sts	0x00C6, r1\n"   // PIXELOUT(0)

"0:\n"
"        ld	r19, Z+\n"    // 5     2 run
"        ld	r18, Z+\n"    // 7     2 c
"        cp	r1, r19\n"    // 9     1 if (run > 0)
"        brlt	1f\n"         // 10    1:2  goto even

// odd
"        mov	r24, r18\n"     // 11 last |= c & 0x0F;
"        andi	r24, 0x0F\n"    // 12
"        or	r20, r24\n"     // 13
"        sts	0x00C6, r20\n"  // 14 PIXEL_OUT(last);
"        subi	r25, 0x01\n"    // 0 if (!--x)
"        breq	99f\n"          // 1  goto l99;
"        andi	r19, 0x7F\n"    // 2 if (!(run &= 0x7F))
"        breq   0b\n"           // 3  goto l0;
"        rjmp   33f\n"          // 4 goto loop

//  even
"1:      \n"
"        mov	r20, r18\n"    // 13 last = c;
"        andi	r20, 0xF0\n"   // 14 last &= 0xF0
"        sts	0x00C6, r18\n" // 14 PIXEL_OUT(c); 
"        subi	r25, 0x01\n"   // 0  if (!--x)
"        breq	99f\n"         // 1   goto l99;
"        subi	r19, 0x01\n"   // 2  if (!--run)
"        breq   0b\n"          // 3   goto l0;
"        rjmp   33f\n"         // 4  goto loop

"22:\n"
"        nop\n" // 3
"        nop\n" // 4
"        nop\n" // 5
"33:\n"
"        mov	r20, r18\n"     // 6  last = c;
"        andi	r20, 0xF0\n"    // 7  last &= 0xF0
"        subi	r25, 0x01\n"    // 8  --x
"        nop\n"                 // 9
"        nop\n"                 // 10
"        nop\n"                 // 11
"        nop\n"                 // 12
"        nop\n"                 // 13
"        sts	0x00C6, r18\n"  // 14 PIXEL_OUT(c);
"        subi	r19, 0x01\n"    // 0  if (--run) 
"        brne  22b\n"           // 1   goto noploop;
"        and r25,r25\n"         // 2  if (x)
"        brne 0b\n"             // 3   goto l0;

"99:\n"
:"=z" (src)
:"z" (src)
: "r18","r19","r20","r24","r25"
);
  PIXEL_OUT(0x00);
  UCSR0B = 0;
  _read_rle = src;
}


#ifdef BALLBLASTER_GAME
// Simple tiles for 16 lines in middle of screen used by BALLBLASTER

extern const uint8_t* _celltiles;
extern uint8_t _cellmap[32*2];

void blit_tiles(uint8_t line)
{
    const uint8_t* font = _celltiles + ((line & 7)<<8);   // 128 chars in tilemap
    uint8_t* src = _cellmap + ((line << 2) & 0xE0);

    uint8_t n = 12;  // A little black on left
    while (n--)
      asm("nop");
   
    UNPAUSE_VIDEO();
    PIXEL_OUT(0);
    asm("nop");      // Prevent SPI overruns
    asm("nop");

    n = 32;
    do
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        
        uint8_t c = *src++;            // 2
        const uint8_t* f = font+c+c;   // 
        if (c == 0)                    // 2 or 3
            break;
        PIXEL_OUT(pgm_read_byte(f++));
        PIXEL_OUT(pgm_read_byte(f++));
    } while (--n);  // 3
}
#endif

//  Blit from 2 fonts, one a program memory font, the other a ram font
//  Normally containing composited sprites

//  Global dither/fade is possible
/*
uint8_t v_dither[] = {  
  0xAA,0xAA,
  0x55,0x55,
  0xAA,0xAA,
  0x55,0x55,
  0xAA,0xAA,
  0x55,0x55,
  0xAA,0xAA,
  0x55,0x55,
};*/


void tile_blit(VRowTiles* row, uint8_t line)
{
    //asm("nop");  // Color timing
    line <<= 1;
    uint16_t font = line*row->rom_font_count;
    font += (uint16_t)row->rom_font;
    uint8_t* src = row->tiles;

    // Global dither would be a nice add
    //uint8_t m0 = v_dither[line];
    //uint8_t m1 = v_dither[line+1];

    asm("nop");  // Color timing
    UNPAUSE_VIDEO();  // Get from unpause to loop
    UDR0 = 0x00;

    const uint8_t* ram_font = row->ram_font + line;
    uint8_t ram_start = row->rom_font_count;

    // 32 clocks/40 cols = 1280
   uint8_t n = 38;
    do
    {
        uint8_t b;
        uint8_t c = *src++;
        if (c >= ram_start)
        {
            c -= ram_start;
            c <<= 2;
            const uint8_t* s = ram_font + (c<<2); // can live without shift
            UDR0 = *s++;
            b = *s++;
        } else {
            if (c == 0)
              break;
            uint16_t f = font+c+c;
            UDR0 = pgm_read_byte(f++);
            b = pgm_read_byte(f++);
        }
        asm("nop");  // 5 unused cycles
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        UDR0 = b;
    } while(--n);  // 4
}

__attribute__((weak))
void video_active(uint8_t line) {};

__attribute__((weak))
void video_begin_post(uint8_t phase) {};

__attribute__((weak)) 
inline void video_blanking(uint8_t line) {};

//============================================================================
//============================================================================

#undef ARDUINO
#include "music.cpp"
#define ARDUINO

//============================================================================
//============================================================================
// Video ISR
// Handles video and audio generation as well as keyboard scanning

// phase for 0x11 is 1.0 or 1.5, extra nop to select
#define BURST 0x11

uint8_t _line_count;
ISR(TIMER1_COMPB_vect)
{
    OCR0A = 128-_mix;  // output last audio sample
    
    if (v_state != STATE_VBLANK)
    {
      // Generate 9 cycle colorburst + step audio engine
        tick_align(TICKS_HBLANK);  // Wait for burst time
        UDR0 = BURST;
        UCSR0B = 1<<TXEN0;
        _channels[0].pos += _channels[0].step;
        UDR0 = BURST;
        _channels[1].pos += _channels[1].step;
        UDR0 = BURST;
        _channels[2].pos += _channels[2].step;
        UDR0 = BURST;
        _channels[3].pos += _channels[3].step;
        UDR0 = 0;
        PAUSE_VIDEO();
    } else {
      // Just step audio engine
      _channels[0].pos += _channels[0].step;
      _channels[1].pos += _channels[1].step;
      _channels[2].pos += _channels[2].step;
      _channels[3].pos += _channels[3].step;
    }

 // Audio kernel in two parts for smaller code
 // compiler reordering can cause problems
 // timing for all of this was for gcc, will probably need to be re-tuned for llvm

 // Sample
    uint16_t* p;
    int8_t a0,a1,a2,a3;
    p = (uint16_t*)(&_channels[0].flags-1);
    a0 = pgm_read_byte(_wave_tables + *p);
    p = (uint16_t*)(&_channels[1].flags-1);
    a1 = pgm_read_byte(_wave_tables + *p);
    p = (uint16_t*)(&_channels[2].flags-1);
    a2 = pgm_read_byte(_wave_tables + *p);
    p = (uint16_t*)(&_channels[3].flags-1);
    a3 = pgm_read_byte(_wave_tables + *p);

 // Mix
    short mix = a0*(int8_t)_channels[0].volume;
    mix += a1*(int8_t)_channels[1].volume;
    mix += a2*(int8_t)_channels[2].volume;
    mix += a3*(int8_t)_channels[3].volume;
    _mix = mix >> 8;

 // Draw active video
    if (v_state == STATE_ACTIVE)
    {
        uint8_t line = ACTIVE_LINES-v_count;
#ifdef BALLBLASTER_GAME
          if (line < 87 || line >= (87+18))
          {
            asm("nop");  // Color timing
            asm("nop");
            UNPAUSE_VIDEO();
            PIXEL_OUT(0);
            blit_rle();          // 3D playfields
          }
          else {
            line -= 88;
            if (line < 16)
              blit_tiles(line);  // Score/Time
          }
#else
        VRow* r = (VRow*)_vrows[_vread & VROWS_MASK];    // get the next row
        //if (r->mode == VMODE_TILES)
        {
            tile_blit((VRowTiles*)r,(line + ((VRowTiles*)r)->v_scroll)&7); // (line + r->v_scroll)
            UCSR0B = 0;
            //if (line >= r->endy)  // y end
            if (line == r->y)  // y end
              _vread++;
        }
#endif
    }
    UCSR0B = 0;
    UNPAUSE_VIDEO();
    
    // Advance video state machine
    if (!--v_count)
    {
        v_state = (v_state+1) & 3;
        v_count = pgm_read_byte(v_lines + v_state);
        OCR1A = v_state ? TICKS_HSYNC : TICKS_LONG_HSYNC;
        if (v_state == STATE_ACTIVE)
        {
#ifdef BALLBLASTER_GAME
            _read_rle = _bb_buf;
#endif
            disable_ps2();         // don't field ps2 keyboard interrupts during active video
            if (v_vbicountdown)
                v_vbicountdown--;  // VBI countdown timer
        }
        else if (v_state == STATE_POST)
        {
            _vread = _vwrite;
            video_begin_post(0);
            enable_ps2();
            v_framecounter++;
        }
    }
    
    // IR keyboard/controllers
    uint8_t ir = PINB & 1;  // a little jitter
    if (ir != ir_last)
    {
        keyboardIREvent(ir_count,ir_last);
        ir_count = 0;
        ir_last = ir;
    }
    if (ir_count != 0xFF)
        ir_count++;
    _line_count++;
}

void video_start()
{     
    UBRR0H = 0;  // Fastest serial clock  
    UBRR0L = 0;
    UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0); // START SPI MODE

    DDRB |= _BV(SYNC_PIN);
    DDRB &= ~_BV(IR_PIN);
    DDRD |= _BV(AUDIO_PIN) | _BV(VIDEO_PIN);
    PORTD &= ~_BV(VIDEO_PIN);  // black when serial port is not driving TX
    
    DDRD &= ~_BV(PS2_DATA_PIN);  // DATA PIN FROM PS2

    DDRD |= 0x80; // Debug
    
    // Audio pwm
    TCCR0A |= _BV(COM0A1) | _BV(WGM00) | _BV(WGM01); // fast PWM, turn on OC0A
    TCCR0B = 1;
    OCR0A = 0x00;
    
    // Set OC1A on Compare Match, Fast PWM, No prescaling, Timer 1
    TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
    ICR1 = TICKS_SCANLINE;
    OCR1A = TICKS_HSYNC; 
    OCR1B = TICKS_INTERRUPT;
    TIMSK0 = 0;  // Turn off Timer0 (use ticks from vblank, syncs etc)
    TIMSK1 = _BV(OCIE1B);
    PORTD = 0;
    v_state = 3;
    v_count = 1;
    sei();
}

