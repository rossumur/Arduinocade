
#define MAX_SPRITE_X 160
#define MAX_SPRITE_Y 192
#define MAX_RAM_TILES  64  // Bigger (32,64) if you have lots of sprites
                           // 10 for pacman (5 sprites misaligned on 1 axis = 10)
//==============================================================
//==============================================================
//  Sprites
//  Ring buffer of composited sprite tiles

void sprite_init(const uint8_t* font, uint8_t font_count);
uint8_t* sprite_get_tile(uint8_t col, uint8_t row);
bool sprite_add(const uint8_t* data, short x, short y, uint8_t clipy = MAX_SPRITE_Y);
uint8_t* sprite_draw(uint8_t* line, uint8_t row);
