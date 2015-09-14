
#ifndef __MUSIC_H__
#define __MUSIC_H__

//=====================================================================
//=====================================================================
// music

int8_t music_step_track(uint8_t index);

void music_play(const uint8_t* track, uint8_t index = 0);
bool music_playing(uint8_t index = 0);
void music_kernel();

void music_note(uint8_t channel, uint8_t note, uint8_t velocity = 63);
void music_off(uint8_t channel);
void music_volume(uint8_t channel, uint8_t volume);

//  low level kernel
void audio_init();
inline void audio_step();
inline void audio_mix();

extern uint8_t _mix;    // output from last audio mix

typedef struct {
    uint16_t step;
    uint16_t pos;  // (h << 8) | flags is table offset
    uint8_t flags;
    uint8_t volume;
} AudioChannel;

extern
AudioChannel _channels[4];

#define CH1 1
#define CH2 2
#define CH3 4
#define CH4 8

// Wave
enum {
    SINE = 0,
    SAWTOOTH = 1,
    TRIANGLE = 2,
    SQUARE = 3,
    RANDOM = 4,
};

// envelope
enum {
    NONE,
    KEYBOARD,
    HIHAT,
    FADE,
    FADE2,
    HIT,
    FADE3
};

//  Music
enum {
    M_END = 0,
    M_DELAY = 1,
    M_SLIDE = 2,
    M_FLAG = 3,
    M_VOLUME = 4,
    M_WAVE = 5,
    M_ENVELOPE = 6,
    
    M_VELOCITY_N = 0x80,
    M_NOTES_ON = 0xA0,
    M_NOTES_OFF = 0xC0,
    
    M_DELAY_N = 0xE0,   // 0..31 in single byte
    M_DELAY_1,
    M_DELAY_2,
    M_DELAY_3,
    M_DELAY_4,
    M_DELAY_5,
    M_DELAY_6,
    M_DELAY_7,
    M_DELAY_8,
    M_DELAY_9,
    M_DELAY_10,
    M_DELAY_11,
    M_DELAY_12,
    M_DELAY_13,
    M_DELAY_14,
    M_DELAY_15,
    M_DELAY_16,
    M_DELAY_17,
    M_DELAY_18,
    M_DELAY_19,
    M_DELAY_20,
    M_DELAY_21,
    M_DELAY_22,
    M_DELAY_23,
    M_DELAY_24,
    M_DELAY_25,
    M_DELAY_26,
    M_DELAY_27,
    M_DELAY_28,
    M_DELAY_29,
    M_DELAY_30,
    
    M_BAD  // 0xFF
};

#endif
