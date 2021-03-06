/**
 * Text drawing routines
 * @file
 */

#ifndef MINOTE_TEXT_H
#define MINOTE_TEXT_H

#include "base/math.hpp"
#include "engine/engine.hpp"
#include "engine/font.hpp"

/**
 * Initialize text drawing. Must be called after fontInit().
 * Must be called before any other text functions.
 */
void textInit(void);

/**
 * Clean up the text drawing system. No other text function can be used until
 * textInit() is called again.
 */
void textCleanup(void);

/**
 * Queue up a string of text to be drawn on the screen. printf formatting
 * is supported.
 * @param font Font type to use
 * @param size Font size multiplier
 * @param pos Bottom left corner of the text
 * @param color Text color
 * @param fmt Format string
 * @param ... Any additional arguments for string replacement
 */
void textQueue(minote::Font& font, float size, minote::vec3 pos, minote::color4 color,
	const char* fmt, ...);

/**
 * Queue up a string of text to be drawn on the screen in the specified
 * direction. printf formatting is supported.
 * @param font Font type to use
 * @param size Font size multiplier
 * @param pos Bottom left corner of the text
 * @param dir Direction of the text, normalized
 * @param up Normalized up vector
 * @param color Text color
 * @param fmt Format string
 * @param ... Any additional arguments for string replacement
 */
void textQueueDir(minote::Font& font, float size, minote::vec3 pos, minote::vec3 dir, minote::vec3 up,
	minote::color4 color, const char* fmt, ...);

/**
 * Render all queued strings on the screen, with as few draw calls as possible.
 */
void textDraw(minote::Engine& engine);

#endif //MINOTE_TEXT_H
