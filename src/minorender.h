#ifndef MINORENDER_H
#define MINORENDER_H

#include "mino.h"
#include "state.h"

void initMinoRenderer(void);
void cleanupMinoRenderer(void);
void queueMinoPlayfield(mino playfield[PLAYFIELD_H][PLAYFIELD_W]);
void queueMinoPlayer(pieceState* cpiece);
void renderMino(void);

#endif