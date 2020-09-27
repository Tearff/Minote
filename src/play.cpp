/**
 * Implementation of play.h
 * @file
 */

#include "play.hpp"

#include <assert.h>
#include "window.hpp"
#include "mapper.hpp"
#include "darray.hpp"
#include "util.hpp"
#include "mrs.hpp"
#include "log.hpp"

using minote::L;

/// Timestamp of the next game logic update
static nsec nextUpdate = 0;

/// List of collectedInputs for the next logic frame to process
static darray* collectedInputs = nullptr;

static bool initialized = false;

void playInit(void)
{
	if (initialized) return;

	collectedInputs = darrayCreate(sizeof(Input));
	nextUpdate = getTime() + MrsUpdateTick;
	mrsInit();

	initialized = true;
	L.debug("Play layer initialized");
}

void playCleanup(void)
{
	if (!initialized) return;

	mrsCleanup();
	if (collectedInputs) {
		darrayDestroy(collectedInputs);
		collectedInputs = nullptr;
	}

	initialized = false;
	L.debug("Play layer cleaned up");
}

void playUpdate(void)
{
	assert(initialized);

	// Update as many times as we need to catch up
	while (nextUpdate <= getTime()) {
		Input i;
		while (mapperPeek(&i)) { // Exhaust all collectedInputs...
			if (i.timestamp <= nextUpdate)
				mapperDequeue(static_cast<Input*>(darrayProduce(collectedInputs)));
			else
				break; // Or abort if we encounter an input from the future

			// Interpret quit events here for now
			if (i.type == InputQuit && i.state)
				windowClose();
		}

		mrsAdvance(collectedInputs);
		darrayClear(collectedInputs);
		nextUpdate += MrsUpdateTick;
	}
}

void playDraw(void)
{
	assert(initialized);
	mrsDraw();
}
