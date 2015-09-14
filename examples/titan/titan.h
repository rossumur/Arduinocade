
typedef struct {
    uint8_t kind;
    uint8_t x,y,width,height;
} Explodable;

#ifndef ARDUINO
#define GENERATE_EXPLODABLES
#endif
