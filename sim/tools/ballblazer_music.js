

// WAVE(TRIANGLE,1,1,1,0)
// ENV(KEYBOARD,0,0,0,1)
// ON(CH0|CH1,0x20,0x2C);

// sled womp and fire are mirrors

// Theme, 6.4 second loop
// Saw for 0-2, HiHat on 3
var PREPLAY_OFFSET = 123;

var _theme_init = [
    M_WAVE,     (SAWTOOTH << 4) | 1 | 2 | 4,
    M_WAVE,     (RANDOM << 4) | 8,
    M_ENVELOPE, (KEYBOARD << 4) | 1 | 2 | 4,
    M_ENVELOPE, (HIHAT << 4) | 8,
    M_END
];
var _theme =[
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x28,0x34,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x2f,0x3b,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x25,0x31,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x23,0x2f,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x36,0x3b,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x20,0x2c,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_15,
    
    // preplay cue
    M_NOTES_ON | 0x03,0x27,0x33,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x0b,0x28,0x34,0x28,
    M_DELAY_9,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x03,0x2f,0x3b,
    M_DELAY_8,
    M_NOTES_OFF | 0x08,
    M_DELAY_1,
    M_NOTES_OFF | 0x03,
    M_DELAY_3,
    M_NOTES_ON | 0x07,0x25,0x38,0x41,
    M_DELAY_20,
    M_NOTES_OFF | 0x07,
    M_DELAY_4,
    M_NOTES_ON | 0x0f,0x23,0x36,0x3f,0x28,
    M_DELAY_20,
    M_NOTES_OFF | 0x0f,
    M_DELAY_4,
    M_NOTES_ON | 0x07,0x1c,0x38,0x40,
    M_DELAY_4,
    M_NOTES_OFF | 0x07,
    M_DELAY_2,
    M_NOTES_ON | 0x07,0x1c,0x36,0x3e,
    M_DELAY_4,
    M_NOTES_OFF | 0x07,
    M_DELAY_14,
    M_NOTES_ON | 0x0f,0x1c,0x38,0x3b,0x28,
    M_DELAY_20,
    M_NOTES_OFF | 0x0f,
    M_DELAY_4,
    M_END
];

// Music and sounds
// TADA theme on goal
var _tada = [
    M_WAVE,     (TRIANGLE << 4) | (1 | 2 | 4),
    M_ENVELOPE, (NONE << 4) | (1 | 2 | 4),
    
    M_NOTES_ON | 1 | 2 | 4, 69,76,81,
    M_DELAY_8,
    M_NOTES_ON | 1 | 2 | 4, 71,78,83,
    M_DELAY_16,
    
    M_NOTES_ON | 1 | 2 | 4, 69,76,81,
    M_DELAY_8,
    M_NOTES_ON | 1 | 2 | 4, 71,78,83,
    M_DELAY_16,
    
    M_SLIDE, 4, 0xFF,// (uint8_t)-1,
    M_DELAY, 36,
    M_SLIDE, 4, 0, // slide channel 2 83->47
    M_DELAY_4,
    
    M_NOTES_ON | 1, 63,
    M_DELAY, 40,
    
    M_NOTES_OFF | 1 | 2 | 4,
    M_DELAY, 30,   // beat
    M_END
];

var _win = [
    M_WAVE, (SQUARE << 4)  | CH2,
    M_NOTES_ON | CH2, 96,
    M_SLIDE, CH2, 1,
    M_DELAY_3,
    M_WAVE, (RANDOM << 4)  | CH2,
    M_NOTES_ON | CH2, 150,
    // missing delay
];


var _effect_init = [
    M_NOTES_OFF | 0xF,	// All off TODO

    M_WAVE,     (SQUARE << 4)   | 1,    // Ball/Field buzzing
    M_ENVELOPE, (NONE << 4)     | 1,
    M_NOTES_ON | 1, 36,                 // start buzzing on channel 0 (~66hz)
    M_VOLUME, 1, 0,
    
    M_WAVE,     (RANDOM << 4)   | CH2,     // Ball firing
    M_ENVELOPE, (FADE2 << 4)    | CH2,
    
    M_WAVE,     (SQUARE << 4)   | CH3,     // Sled womp/bounce noise
    M_ENVELOPE, (HIHAT << 4)    | CH3,
    
    M_WAVE,     (RANDOM << 4)   | CH4,     // Tat-tat-tat sound
    M_ENVELOPE, (FADE << 4)     | CH4,
    
    M_END
];

var _ball_push = [
    M_WAVE, (SAWTOOTH << 4)  | CH4,
    M_NOTES_ON | CH4, 70,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_SLIDE, CH4, 256-6,
    M_END
];

var _sled_womp = [
    M_NOTES_ON | 4, 20,      // channel 3
    M_NOTES_ON | 8, 120,     // channel 4
    M_SLIDE, 4, 10,
    M_END
];

var _sled_womp2 = [
    M_ENVELOPE, (FADE3 << 4) | CH4,
    M_WAVE, (SAWTOOTH << 4)  | CH4,
    M_NOTES_ON | CH4, 40,    // channel 3
    M_SLIDE, CH4, 6,
    M_END
];

var _tat = [
    M_NOTES_ON | 8, 120,      // channel 4
    M_END
];

var _bonk = [
    M_WAVE, (SAWTOOTH << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH3,
    M_NOTES_ON | CH3, 32,
    M_DELAY_12,
    M_END
];


var _bounce = [
    M_NOTES_ON | CH3, 0,      // channel 3 low freq
    M_VOLUME, CH3, 0xFF,
    M_SLIDE, CH3, 10,
    M_END
];

var _tick = [
    M_NOTES_ON | 4, 77,      // channel 3
    M_END
];

var _tock = [
    M_NOTES_ON | 4, 89,      // channel 3
    M_END
];

var _delay5 = [
    M_DELAY_5,
    M_END
];

var _delay10 = [
    M_DELAY_10,
    M_END
];

// Caverns
var s_fire = [
    M_NOTES_ON | CH2, 80,
    M_SLIDE, 2, 256-10,
    M_END
];

var  s_explode = [
    M_NOTES_ON | CH4, 0,
    M_SLIDE, 2, 1,
    M_END
];


add_music("ballblazer",[
	{ name: "theme", list: [_theme_init,_theme] },
	{ name: "theme intro", list: [_theme_init,{data:_theme,offset:123}]},
    { name: "tada", list: [_tada]},
	{ name: "win", list: [_win,_win,_win,_win,_win,_win,_win,_win,_win,_win,_win,_win,_win,_win,_win],},
	{ name: "ball_push", list: [_effect_init,_ball_push,_delay5]},
    { name: "sled_womp", list: [_effect_init,_sled_womp,_delay5]},
    { name: "sled_womp2", list: [_effect_init,_sled_womp2,_delay10]},
	{ name: "tat", list: [_effect_init,_tat,_delay5]},
    { name: "bounce", list: [_effect_init,_bounce,_delay5]},
    { name: "bonk", list: [_effect_init,_bonk,_delay10]},
	{ name: "tick", list: [_effect_init,_tick,_delay5]},
	{ name: "tock", list: [_effect_init,_tock,_delay5]},
	{ name: "s_fire", list: [_effect_init,s_fire,_delay5]},
	{ name: "s_explode", list: [_effect_init,s_explode,_delay5]},
]);
