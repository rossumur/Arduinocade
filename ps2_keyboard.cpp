
/* Copyright (c) 2010-2014, Peter Barrett
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

#include <Arduino.h>

//==========================================================
//==========================================================
//  PS2 keyboard support

#define PS2_DATA 2   // pin 2 port D
#define PS2_CLOCK 3   // pin 3 port D

#define PS2_ESCAPE 1
#define PS2_KEYUP  2
#define PS2_SHIFT  4
#define PS2_CAPS  8

// 4 bytes of state for ps2_keyboard
uint8_t ps2_key = 0;
uint8_t ps2_flags = 0;
uint16_t ps2_code = 0;

PROGMEM const uint8_t ps2_map[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x60,0x00,
    0x00,0x00,0x00,0x00,0x00,0x71,0x31,0x00,0x00,0x00,0x7A,0x73,0x61,0x77,0x32,0x00,
    0x00,0x63,0x78,0x64,0x65,0x34,0x33,0x00,0x00,0x20,0x76,0x66,0x74,0x72,0x35,0x00,
    0x00,0x6E,0x62,0x68,0x67,0x79,0x36,0x00,0x00,0x00,0x6D,0x6A,0x75,0x37,0x38,0x00,
    0x00,0x2C,0x6B,0x69,0x6F,0x30,0x39,0x00,0x00,0x2E,0x2F,0x6C,0x3B,0x70,0x2D,0x00,
    0x00,0x00,0x27,0x00,0x5B,0x3D,0x00,0x00,0x00,0x00,0x0A,0x5D,0x00,0x5C,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
    // ps2_keys shifted
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x7E,0x00,
    0x00,0x00,0x00,0x00,0x00,0x51,0x21,0x00,0x00,0x00,0x5A,0x53,0x41,0x57,0x40,0x00,
    0x00,0x43,0x58,0x44,0x45,0x24,0x23,0x00,0x00,0x20,0x56,0x46,0x54,0x52,0x25,0x00,
    0x00,0x4E,0x42,0x48,0x47,0x59,0x5E,0x00,0x00,0x00,0x4D,0x4A,0x55,0x26,0x2A,0x00,
    0x00,0x3C,0x4B,0x49,0x4F,0x29,0x28,0x00,0x00,0x3E,0x3F,0x4C,0x3A,0x50,0x5F,0x00,
    0x00,0x00,0x22,0x00,0x7B,0x2B,0x00,0x00,0x00,0x00,0x0A,0x7D,0x00,0x7C,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
};

// read ascii keydown value from ps2 keyboard or 0 if no key ready
uint8_t ps2_read_key()
{
    if (!ps2_key)
        return 0;
    uint8_t a = ps2_key;    
    if (ps2_flags & PS2_ESCAPE)
    {
        switch (a) {
            case 0x6B:  a = 0x1E; break;  // left
            case 0x72:  a = 0x1D; break;  // down
            case 0x74:  a = 0x1F; break;  // right
            case 0x75:  a = 0x1C; break;  // up
            default: a = 0;
        }
    }
    else
    {
        if (a < 0x68)
        {
            if (((ps2_flags >> 2) ^ (ps2_flags >> 3)) & 1)
                a += 0x68;
            a = pgm_read_byte(ps2_map + a);
        } else
            a = 0;
    }
    ps2_key = 0;
    ps2_flags &= 0xFC;
    return a;
}

ISR(INT1_vect)
{
    ps2_code = (ps2_code << 1) | ((PIND & 4) >> 2);
    
    if (ps2_code & 0x800) // 11 bits 0:DATA_LSB:P:1
    {
        uint8_t a,r = ps2_code >> 2;
        uint8_t p = 0;
        uint8_t b = 8;
        while (b--) {
            p += r & 1;
            a = (a << 1) | (r & 1);
            r >>= 1;
        }
        p ^= ps2_code >> 1;  // reverse bits, check parity
        
        if ((p&1) && ((ps2_code & 0x401) == 1))
        {
            if (a == 0xE0)
                ps2_flags |= PS2_ESCAPE;
            else if (a == 0xF0)
                ps2_flags |= PS2_KEYUP;
            else {
              uint8_t up = ps2_flags & PS2_KEYUP;
              switch (a) {
                case 0x12:  // LSHIFT
                case 0x59:  // RSHIFT
                  if (up)
                      ps2_flags &= ~PS2_SHIFT;
                  else
                      ps2_flags |= PS2_SHIFT;
                  ps2_flags &= 0xFC;
                  break;
                case 0x58:  // CAPS
                    if (!up)
                        ps2_flags ^= PS2_CAPS;
                    ps2_flags &= 0xFC;
                    break;
                default:
                  if (!up)
                    ps2_key = a; // only pass keydowns
                  else
                    ps2_flags &= 0xFC;
              }
            }
        }
        else
        {
            ps2_key = 0;
            ps2_flags &= 0xFC;
        }
        ps2_code = 1;
    }
}

void enable_ps2()
{
    ps2_code = 0x1;
    ps2_key = 0;
    DDRD &= ~_BV(PS2_CLOCK);
    EICRA |= (0 << ISC10) | (1 << ISC11);
    EIFR |= (1 << INTF1);
    EIMSK |= (1 << INT1);
}

void disable_ps2()
{
    EIMSK &= ~(1 << INT1);
    EIFR |= (1 << INTF1);
    DDRD |= _BV(PS2_CLOCK);
    PORTD &= ~_BV(PS2_CLOCK);  // Drive clock low when disabled
}
