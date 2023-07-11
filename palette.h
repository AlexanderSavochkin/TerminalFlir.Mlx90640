#pragma once

#include "color.h"

#define MAX_PALETTE_COUNT 3
#define MAX_COLORTABLE_SIZE 512

typedef struct RGB (*pfnMapValueToColor)(double, double, double, void*);

struct Palette {
    char *name;
    pfnMapValueToColor map_value_to_color;
    void* extra_data;
};

struct PaletteColorTable
{
};

void init_default_palettes();

extern struct Palette palettes[MAX_PALETTE_COUNT];
extern int number_of_palettes;
