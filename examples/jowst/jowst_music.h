
    // Period is 18 frames
PROGMEM
const uint8_t _joust_start[] = {
    M_WAVE, (SQUARE << 4) | CH4,	// 16*7

    M_NOTES_ON | CH4, 84, // to 107
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 56,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 48,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 40,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 32,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 24,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 16,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_ON | CH4, 84, // to 107
    M_VOLUME, CH4, 8,
    M_SLIDE, CH4, 4,	// slide up
    M_DELAY_7,

    M_NOTES_OFF | CH4,
    M_END
};

PROGMEM
const uint8_t _egg[] = {
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_WAVE, (SQUARE << 4) | CH4,
    M_NOTES_ON | CH4, 55,
    M_SLIDE, CH4, 1,
    M_DELAY_10,
    M_SLIDE, CH4, 3,
    M_DELAY_20,
    M_NOTES_OFF | CH4,
    M_END
};

PROGMEM
const uint8_t _thump[] = {
    M_WAVE, (SQUARE << 4) | CH4,
    M_NOTES_ON | CH4, 60, // ->44
    M_SLIDE, CH4, 256-6,
    M_DELAY_8,
    M_NOTES_OFF | CH4,
    M_END
};

PROGMEM
const uint8_t _buzzard[] = {
    M_WAVE, (RANDOM << 4) | CH4,
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_NOTES_ON | CH4, 2, // ->44
    M_SLIDE, CH4, 1,
    M_DELAY_4,
    M_NOTES_ON | CH4, 30,
    M_DELAY_8,
    M_SLIDE, CH4, 256-1,
    M_DELAY_12,
    M_END
};

PROGMEM
const uint8_t _emerge[] = {
    M_WAVE, (SINE << 4) | CH4,
    M_NOTES_ON | CH4, 47,
    M_DELAY_6,
    M_NOTES_ON | CH4, 69,
    M_DELAY_2,
    M_NOTES_ON | CH4, 78,
    M_DELAY_1,
    M_NOTES_ON | CH4, 92,
    M_DELAY_1,

    M_NOTES_ON | CH4, 48,
    M_DELAY_6,
    M_NOTES_ON | CH4, 70,
    M_DELAY_2,
    M_NOTES_ON | CH4, 79,
    M_DELAY_1,

    M_NOTES_ON | CH4, 49,
    M_DELAY_6,
    M_NOTES_ON | CH4, 71,
    M_DELAY_2,
    M_NOTES_ON | CH4, 81,
    M_DELAY_1,
    M_NOTES_OFF | CH4,
    M_END
};

    // 13x
PROGMEM
const uint8_t _start_2[] = {
    M_WAVE, (SQUARE << 4) | CH4,
    M_NOTES_ON | CH4, 80,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 60,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 55,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 50,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 45,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 40,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 35,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 30,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 25,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 20,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 15,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 10,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_NOTES_ON | CH4, 80,
    M_VOLUME, CH4, 5,
    M_SLIDE, CH4, 1,
    M_DELAY_8,

    M_END
};

// on channel 3
PROGMEM
const uint8_t _flap[] = {
    M_WAVE, (RANDOM << 4) | CH2,
    M_NOTES_ON | CH2, 128+15, //
    M_SLIDE, CH2, 1,
    M_VOLUME, CH2, 16,
    M_DELAY_2,
    M_VOLUME, CH2, 32,
    M_DELAY_2,
    M_VOLUME, CH2, 80,
    M_DELAY_2,
    M_VOLUME, CH2, 128,
    M_SLIDE, CH2, 256-1,
    M_DELAY_2,
    M_VOLUME, CH2, 80,
    M_DELAY_2,
    M_VOLUME, CH2, 32,
    M_DELAY_2,
    M_VOLUME, CH2, 16,
    M_DELAY_2,
    M_NOTES_OFF | CH2,
    M_END
};

PROGMEM
const uint8_t _skid[] = {
    M_WAVE, (SINE << 4) | CH4,
    M_WAVE, (TRIANGLE << 4) | CH3,
    M_NOTES_ON | CH3 | CH4, 78, 79, //
    M_DELAY_3,
    M_NOTES_ON | CH4, 78, //
    M_DELAY_3,
    M_NOTES_ON | CH4, 79, //
    M_DELAY_3,
    M_NOTES_ON | CH3, 79, //
    M_NOTES_ON | CH4, 78, //
    M_DELAY_3,
    M_NOTES_ON | CH3, 78, //
    M_DELAY_3,
    M_NOTES_ON | CH4, 79, //
    M_DELAY_3,
    M_NOTES_ON | CH3, 78, //
    M_NOTES_ON | CH4, 79, //
    M_DELAY_3,
    M_NOTES_ON | CH4, 78, //
    M_ENVELOPE, (FADE2 << 4) | CH4 | CH3,
    M_DELAY_3,
    M_SLIDE, CH3, 1,
    M_DELAY_1,
    M_SLIDE, CH4, 1,
    M_DELAY_3,
    M_NOTES_OFF | CH3 | CH4,
    M_END
};

PROGMEM
const uint8_t _step[] = {
    M_WAVE, (SQUARE << 4) | CH4,
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_NOTES_ON | CH4, 30, // ->44
    M_DELAY_2,
    M_NOTES_OFF | CH4,
    M_DELAY_4,
    M_END
};


PROGMEM
const uint8_t _joustkill[] = {
    M_WAVE, (RANDOM << 4) | CH4,
    M_WAVE, (SQUARE << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH3 | CH4,
    M_NOTES_ON | CH3 | CH4, 50, 128+32, //
    M_SLIDE, CH3 | CH4, 256-1,256-2,
    M_DELAY_20,
    M_NOTES_OFF | CH3 | CH4,
    M_END
};
