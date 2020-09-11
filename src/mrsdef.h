/**
 * Definitions and tweakables for the mrs playmode
 * @file
 */

#ifndef MINOTE_MRSDEF_H
#define MINOTE_MRSDEF_H

#include "mino.h"

// Logic defs

#define MrsSpawnX 3 ///< X position of player piece spawn
#define MrsSpawnY 18 ///< Y position of player piece spawn
#define MrsSubGrid 256 ///< Number of subpixels per cell, used for gravity

#define MrsStartingTokens 6 ///< Number of tokens that each piece starts with

#define MrsAutoshiftCharge 12 ///< Frames direction has to be held before autoshift
#define MrsAutoshiftRepeat 1 ///< Frames between autoshifts
#define MrsLockDelay 40 ///< Frames a piece can spend on the stack before locking
#define MrsClearOffset 5 ///< Frames between piece lock and line clear
#define MrsClearDelay 30 ///< Frames between line clear and thump
#define MrsSpawnDelay 24 ///< Frames between lock/thump and new piece spawn

// Graphics defs

#define MrsFieldHeightVisible 20u ///< Number of bottom rows the player can see
#define MrsPreviewX -2.0f ///< X offset of preview piece
#define MrsPreviewY 21.0f ///< Y offset of preview piece
#define MrsFieldDim 0.3f ///< Multiplier of field block color
#define MrsExtraRowDim 0.25f ///< Multiplier of field block alpha above the scene
#define MrsGhostDim 0.2f ///< Multiplier of ghost block alpha
#define MrsBorderDim 0.5f ///< Multiplier of border alpha
#define MrsLockFlashBrightness 1.2f ///< Color value of lock flash highlight
#define MrsParticlesClearBoost 1.4f ///< Intensity multiplier for line clear effect

/**
 * Query the mrs for a specific piece.
 * @param type Type of the piece, between MinoNone and MinoGarbage (exclusive)
 * @param rotation Spin of the piece
 * @return Read-only piece data
 */
piece* mrsGetPiece(mino type, spin rotation);

#endif //MINOTE_MRSDEF_H
