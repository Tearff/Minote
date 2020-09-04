/**
 * Sublayer: play -> mrs
 * @file
 * Original rotation system, with the goal of retaining depth while being
 * more intuitive to newcomers.
 */

#ifndef MINOTE_MRS_H
#define MINOTE_MRS_H

#include "darray.h"
#include "mapper.h"
#include "mino.h"
#include "util.h"

/// Frequency of game logic updates, simulated by semi-threading, in Hz
#define MrsUpdateFrequency 60.0
/// Inverse of #MrsUpdateFrequency, in ::nsec
#define MrsUpdateTick (secToNsec(1) / MrsUpdateFrequency)

#define FieldWidth 10u ///< Width of the playfield
#define FieldHeight 22u ///< Height of the playfield

/// State of player piece FSM
typedef enum PlayerState {
	PlayerNone, ///< zero value
	PlayerSpawned, ///< the frame of piece spawn
	PlayerActive, ///< piece can be controlled
	PlayerClear, ///< line has been cleared
	PlayerSpawn, ///< waiting to spawn a new piece
	PlayerSize ///< terminator
} PlayerState;

/// A player-controlled active piece
typedef struct Player {
	bool inputMapRaw[InputSize]; ///< Unfiltered input state
	bool inputMap[InputSize]; ///< Filtered input state
	bool inputMapPrev[InputSize]; ///< #inputMap of the previous frame
	InputType lastDirection; ///< None, Left or Right

	PlayerState state;
	mino type; ///< Current player piece
	mino preview; ///< Next player piece
	int tokens[MinoGarbage - 1]; ///< Past player pieces
	spin rotation; ///< ::spin of current piece
	point2i pos; ///< Position of current piece
	int ySub; ///< Y subgrid of current piece
	int yLowest; ///< The bottommost row reached by current piece

	int autoshiftDirection; ///< Autoshift state: -1 left, 1 right, 0 none
	int autoshiftCharge;
	int autoshiftDelay;
	int lockDelay;
	int clearDelay;
	int spawnDelay;
	int gravity;
} Player;

/// State of tetrion FSM
typedef enum TetrionState {
	TetrionNone, ///< zero value
	TetrionReady, ///< intro
	TetrionPlaying, ///< gameplay
	TetrionOutro, ///< outro
	TetrionSize ///< terminator
} TetrionState;

/// A play's logical state
typedef struct Tetrion {
	TetrionState state;
	int ready; ///< Countdown timer
	int frame; ///< Frame counter since #ready = 0

	Field* field;
	bool linesCleared[FieldHeight]; ///< Storage for line clears pending a thump
	Player player;
	Rng* rng;
} Tetrion;

/// Current state of the mode. Read-only.
extern Tetrion mrsTet;

/**
 * Initialize the mrs sublayer. Needs to be called before the layer can be
 * used.
 */
void mrsInit(void);

/**
 * Clean up the mrs sublayer. Play functions cannot be used until playInit() is
 * called again.
 */
void mrsCleanup(void);

/**
 * Simulate one frame of gameplay logic.
 * @param in List of ::Input events that happened during the frame
 */
void mrsAdvance(darray* inputs);

/**
 * Draw the mrs sublayer to the screen.
 */
void mrsDraw(void);

#endif //MINOTE_MRS_H
