//
//  pacman_music.h
//

//===============================================================
//===============================================================

const uint8_t _pacman_intro[] PROGMEM = {
    M_WAVE, (TRIANGLE << 4) | CH1 | CH2,
    M_ENVELOPE, (FADE2 << 4) | CH1 | CH2,
    M_NOTES_ON | 0x03,0x47,0x23,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_OFF | 0x02,
    M_NOTES_ON | 0x01,0x53,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x01,0x4e,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x4b,0x2f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_OFF | 0x02,
    M_NOTES_ON | 0x03,0x53,0x23,
    M_DELAY_4,
    M_NOTES_ON | 0x01,0x4e,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_DELAY_9,
    M_NOTES_ON | 0x01,0x4b,
    M_DELAY_9,
    M_NOTES_ON | 0x02,0x2f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x48,0x24,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_OFF | 0x02,
    M_DELAY_1,
    M_NOTES_ON | 0x01,0x54,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x01,0x4f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x03,0x4c,0x30,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x54,0x24,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_1,
    M_NOTES_ON | 0x01,0x4f,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_DELAY_9,
    M_NOTES_ON | 0x01,0x4c,
    M_DELAY_8,
    M_NOTES_ON | 0x02,0x30,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_OFF | 0x02,
    M_DELAY_1,
    M_NOTES_ON | 0x03,0x47,0x23,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_OFF | 0x02,
    M_NOTES_ON | 0x01,0x53,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x01,0x4e,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x4b,0x2f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x03,0x53,0x23,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_NOTES_ON | 0x01,0x4e,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_DELAY_9,
    M_NOTES_ON | 0x01,0x4b,
    M_DELAY_9,
    M_NOTES_ON | 0x02,0x2f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x4b,0x2a,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_1,
    M_NOTES_ON | 0x01,0x4c,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_NOTES_ON | 0x01,0x4d,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_4,
    M_NOTES_ON | 0x03,0x4d,0x2c,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_1,
    M_NOTES_ON | 0x01,0x4e,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_NOTES_ON | 0x01,0x4f,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x03,0x4f,0x2e,
    M_DELAY_4,
    M_NOTES_ON | 0x01,0x50,
    M_DELAY_4,
    M_NOTES_OFF | 0x03,
    M_NOTES_ON | 0x01,0x51,
    M_DELAY_4,
    M_NOTES_OFF | 0x01,
    M_DELAY_5,
    M_NOTES_ON | 0x03,0x53,0x2f,
    M_DELAY_8,
    M_NOTES_OFF | 0x07,
    M_END,
};

const uint8_t _sound_off[] PROGMEM = {
    M_NOTES_OFF | 0x0F,
    M_END
};

// Channel 3/2 sounds
const uint8_t _siren_slow[] PROGMEM = {
    M_ENVELOPE, (NONE << 4) | CH3,
	M_WAVE, (TRIANGLE << 4) | CH3,
    M_NOTES_ON | CH3, 68, 	//
    M_VOLUME, CH3, 40,
    M_SLIDE, CH3, 1,
    M_DELAY_12,
    M_SLIDE, CH3, 256-1,
    M_DELAY_11,
    M_END
};

const uint8_t _siren_medium[] PROGMEM = {
    M_ENVELOPE, (NONE << 4) | CH3,
	M_WAVE, (TRIANGLE << 4) | CH3,
    M_NOTES_ON | CH3, 72, 	//
    M_VOLUME, CH3, 40,
    M_SLIDE, CH3, 1,
    M_DELAY_10,
    M_SLIDE, CH3, 256-1,
    M_DELAY_9,
    M_END
};

const uint8_t _siren_fast[] PROGMEM = {
    M_ENVELOPE, (NONE << 4) | CH3,
	M_WAVE, (TRIANGLE << 4) | CH3,
    M_NOTES_ON | CH3, 76, 	//
    M_VOLUME, CH3, 40,
    M_SLIDE, CH3, 1,
    M_DELAY_6,
    M_SLIDE, CH3, 256-1,
    M_DELAY_5,
    M_END
};

const uint8_t _alarm[] PROGMEM = {
    M_ENVELOPE, (NONE << 4) | CH3,
    M_WAVE, (SINE << 4) | CH3,
    M_NOTES_ON | CH3, 98, 	//
    M_VOLUME, CH3, 40,
    M_SLIDE, CH3, 256-2,
    M_DELAY_15,
    M_NOTES_OFF | CH3,
    M_END
};

const uint8_t _power[] PROGMEM = {
    M_VOLUME, CH3, 120,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_WAVE, (SQUARE << 4) | CH3,
    M_NOTES_ON | CH3, 60,
    M_SLIDE, CH3, 6,
    M_DELAY_8,
    M_NOTES_OFF | CH3,
    M_END
};

const uint8_t _got_ghost[] PROGMEM = {
    M_WAVE, (SINE << 4) | CH2,
    M_WAVE, (SAWTOOTH << 4) | CH3,
    M_ENVELOPE, (NONE << 4) | CH2 | CH3,
    M_NOTES_ON | CH2, 60,
    M_NOTES_ON | CH3, 61,
    M_SLIDE, CH2, 1,
    M_SLIDE, CH3, 1,
    M_DELAY_30,
    M_DELAY_30,
    M_NOTES_OFF | CH2 | CH3,
    M_END
};

//  Channel 4 sounds
const uint8_t _wukka[] PROGMEM = {
    M_WAVE, (SINE << 4) | CH4,
    M_NOTES_ON | CH4, 84, 	//
    M_SLIDE, CH4, 256-3,
    M_DELAY_5,
    M_NOTES_OFF | CH4,
    M_DELAY_4,
    M_NOTES_ON | CH4, 66, 	//
    M_SLIDE, CH4, 3,
    M_DELAY_5,
    M_NOTES_OFF | CH4,
    M_DELAY_4,
    M_END
};

const uint8_t _pellet[] PROGMEM = {
    M_WAVE, (SAWTOOTH << 4) | CH4,
    M_NOTES_ON | CH4, 68, 	//
    M_SLIDE, CH4, 3,
    M_DELAY_5,
    M_NOTES_OFF | CH4,
    M_END
};

const uint8_t _fruit[] PROGMEM = {
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_NOTES_ON | CH4, 78, 	//
    M_SLIDE, CH4, 256-3,
    M_DELAY_10,
    M_SLIDE, CH4, 3,
    M_DELAY_9,
    M_END
};

const uint8_t _death[] PROGMEM = {
    M_WAVE, (SINE << 4) | CH4,
    M_WAVE, (TRIANGLE << 4) | CH3,
    M_NOTES_ON | CH3, 90,
    M_NOTES_ON | CH4, 101,

    M_SLIDE, CH3 | CH4, 256-1,256-1,
    M_DELAY_7,
    M_SLIDE, CH3 | CH4, 1,1,
    M_DELAY_5,

    M_SLIDE, CH3 | CH4, 256-1,256-1,
    M_DELAY_7,
    M_SLIDE, CH3 | CH4, 1,1,
    M_DELAY_5,

    M_SLIDE, CH3 | CH4, 256-1,256-1,
    M_DELAY_7,
    M_SLIDE, CH3 | CH4, 1,1,
    M_DELAY_5,

    M_SLIDE, CH3 | CH4, 256-1,256-1,
    M_DELAY_7,
    M_SLIDE, CH3 | CH4, 1,1,
    M_DELAY_5,

    M_SLIDE, CH3 | CH4, 256-1,256-1,
    M_DELAY_7,
    M_SLIDE, CH3 | CH4, 1,1,
    M_DELAY_5,

    // repeat for long death

    // Blerp Blerp at end
    M_NOTES_OFF | CH3,
    M_NOTES_ON | CH4, 58,
    M_SLIDE, CH3 | CH4, 4, 4,
    M_DELAY_10,
    M_NOTES_ON | CH4, 58,
    M_SLIDE, CH3 | CH4, 4, 4,
    M_DELAY_10,
    M_NOTES_OFF | CH3 | CH4,
    M_END
};

const uint8_t _extra_man[] PROGMEM = {
    M_WAVE, (SINE << 4) | CH3,
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_ENVELOPE, (FADE3 << 4) | (CH3 | CH4),

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,

    M_NOTES_ON | CH3, 66,
    M_NOTES_ON | CH4, 102,
    M_DELAY_12,
    M_NOTES_OFF | CH3 | CH4,
    M_END
};

                
