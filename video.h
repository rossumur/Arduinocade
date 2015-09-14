/* Copyright (c) 2015, Peter Barrett  
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

#ifndef __VIDEO_H__
#define __VIDEO_H__

#define VCOLS 40
#define VROWS 24

#define VMODE_TILES     0
#define VMODE_RLE       1
#define VMODE_CALLBACK  2

#ifndef ARDUINO
void PIXEL_OUT(uint8_t p);
void flush_video(bool cb);
#define PAUSE_VIDEO()
#define UNPAUSE_VIDEO()
#define RELEASE_VIDEO()
#else
#define PIXEL_OUT(_p) UDR0 = _p
#define PAUSE_VIDEO() UBRR0L = 0xFF
#define UNPAUSE_VIDEO() UBRR0L = 0
#define RELEASE_VIDEO() UCSR0B = 0
#define flush_video(_b)
#endif

void video_active(uint8_t line);
void video_blanking(uint8_t line);

class VRow
{
public:
    uint8_t mode;
    uint8_t y;                  // 0xFF if empty
};

class VRowTiles
{
public:
    VRow row;
    uint8_t v_scroll;
    uint8_t width;              //
    uint8_t rom_font_count;     // # of pgm based chars
    const uint8_t* rom_font;    // rom based, interleaved
    const uint8_t* ram_font;    // block
    uint8_t* tiles;             //
};

class VRowRLE
{
public:
    VRow row;
    uint8_t skip;               // leading black
    uint8_t width;              // scroll tex etc
    const uint8_t* rle;         // rom based, interleaved
};

void video_enqueue(VRow* row);  // can mix and match these (row of score etc)
int video_current_line();
uint8_t video_queue_count();
void video_start();

void audio_beep(uint8_t freq, uint8_t cycles);
uint8_t readKey();

#endif
