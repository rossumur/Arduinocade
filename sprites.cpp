

#include "config.h"
#include "video.h"
#include "sprites.h"

// This looks a bit complicated.
// Basic idea is a sparse frame buffer of ram tiles
// Tiles are sorted by row,col and allocated as sprites are composited over background tiles
// sprite_add is called during blanking
// sprite_draw patches lines to inserted composited ram tiles

// Uses globals for faster addressing on AVR

struct {
    uint8_t read;
    uint8_t write;
    
    uint8_t tile;    // current tile
    uint8_t tiles[MAX_RAM_TILES*16];
    uint8_t sort[MAX_RAM_TILES];
    uint16_t key[MAX_RAM_TILES];
    
    const uint8_t* font;
    uint8_t font_count;
} sengine;

void sprite_init(const uint8_t* font, uint8_t font_count)
{
    sengine.font = font;
    sengine.font_count = font_count;
    sengine.read = sengine.write = 0;
}

//  call during draw to patch sprites
uint8_t* sprite_draw(uint8_t* line, uint8_t row)
{
    while (sengine.read < sengine.write)
    {
        uint8_t c = sengine.sort[sengine.read];
        uint16_t k = sengine.key[c];
        if ((k >> 8) != row)
            break;
        line[(uint8_t)k] = sengine.font_count + c;
        sengine.read++;
    }
    return sengine.tiles;
}

static
int8_t set_pos(uint8_t col, uint8_t row)
{
    if (sengine.write == MAX_RAM_TILES)
        return -1;
    //printf("%d:%d\n",row,col);
    
    uint8_t* s = sengine.sort;
    uint16_t key = (row << 8) | col;
    
    // Binary search for key
    int8_t imin = 0;
    int8_t imax = sengine.write-1;
    while (imin < imax)
    {
        int8_t imid = (imin + imax) >> 1;
        if (sengine.key[s[imid]] < key)
            imin = imid + 1;
        else
            imax = imid;
    }
    
    // Already exists
    int8_t i = s[imin];
    if ((imax == imin) && sengine.key[i] == key)
        return i;
    
    // Allocate a new tile, insertion sort
    int8_t k = i = sengine.write++;
    if (k && key > sengine.key[s[k-1]])
        imin = k;           // Tack on end
    for (; k > imin; k--)
        s[k] = s[k-1];      // might move up to MAX_SPRITE_TILES bytes
    s[k] = i;
    sengine.key[i] = key;
    
    // Fill in tile
    uint8_t* dst = sengine.tiles + (i << 4);
    uint8_t c = current_game()->get_tile(col,row);
    if (c < 2)
    {
        uint8_t n = 4;
        while (n--)
        {
            *dst++ = 0; // black by convention
            *dst++ = 0;
            *dst++ = 0;
            *dst++ = 0;
        }
    } else {
        const uint8_t* src = sengine.font + (c << 1);
        uint16_t rowBytes = sengine.font_count << 1;
        uint8_t n = 8;
        do
        {
            *dst++ = pgm_read_byte(src);
            *dst++ = pgm_read_byte(src+1);
            src += rowBytes;
        } while (--n);
    }
    return i;
}

// get a tile as ram
uint8_t* sprite_get_tile(uint8_t col, uint8_t row)
{
    uint8_t t = set_pos(col,row);
    if (t == 0xFF)
        return 0;
    return sengine.tiles + (t << 4);
}

// Comes down to reading 2 or 3 bytes, aligning to 2, masking left or right
inline
uint8_t blit_8x8(int8_t sleft, uint8_t dst_offset, uint8_t height, const uint8_t* sd, int8_t rowBytes)
{
    // sleft is position of sprite relative to current tile
    uint8_t *dst = sengine.tiles + ((uint8_t)sengine.tile << 4) + dst_offset;
    int8_t count = sleft + (rowBytes<<1);
    if (count > 4)
        count = 4;
    
    // clip
    if (sleft < 0) {
        sd += -sleft >> 1;
    } else {
        dst += sleft >> 1;
        count -= sleft;
    }
    
    // Horrible, horrible bit of code
    uint8_t d0,d1;
    if (sleft & 1)  // Odd alignment
    {
        if (count == 1)
        {
            do
            {
                d0 = pgm_read_byte(sd);
                if (sleft < 0)
                    d0 <<= 4;
                else
                    d0 >>= 4;
                dst[0] |= d0;
                sd += rowBytes;
                dst += 2;
            } while (--height);
            return 0;
        }
        
        do // Odd alignment
        {
            const uint8_t* s = sd;
            d0 = 0x00;
            if (sleft < 0)
                d0 = pgm_read_byte(s++) << 4;
            d1 = pgm_read_byte(s++);
            *dst++ |= d0 | (d1 >> 4);
            d0 = d1 << 4;
            if ((count == 4) || (sleft > 0))
                d0 |= pgm_read_byte(s++) >> 4;
            *dst++ |= d0;
            sd += rowBytes;
        } while (--height);
        return 0;
    }
    
    // Even alignment
    uint8_t m1 = count > 2 ? 0xFF: 0x00;
    rowBytes -= 2;
    do
    {
        d0 = pgm_read_byte(sd++);
        d1 = pgm_read_byte(sd++) & m1;
        *dst++ |= d0;
        *dst++ |= d1;
        sd += rowBytes;
    } while (--height);
    return 0;
}

bool sprite_add(const uint8_t* data, short sx, short sy, uint8_t clipy)
{
    if (sx >= MAX_SPRITE_X || sy >= MAX_SPRITE_Y)
        return true;

    uint8_t height = pgm_read_byte(data);
    uint8_t row_bytes = pgm_read_byte(data+1);

    short bottom = sy + height;
    if (bottom <= 0)
        return true;
    data += 4;
    
    // V clip
    uint8_t y0 = sy < 0 ? 0 : sy & ~7;
    int8_t skip = y0-sy;
    if (skip > 0)
        data += skip*row_bytes;
    else
        y0 = sy;

    if ((y0+height) > clipy)
    {
        if (y0 >= clipy)
            return true;
        height = clipy - y0;
    }

    int8_t right = (sx + (row_bytes<<1) - 1) >> 2;
    int8_t left = sx >> 2;
    int8_t fx = sx-(sx & ~3);
    
    while (height)
    {
        uint8_t row = y0 >> 3;
        uint8_t y1 = (y0 + 8) & ~7;
        int8_t lines = y1-y0;       // height
        if (lines > height)
            lines = height;
        
        int8_t x = left;
        int8_t dx = fx;
        while (x <= right)
        {
            if (x >= 0 && x < VCOLS)
            {
                if ((sengine.tile = set_pos(x,row)) == 0xFF)
                    return false;
                blit_8x8(dx,(y0 & 7) << 1,lines,data,row_bytes);
            }
            dx -= 4;
            x++;
        }
        y0 = y1;
        y1 += 8;
        data += row_bytes*lines;
        height -= lines;
    }
    return true;
}
