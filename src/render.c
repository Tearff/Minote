#include "render.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "linmath/linmath.h"
#include "log.h"
#include "window.h"
#include "thread.h"
#include "state.h"
#include "minorender.h"
#include "util.h"

#define destroyShader glDeleteShader

thread rendererThreadID = 0;
mat4x4 projection = {};
int renderWidth = 0; // Calculated from the aspect ratio before the first frame is drawn
int renderHeight = 360;
float renderScale = 1.0f;

int viewportWidth = DEFAULT_WIDTH;
int viewportHeight = DEFAULT_HEIGHT;
float viewportScale = 0.0f;
bool viewportDirty = true;
mutex viewportMutex = newMutex;

static state* gameSnap = NULL;

static GLuint createShader(const GLchar* source, GLenum type) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus == GL_FALSE) {
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		logError("Failed to compile shader: %s", infoLog);
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint createProgram(const GLchar* vertexShaderSrc, const GLchar* fragmentShaderSrc) {
	GLuint vertexShader = createShader(vertexShaderSrc, GL_VERTEX_SHADER);
	if(vertexShader == 0) return 0;
	GLuint fragmentShader = createShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
	if(fragmentShader == 0) {
		destroyShader(vertexShader);
		return 0;
	}
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	GLint linkStatus = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE) {
		GLchar infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		logError("Failed to link program: %s", infoLog);
		glDeleteProgram(program);
		program = 0;
	}
	destroyShader(fragmentShader);
	destroyShader(vertexShader);
	return program;
}

static void renderFrame(void) {
	lockMutex(&viewportMutex);
	if(viewportDirty) { // This is true on the first frame, so this section also initializes
		viewportDirty = false;
		glViewport(0, 0, viewportWidth, viewportHeight);
		renderWidth = (int)((float)viewportWidth / (float)viewportHeight * (float)renderHeight);
		mat4x4_ortho(projection, 0.0f, (float)renderWidth, (float)renderHeight, 0.0f, -1.0f, 1.0f);
		renderScale = viewportScale;
	}
	unlockMutex(&viewportMutex);
	lockMutex(&stateMutex);
	memcpy(gameSnap, game, sizeof(state));
	unlockMutex(&stateMutex);
	
	glClearColor(0.03f, 0.07f, 0.07f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	queueMinoPlayfield(gameSnap->playfield);
	queueMinoPlayerPiece(&gameSnap->playerPiece);
	renderMino();
}

static void cleanupRenderer(void) {
	cleanupMinoRenderer();
	free(gameSnap);
	glfwMakeContextCurrent(NULL); // glfwTerminate() hangs if other threads have a current context
}

static void initRenderer(void) {
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Not possible to get an error message?
		logCrit("Failed to initialize GLAD");
		cleanupRenderer();
		exit(1);
	}
	glfwSwapInterval(1); // Enable vsync
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_FRAMEBUFFER_SRGB); // Enable gamma correction
	
	initMinoRenderer();
	
	gameSnap = allocate(1, sizeof(state));
	
	logInfo("OpenGL renderer initialized");
}

void* rendererThread(void* param) {
	(void)param;
	
	initRenderer();
	
	while(isRunning()) {
		renderFrame();
		glGetError(); // Fix graphics not updating in windowed mode. Fucking Intel.
		glfwSwapBuffers(window); // Blocks until next vertical refresh
	}
	
	cleanupRenderer();
	return NULL;
}

void resizeRenderer(int width, int height) {
	lockMutex(&viewportMutex);
	viewportWidth = width;
	viewportHeight = height;
	viewportDirty = true;
	unlockMutex(&viewportMutex);
}

void rescaleRenderer(float scale) {
	lockMutex(&viewportMutex);
	viewportScale = scale;
	viewportDirty = true;
	unlockMutex(&viewportMutex);
}