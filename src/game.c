/**
 * Implementation of game.h
 * @file
 */

#include "game.h"

#include "renderer.h"
#include "effects.h"
#include "window.h"
#include "mapper.h"
#include "bloom.h"
#include "debug.h"
#include "world.h"
#include "model.h"
#include "util.h"
#include "play.h"
#include "aa.h"

static void gameInit(void)
{
	mapperInit();
	rendererInit();
	modelInit();
	bloomInit();
	aaInit(AAComplex);
	worldInit();
#ifdef MINOTE_DEBUG
	debugInit();
#endif //MINOTE_DEBUG
	playInit();
	effectsInit();
}

static void gameCleanup(void)
{
	effectsCleanup();
	playCleanup();
#ifdef MINOTE_DEBUG
	debugCleanup();
#endif //MINOTE_DEBUG
	worldCleanup();
	aaCleanup();
	bloomCleanup();
	modelCleanup();
	rendererCleanup();
	mapperCleanup();
}

static void gameDebug(void)
{
	static AAMode aa = AAComplex;
	if (nk_begin(nkCtx(), "Settings", nk_rect(1070, 30, 180, 220),
		NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_NO_SCROLLBAR)) {
		nk_layout_row_dynamic(nkCtx(), 20, 1);
		bool sync = nk_check_label(nkCtx(), "GPU synchronization", rendererGetSync());
		if (sync != rendererGetSync())
			rendererSetSync(sync);
		nk_label(nkCtx(), "Antialiasing:", NK_TEXT_LEFT);
		if (nk_option_label(nkCtx(), "None", aa == AANone)) {
			aa = AANone;
			aaSwitch(aa);
		}
		if (nk_option_label(nkCtx(), "SMAA 1x", aa == AAFast)) {
			aa = AAFast;
			aaSwitch(aa);
		}
		if (nk_option_label(nkCtx(), "MSAA 4x", aa == AASimple)) {
			aa = AASimple;
			aaSwitch(aa);
		}
		if (nk_option_label(nkCtx(), "SMAA S2x", aa == AAComplex)) {
			aa = AAComplex;
			aaSwitch(aa);
		}
		if (nk_option_label(nkCtx(), "MSAA 8x", aa == AAExtreme)) {
			aa = AAExtreme;
			aaSwitch(aa);
		}
	}
	nk_end(nkCtx());
}

static void gameUpdate(void)
{
	mapperUpdate();
#ifdef MINOTE_DEBUG
	debugUpdate();
	gameDebug();
#endif //MINOTE_DEBUG
	playUpdate();
	worldUpdate();
	effectsUpdate();
}

static void gameDraw(void)
{
	rendererFrameBegin();
	aaBegin();
	playDraw();
	effectsDraw();
	aaEnd();
	bloomApply();
#ifdef MINOTE_DEBUG
	debugDraw();
#endif //MINOTE_DEBUG
	rendererFrameEnd();
}

void* game(void* arg)
{
	(void)arg;
	gameInit();

	while (windowIsOpen()) {
		gameUpdate();
		gameDraw();
	}

	gameCleanup();
	return null;
}
