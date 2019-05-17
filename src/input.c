#include "input.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "state.h"
#include "window.h"
#include "timer.h"

#define INPUT_FREQUENCY 1000 // in Hz
#define TIME_PER_POLL (SEC / INPUT_FREQUENCY)
static nsec lastPollTime = 0;

void initInput() {
	// Nothing here but us comments
}

void cleanupInput() {
	// To be continued...
}

void updateInput() {
	lastPollTime = getTime();
	glfwPollEvents();
	if(glfwWindowShouldClose(window))
		setRunning(false);
}

void sleepInput() {
	nsec timePassed = getTime() - lastPollTime;
	if(timePassed < TIME_PER_POLL) // Only bother sleeping if we're ahead of the target
		sleep(TIME_PER_POLL - timePassed);
}