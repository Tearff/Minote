// Minote - window.cpp

#include "window.h"

#include <string>
#include "gsl/gsl"
#include "log.h"

using namespace std::string_literals;

Window::Window(System& sys, std::string_view name, bool fullscreen, Size<int> size)
		:system{sys},
		 size{size},
		 fullscreen{fullscreen}
{
	Expects(size.x > 0 && size.y > 0);

	// OpenGL 3.3 Core Profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); // DPI aware
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE); // Linear gamma
	glfwWindowHint(GLFW_SAMPLES, 4); // 4xMSAA
	if (fullscreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		window = glfwCreateWindow(mode->width, mode->height, std::string{name}.c_str(), monitor, nullptr);
	}
	else {
		window = glfwCreateWindow(size.x, size.y, std::string{name}.c_str(), nullptr, nullptr);
	}
	system.checkError("Failed to create a "s + to_string(size) + " window"s);
	glfwSetWindowUserPointer(window, this);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetWindowContentScaleCallback(window, windowScaleCallback);

	// An initial check is required to get correct values for non-100% scaling
	glfwGetFramebufferSize(window, &size.x, &size.y);
	framebufferResizeCallback(window, size.x, size.y);
	float fScale{0.0};
	glfwGetWindowContentScale(window, &fScale, nullptr);
	windowScaleCallback(window, fScale, 0);

	glfwSetKeyCallback(window, keyCallback);

	Log::info("Created a ", fullscreen ? "fullscreen " : "", to_string(size), " window");

	Ensures(window);
	Ensures(scale > 0.0);
}

Window::~Window()
{
	glfwDestroyWindow(window);
}

auto Window::isOpen() -> bool
{
	std::unique_lock lock{openMutex};
	return !glfwWindowShouldClose(window);
}

auto Window::close() -> void
{
	Expects(isOpen());

	std::unique_lock lock{openMutex};
	glfwSetWindowShouldClose(window, true);

	Ensures(!isOpen());
}

auto Window::framebufferResizeCallback(GLFWwindow* window, int w, int h) -> void
{
	Expects(glfwGetWindowUserPointer(window));
	Expects(w > 0 && h > 0);

	auto* object = static_cast<Window*>(glfwGetWindowUserPointer(window));
	object->size.x = w;
	object->size.y = h;
	Log::debug("Framebuffer resized to ", to_string(object->size));
}

auto Window::windowScaleCallback(GLFWwindow* window, float xScale, float) -> void
{
	Expects(glfwGetWindowUserPointer(window));
	Expects(xScale > 0.0f);

	auto* object = static_cast<Window*>(glfwGetWindowUserPointer(window));
	object->scale = xScale;
	Log::debug("DPI scale changed to ", std::to_string(object->scale), "x");
}

auto Window::keyCallback(GLFWwindow* window, int key, int, int action, int) -> void
{
	Expects(glfwGetWindowUserPointer(window));

	auto* object = static_cast<Window*>(glfwGetWindowUserPointer(window));
	object->pushInput({key, action, object->system.getTime()});
}

auto Window::pushInput(Input i) -> void
{
	std::unique_lock lock{inputsMutex};
	inputs.push(i);
}

auto Window::popInput() -> std::optional<Input>
{
	std::unique_lock lock{inputsMutex};

	if (inputs.empty())
		return std::nullopt;

	Input result{inputs.front()};
	inputs.pop();
	return result;
}

auto Window::attachContext() -> void
{
	glfwMakeContextCurrent(window);
	system.checkError("Failed to activate the OpenGL context");
}

auto Window::detachContext() -> void
{
	glfwMakeContextCurrent(nullptr);
	system.checkError("Failed to deactive the OpenGL context");
}

auto Window::swapBuffers() -> void
{
	glfwSwapBuffers(window);
	system.checkError("Failed to swap renderbuffers");
}
