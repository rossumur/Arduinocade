

// SINE
// Period is 18 frames
var _joust_start = [
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

    M_END
];

// squae or triamnge
var _joust_egg = [
	300-4000, 15
];

var _tomes = [
	M_WAVE, (TRIANGLE << 4) | CH4,	// 16*7

    M_NOTES_ON | CH4, 59,
    M_DELAY_30,

    M_NOTES_ON | CH4, 71,
    M_DELAY_30,

    M_NOTES_ON | CH4, 84,
    M_DELAY_30,

    M_NOTES_ON | CH4, 96,
    M_DELAY_30,

    M_NOTES_ON | CH4, 107,
    M_DELAY_30,

    M_NOTES_ON | CH4, 119,
    M_DELAY_30,
    M_END
];

var _coin = [
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_DELAY_1,

    M_NOTES_ON | CH4, 95,
    M_DELAY_5,

    M_NOTES_ON | CH4, 100,
    M_DELAY_8,
    M_END
];

var _jump = [
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_NOTES_ON | CH4, 74,
    M_SLIDE, CH4, 1,    // slide up
    M_DELAY_8,
    M_END
];

var _life = [
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,

    M_NOTES_ON | CH4, 75,
    M_DELAY_5,
    M_NOTES_ON | CH4, 101,
    M_DELAY_5,
    M_NOTES_ON | CH4, 96,
    M_DELAY_5,
    M_NOTES_ON | CH4, 97,
    M_DELAY_5,
    M_NOTES_ON | CH4, 103,
    M_DELAY_5,
    M_END
];

var _step = [
    M_WAVE, (RANDOM << 4) | CH4,
    M_NOTES_ON | CH4, 30, // ->44
    M_DELAY_1,
    M_NOTES_OFF | CH4,
    M_END
];

var _thump = [
    M_WAVE, (SQUARE << 4) | CH4,
    M_NOTES_ON | CH4, 60, // ->44
    M_SLIDE, CH4, 256-6,
    M_DELAY_8,
    M_NOTES_OFF | CH4,
    M_END
];

var _buzzard = [
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

];

var _lo = 56;
var _hi = 60;

var _rumble = [
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_WAVE, (SINE << 4) | CH3,
    M_NOTES_ON | CH4, _lo, 
    M_NOTES_ON | CH3, _hi+3, 
    M_DELAY_3,
    M_NOTES_ON | CH3, _hi+2, 
    M_DELAY_3,
    M_NOTES_ON | CH3, _hi+1, 
    M_DELAY_3,
    M_NOTES_ON | CH3, _hi+0, 
    M_DELAY_3,
    M_END
];

var _emerge = [
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
    M_END
];

// 13x
var _start_2 = [
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
];

var _flap = [
    M_WAVE, (RANDOM << 4) | CH3,
    M_NOTES_ON | CH3, 128+15, //
    M_SLIDE, CH3, 1,
    M_VOLUME, CH3, 16,
    M_DELAY_2,
    M_VOLUME, CH3, 32,
    M_DELAY_2,
    M_VOLUME, CH3, 80,
    M_DELAY_2,
    M_VOLUME, CH3, 128,
    M_SLIDE, CH3, 256-1,
    M_DELAY_2,
    M_VOLUME, CH3, 80,
    M_DELAY_2,
    M_VOLUME, CH3, 32,
    M_DELAY_2,
    M_VOLUME, CH3, 16,
    M_DELAY_2,
    M_NOTES_OFF | CH3,
    M_END
];

var _joustkill = [
    M_WAVE, (RANDOM << 4) | CH4,
    M_WAVE, (SQUARE << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH3 | CH4,
    M_NOTES_ON | CH3 | CH4, 50, 128+32, //
    M_SLIDE, CH3 | CH4, 256-1,256-2,
    M_DELAY_20,
    M_NOTES_OFF | CH3 | CH4,
    M_END
];

var _skid = [
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_WAVE, (RANDOM << 4) | CH3,
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
];



// Cool bullet
var bb = 200;
var _shot = [
    M_WAVE, (RANDOM << 4) | CH4 | CH3 | CH2,
    M_ENVELOPE, (FADE2 << 4) | CH4 | CH3 | CH2,
    M_NOTES_ON | CH2 |CH3 | CH4, bb+1,bb+3,bb+2,
    M_DELAY_20,
    M_END
];

bb = 80;
var _laser = [
    M_WAVE, (RANDOM << 4) | CH3,
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_NOTES_ON | CH3 | CH4, bb+2,bb,
    M_SLIDE, CH4, 256-3,
    M_DELAY_20,
    M_END
];

var _laser2 = [
    M_WAVE, (RANDOM << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_NOTES_ON | CH4, 80,
    M_SLIDE, CH4, 256-3,
    M_DELAY_3,
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH4,
    M_NOTES_ON | CH4, 80,
    M_SLIDE, CH4, 256-5,
    M_DELAY_5,
];


bb = 60;
var _smack = [
    M_WAVE, (RANDOM << 4) | CH3,
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_NOTES_ON | CH3 | CH4, bb+1,bb,
    M_SLIDE, CH4, 256-3,
    M_DELAY_20,
    M_END
];

// interference noise
bb = 90;
var _laser3 = [
    M_WAVE, (RANDOM << 4) | CH3,
    M_WAVE, (SAWTOOTH << 4) | CH4,
    M_WAVE, (SINE << 4) | CH4,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_NOTES_ON | CH3 | CH4, bb+1,bb,
    M_SLIDE, CH4, 256-2,
    M_DELAY_20,
    M_END
];
// 

bb = 50;
var _rumble = [
    M_ENVELOPE, (TREMLO << 4) | CH3 | CH4,
    M_WAVE, (TRIANGLE << 4) | CH4,
    M_WAVE, (SAWTOOTH << 4) | CH3,
    M_NOTES_ON | CH3 | CH4, bb,bb,
    M_SLIDE, CH3, 1,
    M_SLIDE, CH4, 256-1,
    M_DELAY_20,
    M_DELAY_20,
    M_END
];

var _egg = [
    M_ENVELOPE, (FADE2 << 4) | CH4,
    M_WAVE, (SQUARE << 4) | CH4,
    M_NOTES_ON | CH4, 55,
    M_SLIDE, CH4, 1,
    M_DELAY_10,
    M_SLIDE, CH4, 3,
    M_DELAY_20,
    M_NOTES_OFF | CH4,
    M_END
];


add_music("joust",[
	{  name: "joust_start",   list: [_joust_start]},
    {  name: "tomes",   list: [_tomes]},
    {  name: "thump",   list: [_thump]},
    {  name: "step",   list: [_step]},
    {  name: "buzzard",   list: [_buzzard]},
    {  name: "egg",   list: [_egg]},
    {  name: "_rumble",   list: [_rumble]},
    {  name: "_emerge",   list: [_emerge]},
    {  name: "_start_2",   list: [_start_2]},
    {  name: "_joustkill",   list: [_joustkill]},
    {  name: "_skid",   list: [_skid]},
    {  name: "flap",   list: [_flap]},
    {  name: "coin",   list: [_coin]},
    {  name: "jump",   list: [_jump]},
    {  name: "life",   list: [_life]},
    {  name: "shot",   list: [_shot]},
    {  name: "_laser",   list: [_laser]},
    {  name: "_laser2",   list: [_laser2]},
    {  name: "_laser3",   list: [_laser3]},
        {  name: "_smack",   list: [_smack]},
    {  name: "_rumble",   list: [_rumble]}
]);
