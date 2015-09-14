//===============================================================
//===============================================================

function MIDI()
{
    var _programs = [
        "Acoustic Grand Piano",     /* 000 */     /* Pianos */
        "Bright Acoustic Piano",    /* 001 */
        "Electric grand Piano",     /* 002 */
        "Honky Tonk Piano",         /* 003 */
        "Electric Piano 1",         /* 004 */
        "Electric Piano 2",         /* 005 */
        "Harpsichord",              /* 006 */
        "Clavinet",                 /* 007 */
        "Celesta",                  /* 008 */     /* Chromatic Percussion */
        "Glockenspiel",             /* 009 */
        "Music Box",                /* 010 */
        "Vibraphone",               /* 011 */
        "Marimba",                  /* 012 */
        "Xylophone",                /* 013 */
        "Tubular bells",            /* 014 */
        "Dulcimer",                 /* 015 */
        "Drawbar Organ",            /* 016 */     /* Organs */
        "Percussive Organ",         /* 017 */
        "Rock Organ",               /* 018 */
        "Church Organ",             /* 019 */
        "Reed Organ",               /* 020 */
        "Accordion",                /* 021 */
        "Harmonica",                /* 022 */
        "Tango Accordion",          /* 023 */
        "Nylo n Accustic Guitar",   /* 024 */     /* Guitars */
        "Steel Acoustic Guitar",    /* 025 */
        "Jazz Electric Guitar",     /* 026 */
        "Clean Electric Guitar",    /* 027 */
        "Muted Electric Guitar",    /* 028 */
        "Overdrive Guitar",         /* 029 */
        "Distorted Guitar",         /* 030 */
        "Guitar Harmonics",         /* 031 */
        "Acoustic Bass",            /* 032 */     /* Basses */
        "Electric Fingered Bass",   /* 033 */
        "Electric Picked Bass",     /* 034 */
        "Fretless Bass",            /* 035 */
        "Slap Bass 1",              /* 036 */
        "Slap Bass 2",              /* 037 */
        "Syn Bass 1",               /* 038 */
        "Syn Bass 2",               /* 039 */
        "Violin",                   /* 040 */     /* Strings/Orchestra */
        "Viola",                    /* 041 */
        "Cello",                    /* 042 */
        "Contrabass",               /* 043 */
        "Tremolo Strings",          /* 044 */
        "Pizzicato Strings",        /* 045 */
        "Orchestral Harp",          /* 046 */
        "Timpani",                  /* 047 */
        "String Ensemble 1",        /* 048 */     /* Ensembles */
        "String Ensemble 2 (Slow)", /* 049 */
        "Syn Strings 1",            /* 050 */
        "Syn Strings 2",            /* 051 */
        "Choir Aahs",               /* 052 */
        "Voice Oohs",               /* 053 */
        "Syn Choir",                /* 054 */
        "Orchestral Hit",           /* 055 */
        "Trumpet",                  /* 056 */     /* Brass */
        "Trombone",                 /* 057 */
        "Tuba",                     /* 058 */
        "Muted Trumpet",            /* 059 */
        "French Horn",              /* 060 */
        "Brass Section",            /* 061 */
        "Syn Brass 1",              /* 062 */
        "Syn Brass 2",              /* 063 */
        "Soprano Sax",              /* 064 */     /* Reeds */
        "Alto Sax",                 /* 065 */
        "Tenor Sax",                /* 066 */
        "Baritone Sax",             /* 067 */
        "Oboe",                     /* 068 */
        "English Horn",             /* 069 */
        "Bassoon",                  /* 070 */
        "Clarinet",                 /* 071 */
        "Piccolo",                  /* 072 */     /* Pipes */
        "Flute",                    /* 073 */
        "Recorder",                 /* 074 */
        "Pan Flute",                /* 075 */
        "Bottle Blow",              /* 076 */
        "Shakuhachi",               /* 077 */
        "Whistle",                  /* 078 */
        "Ocarina",                  /* 079 */
        "Syn Square Wave",          /* 080 */     /* Synth Leads */
        "Syn Sawtooth Wave",        /* 081 */
        "Syn Calliope",             /* 082 */
        "Syn Chiff",                /* 083 */
        "Syn Charang",              /* 084 */
        "Syn Voice",                /* 085 */
        "Syn Fifths Sawtooth Wave", /* 086 */
        "Syn Brass & Lead",         /* 087 */
        "New Age Syn Pad",          /* 088 */     /* Synth Pads */
        "Warm Syn Pad",             /* 089 */
        "Polysynth Syn Pad",        /* 090 */
        "Choir Syn Pad",            /* 091 */
        "Bowed Syn Pad",            /* 092 */
        "Metal Syn Pad",            /* 093 */
        "Halo Syn Pad",             /* 094 */
        "Sweep Syn Pad",            /* 095 */
        "SFX Rain",                 /* 096 */     /* Synth Effects */
        "SFX Soundtrack",           /* 097 */
        "SFX Crystal",              /* 098 */
        "SFX Atmosphere",           /* 099 */
        "SFX Brightness",           /* 100 */
        "SFX Goblins",              /* 101 */
        "SFX Echoes",               /* 102 */
        "SFX Sci-fi",               /* 103 */
        "Sitar",                    /* 104 */     /* Ethnic */
        "Banjo",                    /* 105 */
        "Shamisen",                 /* 106 */
        "Koto",                     /* 107 */
        "Kalimba",                  /* 108 */
        "Bag Pipe",                 /* 109 */
        "Fiddle",                   /* 110 */
        "Shanai",                   /* 111 */
        "Tinkle Bell",              /* 112 */     /* Percussive */
        "Agogo",                    /* 113 */
        "Steel Drums",              /* 114 */
        "Woodblock",                /* 115 */
        "Taiko Drum",               /* 116 */
        "Melodic Tom",              /* 117 */
        "Syn Drum",                 /* 118 */
        "Reverse Cymbal",           /* 119 */
        "Guitar Fret Noise",        /* 120 */     /* Sound Effects */
        "Breath Noise",             /* 121 */
        "Seashore",                 /* 122 */
        "Bird Tweet",               /* 123 */
        "Telephone Ring",           /* 124 */
        "Helicopter",               /* 125 */
        "Applause",                 /* 126 */
        "Gun Shot"                  /* 127 */
    ];

    function HEX(n)
    {
        var x = n.toString(16);
        if (x.length == 1)
            return "0x0"+x;
        return "0x"+x;
    }

    function H(n)
    {
        var x = n.toString(16);
        if (x.length == 1)
            return "0"+x;
        return x;
    }
    function toHex(d)
    {
        var s = '';
        for (var i in d)
            s += H(d[i]);
        return s;
    }
    // Parse the midi file

    function u32(d,i)
    {
        return (d[i] << 24) | (d[i+1] << 16) | (d[i+2] << 8) | d[i+3];
    }

    function u16(d,i)
    {
        return  (d[i] << 8) | d[i+1];
    }

    function print(e)
    {
        if (e.data) {
            var str = "";
            for (var i = 0; i < e.data.length; i++)
                str += String.fromCharCode(e.data[i]);
            var name = e.meta;
            switch (name) {
                case 1: name = "Text";          break;
                case 2: name = "Copyright";     break;
                case 3: name = "Name";          break;
                case 4: name = "Instrument";    break;
                case 5: name = "Lyrics";        break;
                case 6: name = "Marker";        break;
                case 7: name = "CuePoint";      break;
                case 9: name = "DeviceName";    break;

                case 0x20:
                    name = "ChannelPrefix";
                    str = e.data[0];
                    break;
                case 0x21:
                    name = "MidiPort";
                    str = e.data[0];
                    break;
                case 0x2F: 
                    name = "EndOfTrack";
                    break;
                case 0x51:
                    name = "SetTempo";
                    str = (e.data[0] << 16) | (e.data[1] << 8) | e.data[2]; // Tempo is in microseconds per beat
                    break;
                 case 0x59:
                    name = "SMPTEOffset";
                    str = toHex(e.data);
                    break;
                case 0x59:
                    name = "KeySignature";
                    str = (e.data[0] << 8) | e.data[1];
                    break;
                case 0x7F:
                    name = "SequencerSpecific";
                    str = toHex(e.data);
                    break;
            }
            return "META " + e.track + " " + name + " " + str;
        } else {
            var t = e.evt >> 4;
            var name = t;
            switch (name) {
                case 0x8: name = "NoteOff"; break;
                case 0x9: name = "NoteOn "; break;
                case 0xA: name = "NoteAftertouch"; break;
                case 0xB: name = "Controller"; break;
                case 0xC: 
                    name = "ProgramChange ";
                    name += _programs[e.p0];
                    break;
                case 0xD: name = "ChannelAftertouch"; break;
                case 0xE: name = "PitchBend"; break;
                case 0xF: name = "System"; break;
            }
            if (t == 0xB) {
                switch (e.p0) {
                    case 0x7: name += "-Volume";    break;
                    case 0x8: name += "-Balance";   break;
                    case 0xA: name += "-Pan";       break;

                    case 0x5B: name += "-Effects1Depth"; break;
                    case 0x5C: name += "-Effects2Depth"; break;
                    case 0x5D: name += "-Effects3Depth"; break;
                    case 0x5E: name += "-Effects4Depth"; break;
                    case 0x5F: name += "-Effects5Depth"; break;
                }
            }
            return e.track + " " + e.time + ":" + name + " " + H(e.p0) + ":" + e.p1;
        }
    }

    function emitEvents(events,tempo)
    {
        var slots = [0,0,0,0];
        var velocities = [0,0,0,0];
        var channel2program = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
        var lru = [0,0,0,0];
        var prog = [-1,-1,-1,-1];
        var last_velocity = 110;
        var str = '';

        var notes_off = [];
        var notes_on = [];
        var prog_change = [];

        function MASK(n)
        {
            return HEX(n);
        }

        function flusht()
        {
            if (t < last_t)
                throw "time is running backwards";

            if (t != last_t)
            {
                if (prog_change.length) {
                    var mask = 0;
                    for (var i in prog_change) {
                        var s = parseInt(i);
                        mask |= 1 << s;
                    }
                    for (var i in prog_change)
                        str += "// " + i + " to " + prog_change[i] + " - " + _programs[prog_change[i]] + "\n";
                    str += "M_PROGRAM," + MASK(mask) + ",";
                    for (var i in prog_change)
                        str += HEX(prog_change[i]) + ",";
                    str += "\n";
                    prog_change = [];
                }

                if (notes_on.length) {
                    var mask = 0;

                    // update velocities
                    for (var i in notes_on)
                    {
                        if (velocities[i] != notes_on[i].velocity)
                        {
                            velocities[i] = notes_on[i].velocity;
                            mask |= 1 << parseInt(i);
                        }
                    }
                    if (mask) {
                        str += "M_VELOCITY_N | " + MASK(mask) + ",";
                        for (var i = 0; i < 4; i++)
                            if (mask & (1 << i))
                                str += HEX(velocities[i]) + ",";
                        str += "\n";
                    }

                    mask = 0;
                    for (var i in notes_on) {
                        var s = parseInt(i);
                        mask |= 1 << s;
                        delete notes_off[s];    // would be redundant**
                    }

                    str += "M_NOTES_ON | " + MASK(mask) + ",";
                    for (var i in notes_on)
                        str += HEX(notes_on[i].note) + ",";
                    str += "\n";
                    notes_on = [];
                }

                if (notes_off.length) {
                    var mask = 0;
                    for (var i in notes_off)
                        mask |= 1 << parseInt(i);
                    if (mask)
                        str += "M_NOTES_OFF | " + MASK(mask) + ",\n";
                    notes_off = [];
                }

                var n = t-last_t;
                if (n < 31)
                    str += "M_DELAY_" + n + ",\n";
                else
                    str += "M_DELAY," + n + ",\n";
                last_t = t;
            }
        }

        function noteOff(track,channel,note,velocity)
        {
            var s = channel+1;
            for (var i = 0; i < 4; i++)
            {
                if (slots[i] == s)
                {
                    slots[i] = 0;
                    flusht();
                    notes_off[i] = 1;
                }
            }
        }

        var _counter = 0;
        function findSlot(s)
        {
            // TODO
            var prog = channel2program[s - 1];
            var prog_i = -1;

            var min_counter = _counter;
            var min_i = -1;

            for (var i = 0; i < 4; i++)
            {
                if (slots[i] == s || !slots[i])
                {
                    lru[i] = _counter++;
                    return i;
                }
                if (lru[i] < min_counter)
                {
                    min_counter = lru[i];
                    min_i = i;
                }
                if (prog[i] == prog)
                    prog_i = i;
            }
            // optional program match?
            if (prog_i != -1) {
                str += "// matching program " + (s-1) + " into " + min_i + ",\n";
                return prog_i;
            }
            str += "// forcing " + (s-1) + " into " + min_i + ",\n";
            lru[min_i] = _counter++;
            return min_i;
        }

        function noteOn(track,channel,note,velocity)
        {
            if (velocity == 0)
            {
                noteOff(track,channel,note,velocity);
                return;
            }

            var s = channel+1;
            var i = findSlot(s,true);
            if (i != -1)
            {
                slots[i] = s;
                flusht();
                notes_on[i] = {note:note,velocity:velocity}
                if (channel2program[channel] != prog[i]) {
                    prog_change[i] = prog[i] = channel2program[channel];
                }
            } else {
                var e = "failed to find slot for " + track + ":" + channel;
                throw e;
            }
        }

        function program(track,channel,p)
        {
            channel2program[channel] = p;
        }

        var _usPerBeat= 500000;  // Microseconds per BEAT (500000 is 120bpm)
        var _base = 0;
        var _base_us = 0;
        function updateTime(new_usPerBeat, base)
        {
            _base_us = toMicroseconds(base);
            _base = base;
            _usPerBeat = (e.data[0] << 16) | (e.data[1] << 8) | e.data[2];
            console.log("tempo: " + tempo + " usPerBeat:" + _usPerBeat);
        }

        function toMicroseconds(t)
        {
           return _base_us + (t-_base)*_usPerBeat/tempo;              // convert to absolute
        }

        // scan events, map to 4 voices
        var last_t = 0;
        var t;
        for (var i in events)
        {
            var list = events[i];
            t = Math.floor(toMicroseconds(list[0].time)*60/1000000+0.5);   // to 60hz ticks

            //console.log(t);
            for (var j in list) {
                var e = list[j];
                e.t60 = t;
                if (e.data) {
                    // META
                    switch (e.meta) {
                        case 47:
                            flusht();
                            //str += "META_TRACK_END,\n";
                            break;
                        case 81:
                         // Microseconds per BEAT (500000 is 120bpm)
                            updateTime((e.data[0] << 16) | (e.data[1] << 8) | e.data[2],list[0].time);
                            break;
                    }
                } else {
                    // EVENT
                    var cmd = e.evt >> 4;
                    var channel = e.evt & 0xF;
                    switch (cmd) {
                        case 0x08: noteOff(e.track,channel,e.p0,e.p1);  break;
                        case 0x09: noteOn(e.track,channel,e.p0,e.p1);   break;
                        case 0x0C: program(e.track,channel,e.p0);       break;
                        /*
                        case 0x0A: console.log("Polyphonic aftertouch"); break;
                        case 0x0B: console.log("Control change");       break;
                        case 0x0C: console.log("Program change");       break;
                        case 0x0D: console.log("Channel aftertouch");   break;
                        case 0x0E: console.log("Pitch Wheel");          break;
                        case 0x0F: console.log("SYSTEM?");              break;
                        */
                    }
                }
            }
        }

        //console.log(str);
        events.str = str;
        return events;
    }

    function parseTrack(d,i,end)
    {
        var track = [];
        var t = 0;
        var running = 0;
        while (i < end)
        {
            var n;
            var v = 0;  // delta time
            do {
                n = d[i++];
                v = (v << 7) | (n & 0x7F);
            } while (n & 0x80);
            t += v;

            var e = {};
            e.time = t;
            if (d[i] == 0xFF) { // Meta EVENT
                i++;
                e.meta = d[i++];
                v = 0;
                do {
                    n = d[i++];
                    v = (v << 7) | (n & 0x7F);
                } while (n & 0x80);
                e.data = [];
                while (v--)
                    e.data.push(d[i++]);
            } else {
                if ((d[i] >> 4) < 8)
                    e.evt = running;
                else
                    running = e.evt = d[i++];  // Type/Channel event
                var evt = e.evt >> 4;
                switch (evt) {
                    case 0x08:
                    case 0x09:
                    case 0x0A:
                    case 0x0B:
                    case 0x0E:
                        e.p0 = d[i++];
                        e.p1 = d[i++];
                        break;
                    case 0x0C:
                    case 0x0D:
                        e.p0 = d[i++];
                        e.p1 = 0;
                        break;
                    case 0x0F:
                        {
                            switch (e.evt & 0xF) {
                                case 0x00:
                                    console.log("system exclusive:" + evt);
                                    var b;
                                    while ((b =d[i++]) != 0xF7) // end of exclusive
                                        console.log(">" + b);
                                    continue;
                                default:
                                    throw "bad system exclusive";
                            }
                        }
                    default:
                        // running status?
                }
            }
            //print(e);
            track.push(e);
        }
        return track;
    }

    function parse(s)
    {
        var d = new Uint8Array(s);
        if (u32(d,0) != 0x4D546864 || u32(d,4) != 6)
        {
            console.log("not a midi file");
            return -1;
        }
        var formatType = u16(d,8);
        var tracks = u16(d,10);
        var tempo = u16(d,12);
        var i = 8+6;
        console.log("tracks: " + tracks + ", tempo: " + tempo);
        var trackIndex = 0;
        var events = [];
        while (i < d.length)
        {
            var sig = u32(d,i);
            var len = u32(d,i+4);
            if (sig == 0x4D54726B)     // TRACK
            {
                var track = parseTrack(d,i+8,i+8+len);
                for (var j = 0; j < track.length; j++)
                {
                    var e = track[j];
                    var t = e.time;
                    e.track = trackIndex;
                    if (!events[t])
                        events[t] = [];
                    events[t].push(e);
                }
                trackIndex++;
            } else
                console.log(HEX(sig)+":"+HEX(len));
            i += len+8;
        }
        return emitEvents(events,tempo);
    }

    function toC(name,data,cols) {
        var s = "PROGMEM\nconst uint8_t " + name + "[] = {\n";
        for (var i = 0; i < data.length; i++)
        {
            var n = data[i];
            s += HEX(n) + ",";
            if (((i+1) % cols) == 0)
                s += "\n";
        }
        s += "};\n";
        return s;
    }

    return {
        parse: function(d)
        {
            return parse(d);
        },
        program : function(i)
        {
            return _programs[i];
        },
        format: function(e)
        {
            return print(e);
        }
    }
}

var _midi = new MIDI();
