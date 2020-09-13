/**
 * Listing of fonts used by the game
 * @file
 */

#ifndef MINOTE_FONTLIST_H
#define MINOTE_FONTLIST_H

#define FONT_DIR u8"fonts"

typedef enum Fonts {
	FontNone,
	FontJost,
	FontSize
} Fonts;

#define FontList ((const char*[FontSize]){ \
    [FontNone] = u8"",                     \
    [FontJost] = u8"Jost-500-Medium"       \
})

#endif //MINOTE_FONTLIST_H
