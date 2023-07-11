/*** Includes ***/
#include <stdio.h>
#include <string.h>

#include "termutils.h"
#include "palette.h"

template<typename T>
T clamp(T x, T low, T high) {
    return  x > high ? high : ((x < low) ? low : x);
}

struct Palette palettes[MAX_PALETTE_COUNT];
struct RGB colortable[MAX_COLORTABLE_SIZE];
int number_of_palettes;

RGB black_hot(double value, double min_value, double max_value, void* _)
{
    value = clamp(value, min_value, max_value);
    unsigned char intensity = 255 - value / (max_value - min_value);
    struct RGB result = {intensity, intensity, intensity};
    return result;
}

RGB white_hot(double value, double min_value, double max_value, void* _)
{
    value = clamp(value, min_value, max_value);
    unsigned char intensity = value / (max_value - min_value);
    struct RGB result = {intensity, intensity, intensity};
    return result;
}

RGB table_lookup(double value, double min_value, double max_value, void* table)
{
    unsigned char intensity = (255) * (value - min_value) / (max_value - min_value);
    struct RGB result = {intensity, intensity, intensity};
    return result;
}

void read_palette(
        const char *table_palette_filename,
        struct RGB *palette_table,
        int *palette_table_size,
        int max_palette_table_size)
{
    FILE* file_pointer;
    const int buffer_length = 255;
    char buffer[buffer_length];
    int irow;

    file_pointer = fopen(table_palette_filename, "r");
    if (file_pointer == NULL) {
        die ("readPalette");
    }        

    for (irow = 0;
         fgets(buffer, buffer_length, file_pointer) && irow < max_palette_table_size;
         ++irow) {
        unsigned char r, g, b;
        sscanf(buffer, "%hhu %hhu %hhu", &r, &g, &b); //FIXME: Considered unsafe
        palette_table[irow].r = r;
        palette_table[irow].g = g;
        palette_table[irow].b = b;

        printf("%d %hhu %hhu %hhu", irow, r, g, b);
    }

    *palette_table_size = irow;

    fclose(file_pointer);
}

void init_default_palettes()
{
    int palette_size;
    read_palette("resources/ironpalette.txt",
            colortable,
            &palette_size,
            MAX_COLORTABLE_SIZE);

    //Table palette
    palettes[0].name = "Iron";
    palettes[0].map_value_to_color = table_lookup;
    palettes[0].extra_data = colortable;

    palettes[1].name = "WH";
    palettes[1].map_value_to_color = black_hot;

    palettes[2].name = "BH";
    palettes[2].map_value_to_color = black_hot;
}

