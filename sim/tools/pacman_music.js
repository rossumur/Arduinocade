

var _pacman_intro = [
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
    M_END,
];


//SINE
// Period is 18 frames
var _wukka = [
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
]

var _siren_slow_start = [
	M_WAVE, (TRIANGLE << 4) | CH4,
	M_NOTES_ON | CH4, 68, 	//
	M_END 
];

var _siren_slow = [
	M_SLIDE, CH4, 1,
	M_DELAY_12,
	M_SLIDE, CH4, 256-1,
	M_DELAY_11,
	M_END
];

var _siren_medium_start = [
	M_WAVE, (TRIANGLE << 4) | CH4,
	M_NOTES_ON | CH4, 72, 	//
	M_END 
];

var _siren_medium = [
	M_SLIDE, CH4, 1,
	M_DELAY_10,
	M_SLIDE, CH4, 256-1,
	M_DELAY_9,
	M_END
];

var _siren_fast_start = [
	M_WAVE, (TRIANGLE << 4) | CH4,
	M_NOTES_ON | CH4, 76, 	//
	M_END 
];

var _siren_fast = [
	M_SLIDE, CH4, 1,
	M_DELAY_6,
	M_SLIDE, CH4, 256-1,
	M_DELAY_5,
	M_END
];

var _alarm_start = [
	M_WAVE, (SINE << 4) | CH4, //99-64
	M_NOTES_ON | CH4, 98, 	//
	M_END
];

var _alarm = [
	M_SLIDE, CH4, 256-2,
	M_DELAY_15,
	M_SLIDE, CH4, 30,
	M_END
];

var _pellet = [
	M_WAVE, (SAWTOOTH << 4) | CH4,
	M_NOTES_ON | CH4, 68, 	//
	M_SLIDE, CH4, 3,
	M_DELAY_5,
	M_END
];

var _fruit = [
	M_WAVE, (TRIANGLE << 4) | CH4,
	M_NOTES_ON | CH4, 78, 	//
	M_SLIDE, CH4, 256-3,
	M_DELAY_10,
	M_SLIDE, CH4, 3,
	M_DELAY_9,
	M_END
];


// up/down
var _death = [
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

	M_END
];

var _extra_man = [
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

    M_END
]

var _power = [
    M_WAVE, (SQUARE << 4) | CH3,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_NOTES_ON | CH3, 60,
    M_SLIDE, CH3, 6,
    M_DELAY_8,
];

var _getghost = [
    M_WAVE, (SAWTOOTH << 4) | CH3 | CH2,
    M_ENVELOPE, (NONE << 4) | CH2,
    M_ENVELOPE, (FADE << 4) | CH3,
    M_NOTES_ON | CH2, 60,
    M_SLIDE, CH2, 1,
    M_NOTES_ON | CH3, 40,
    M_SLIDE, CH3, 7,
    M_DELAY_7,
    M_NOTES_ON | CH3, 40,
    M_SLIDE, CH3, 7,
    M_DELAY_7,
    M_NOTES_ON | CH3, 40,
    M_SLIDE, CH3, 7,
    M_DELAY_7,
    M_NOTES_ON | CH3, 40,
    M_SLIDE, CH3, 7,
    M_DELAY_7,
];

var _getghost2 = [
    M_WAVE, (SINE << 4) | CH2,
    M_WAVE, (SAWTOOTH << 4) | CH3,
    M_ENVELOPE, (NONE << 4) | CH2 | CH3,

    M_NOTES_ON | CH2, 60,
    M_NOTES_ON | CH3, 61,
    M_SLIDE, CH2, 1,
    M_SLIDE, CH3, 1,
    M_DELAY,30,
    M_END
];


add_music("pacman",[
	{  name: "_pacman_intro",         list: [_pacman_intro]},
	{  name: "_wukka",         list: [_wukka,_wukka,_wukka,_wukka]},
	{  name: "_siren_slow",    list: [_siren_slow_start,_siren_slow,_siren_slow,_siren_slow,_siren_slow]},
	{  name: "_siren_medium",  list: [_siren_medium_start,_siren_medium,_siren_medium,_siren_medium]},
	{  name: "_siren_fast",    list: [_siren_fast_start,_siren_fast,_siren_fast,_siren_fast,_siren_fast]},
	{  name: "_death",         list: [_death]},
	{  name: "_alarm",         list: [_alarm_start,_alarm,_alarm,_alarm,_alarm]},
    {  name: "_pellet",        list: [_pellet]},
	{  name: "_fruit",         list: [_fruit]},
    {  name: "_power",         list: [_power,_power,_power,_power,_power,_power,_power,_power]}, // quak on blue ghosts
    {  name: "_getghost",      list: [_getghost2]},
	{  name: "_extra_man",     list: [_extra_man]}
]);
