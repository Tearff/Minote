/**
 * Game entry point
 * @file
 */

#include "main.hpp"

#include <cstdlib>
#include <clocale>
#include <thread>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif //_WIN32
#include "window.hpp"
#include "system.hpp"
#include "debug.hpp"
#include "game.hpp"
#include "log.hpp"

using namespace minote;
using namespace minote::log;

/**
 * Entry point function. Initializes systems and spawns other threads. Itself
 * becomes the input handling thread.
 * @return EXIT_SUCCESS on successful execution, EXIT_FAILURE on a handled
 * critical error, other values on unhandled error
 */
auto main(int, char*[]) -> int
{
	//region Initialization

	// Locale and Unicode
	std::setlocale(LC_ALL, ""); // Use system locale
	std::setlocale(LC_NUMERIC, "C"); // Switch to predictable number format
	std::setlocale(LC_TIME, "C"); // Switch to predictable datetime format
#ifdef _WIN32
	SetConsoleOutputCP(65001); // Set Windows cmd encoding to UTF-8
#endif //_WIN32

	// Global logging
#ifndef NDEBUG
	L.level = Log::Level::Trace;
	constexpr char logfile[]{"minote.log"};
#else //NDEBUG
	L.level = Log::Level::Info;
	constexpr char logfile[]{"minote-debug.log"};
#endif //NDEBUG
	L.console = true;
	L.enableFile(logfile);
	L.info("Starting up %s %s", AppName, AppVersion);

	// Window creation
	systemInit();
	char windowTitle[64]{""};
	std::snprintf(windowTitle, 64, "%s %s", AppName, AppVersion);
	windowInit(windowTitle, {1280, 720}, false);
#ifdef MINOTE_DEBUG
	debugInputSetup();
#endif //MINOTE_DEBUG

	//endregion Initialization

	std::thread gameThread{game};

	// Input loop
	while (windowIsOpen()) {
		windowPoll();
		sleepFor(secToNsec(1) / 1000); // 1ms minimum
	}

	gameThread.join();

	// Cleanup
	windowCleanup();
	systemCleanup();
	L.disableFile();

	return EXIT_SUCCESS;
}