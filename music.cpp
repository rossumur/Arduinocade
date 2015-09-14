//
//  music.cpp
//
//  Created by Peter Barrett on 7/17/14.
//  Copyright (c) 2014 Peter Barrett. All rights reserved.
//

#include "config.h"
#include "music.h"
#include "music_tables.h"

#ifndef ARDUINO // Include in video.cpp when building on arduino

//  Music has low level and high level kernels
//  Low level kernel
//      runs every line (15750000/1001 hz)
//      mixes channels, steps noise
//      emits pcm
//  High level kernel
//      runs every frame (60 hz)
//      steps note envelopes
//      traverses music tracks

// Instrument defs

// envelopes
// vol,duration?
// Patches?
#define E_SUSTAIN   0xFF

// Inline envelopes TODO
const uint8_t envelope_NONE[] PROGMEM = {
    240,E_SUSTAIN,0    // Attack,decay,sustain, 0 terminated
};

const uint8_t envelope_KEYBOARD[] PROGMEM = {
    120,240,160,E_SUSTAIN,60,1,0    // Attack,decay,sustain
};

const uint8_t envelope_HIHAT[] PROGMEM = {
    120,240,208,120,96,64,32,1,0
};

const uint8_t envelope_HIT[] PROGMEM = {
    120,240,240,120,0
};


const uint8_t envelope_FADE[] PROGMEM = {
    120,240,200,160,120,80,40,1,0
};

const uint8_t envelope_FADE2x[] PROGMEM = {
    120,240,220,200,180,160,140,120,100,80,60,40,20,1,0
};

const uint8_t envelope_FADE2[] PROGMEM = {
    120,240,220,200,180,160,160,160,160,160,160,160,140,120,100,80,60,40,20,1,0
};


const uint8_t* get_envelope(uint8_t i)
{
    switch(i) {
        case KEYBOARD:  return envelope_KEYBOARD;
        case HIHAT:     return envelope_HIHAT;
        case FADE:      return envelope_FADE;
        case FADE2:     return envelope_FADE2;
        case HIT:     return envelope_HIT;
    }
    return envelope_NONE;
}

// 6 bytes per
typedef struct {
    const uint8_t* envelope;    //
    uint8_t envelope_pos;
    uint8_t velocity;
    uint8_t note;
    uint8_t slide;
} AudioPatch;

//============================================================================
//============================================================================
//  Sample rate is 15750000/1001 or  15734.273hz
//  Every second frame? Alternate with IR? todo

AudioChannel _channels[4];  // low level
static AudioPatch _patches[4];     // high level
uint8_t _mix;

typedef struct {
    uint8_t delay;
    const uint8_t* track;
} music_state;
music_state _music_state[2] = {0};

//static uint8_t music_delay = 0;
//const uint8_t* music_track = 0;

void music_on(uint8_t channel, uint8_t volume, uint16_t step)
{
    AudioChannel& c = _channels[channel];
    c.volume = volume;
    c.step = step;
}

void music_off(uint8_t channel)
{
    _channels[channel].volume = 0;
}

void music_volume(uint8_t channel, uint8_t volume)
{
    _patches[channel].velocity = volume >> 2;    // 0..63
    if (volume == 0)
        _channels[channel].volume = 0;      // pass
}

bool music_playing(uint8_t index)
{
    return _music_state[index].track != 0;
}

void music_flag(uint8_t channel, uint8_t flags)
{
}

void music_note(uint8_t channel, uint8_t note, uint8_t velocity)
{
    AudioPatch& p = _patches[channel];
    p.velocity = velocity;    // TODO
    p.note = note;
    p.slide = 0;
    if (!p.envelope)
        p.envelope = get_envelope(0);
    p.envelope_pos = 0;
}

void music_note_off(uint8_t channel)
{
    AudioPatch& p = _patches[channel];
    if (p.velocity)
    {
       if (pgm_read_byte(p.envelope+p.envelope_pos) == E_SUSTAIN)
           p.envelope_pos++;
       else {
           music_off(channel);
           p.velocity = 0;
       }
    }
}

void music_slide(uint8_t channel, int8_t slide)
{
    _patches[channel].slide = slide;
}

int8_t music_step_track(uint8_t index)
{
    music_state& m = _music_state[index];
    if (!m.track || !m.delay || --m.delay) // ready for next step
        return 0;
    
    const uint8_t* d = m.track;
    for (;;)
    {
        uint8_t cmd = pgm_read_byte(d++);
        switch (cmd & 0xE0) {
            case 0:
            {
                switch (cmd) {
                    case M_DELAY:
                        m.delay = pgm_read_byte(d++);
                        m.track = d;
                        return 0;
                        
                    case M_WAVE:
                    case M_ENVELOPE:
                    case M_SLIDE:
                    case M_VOLUME:
                        {
                            uint8_t p0 = pgm_read_byte(d++);
                            uint8_t mask = p0;
                            p0 >>= 4;
                            uint8_t m = 1;
                            for (uint8_t i = 0; i < 4; i++)
                            {
                                if (m & mask)
                                {
                                    switch (cmd)
                                    {
                                        case M_WAVE:        _channels[i].flags = p0;                    break;
                                        case M_ENVELOPE:    _patches[i].envelope = get_envelope(p0);    break;
                                        case M_SLIDE:       music_slide(i,(int8_t)pgm_read_byte(d++));  break;
                                        case M_VOLUME:      music_volume(i,pgm_read_byte(d++));         break;
                                    }
                                }
                                m <<= 1;
                            }
                        }
                        break;
                                            
                    default:
                        _music_state[index].track = 0;
                        return 0;
                }
            }
            break;

            case M_DELAY_N:
                m.delay = cmd & 0x1F;
                m.track = d;
                return 0;

            case M_NOTES_OFF:
            {
                uint8_t m = 1;
                for (uint8_t i = 0; i < 4; i++)
                {
                    if (m & cmd)
                        music_note_off(i);
                    m <<= 1;
                }
            }
            break;
                
            case M_NOTES_ON:
            {
                uint8_t m = 1;
                for (uint8_t i = 0; i < 4; i++)
                {
                    if (m & cmd)
                        music_note(i,pgm_read_byte(d++));
                    m <<= 1;
                }
            }
            break;
                
            default:    // ERRORz
                m.track = d;
                return 0;
        }
    }
}

void music_kernel()
{
    // Step music track
    music_step_track(0);
    music_step_track(1);
    
    // Step patches
    for (uint8_t i = 0; i < 4; i++)
    {
        AudioPatch& p = _patches[i];
        if (!p.velocity)
            continue;
        
        const uint8_t* ep = p.envelope + p.envelope_pos;
        uint8_t e = pgm_read_byte(ep);
        uint8_t v = e == E_SUSTAIN ? pgm_read_byte(ep-1) : e;
        uint8_t vol = (v*p.velocity) >> 8;  // apply volume envelope
        uint8_t note = p.note;
        p.note += p.slide;
        uint16_t step;
        if (note >= 128)
            step = note-128;    // Direct control of low freq stepping
        else
            step = pgm_read_word(&_midi_tables[note]);
        music_on(i,vol,step);
        
        if (e == 0)
            p.velocity = 0;
        else if (e != E_SUSTAIN)
            p.envelope_pos++;
    }
}

void music_play(const uint8_t* track, uint8_t index)
{
    _music_state[index].track = track;
    _music_state[index].delay = 1;
    music_step_track(index);
}

//=====================================================================================
//=====================================================================================
//  Low level kernel

inline
void audio_step()
{
    uint8_t i = 4;
    while (i--)
        _channels[i].pos += _channels[i].step;
}

inline
void audio_mix()
{
    int16_t mix = 0;
    uint8_t i;
    
    for (i = 0; i < 4; i++)
    {
        AudioChannel* ch = _channels+i;
        int8_t v = ch->volume;
        if (!v)
            continue;
        int8_t a;
#if ARDUINO
        uint8_t flags = ch->flags;
        uint16_t* p = (uint16_t*)(&ch->flags-1);  // funny addressing hack for speed
        a = pgm_read_byte(_wave_tables + *p);
#else
        uint8_t hi = ch->pos >> 8;  // STEP
        a = (int8_t)pgm_read_byte(_wave_tables + hi + (ch->flags << 8));
#endif
        mix += a*v;
    }
    _mix = mix >> 8;   // Saturating lookup? filtering? TODO
}
 
// Simulator
void AUDIO_STEP()
{
    audio_step();
    audio_mix();
}
#endif
