// Minote - log.c

#include "log.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include "thread.h"

#ifdef NDEBUG
#define LOG_FILENAME "minote.log"
#else // NDEBUG
#define LOG_FILENAME "minote-debug.log"
#endif // NDEBUG

// Table for priority level to string conversion
// Indexes match the PRIO macros' values
static const char *prioStrings[] = {"NONE", "DEBUG", "INFO",
                                    "WARN", "ERROR", "CRIT"};

// Thread safety is being handled internally
static mutex logMutex = newMutex;
static FILE *logFile = NULL;
static bool printToLogFile = true;
#ifdef NDEBUG
static bool printToStdout = false;
static int logLevel = 2;
#else // NDEBUG
static bool printToStdout = true;
static int logLevel = 1;
#endif // NDEBUG

// GLFW error functions require a destination pointer
static const char *GLFWerror = NULL;

void initLogging()
{
	if (!printToLogFile)
		return;
	logFile = fopen(LOG_FILENAME, "w");
	if (!logFile) { // Force stderr logging on failure
		printToLogFile = false;
		printToStdout = true;
		logError("Failed to open %s for writing: %s",
		         LOG_FILENAME, strerror(errno));
	}
}

void cleanupLogging()
{
	if (logFile) {
		fclose(logFile);
		logFile = NULL;
	}
}

static void logTo(FILE *file, int prio, const char *fmt, va_list ap)
{
	lockMutex(&logMutex);
	time_t epochtime; // Time for some stupid time struct wrangling
	struct tm *timeinfo;
	time(&epochtime);
	timeinfo = localtime(&epochtime);
	fprintf(file, "%02d:%02d:%02d [%s] ",
	        timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
	        prioStrings[prio]);
	vfprintf(file, fmt, ap);
	putc('\n', file);
	unlockMutex(&logMutex);
}

void logPrio(int prio, const char *fmt, ...)
{
	if (prio < logLevel)
		return;
	va_list ap;
	va_start(ap, fmt);
	if (printToLogFile) {
		va_list apcopy;
		va_copy(apcopy, ap);
		logTo(logFile, prio, fmt, apcopy);
		va_end(apcopy);
	}
	if (printToStdout)
		logTo(prio >= 3 ? stderr : stdout, prio, fmt, ap);
	va_end(ap);
}

void logPrioGLFW(int prio, const char *msg)
{
	glfwGetError(&GLFWerror);
	logPrio(prio, "%s: %s", msg,
	        GLFWerror != NULL ? GLFWerror : "Unknown error");
}
