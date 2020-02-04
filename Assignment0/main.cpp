#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "nanogui/nanogui.h"

#include <stb_image.h>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers);
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow*, unsigned int codepoint);
void drop_callback(GLFWwindow*, int count, const char** filenames);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 900;


//GUI stuff
using namespace nanogui;
Color colval(0.5f, 0.5f, 0.7f, 1.f);
float cameraX = 0;
float cameraY = 0;
float cameraZ = 0;

float rotateValue = 0;

float zNear = 0;
float zFar = 0;
enum test_enum {
	Item1,
	Item2,
	Item3
};
test_enum renderType = test_enum::Item2;
test_enum cullingType = test_enum::Item1;
std::string modelName = "A string";

Screen* screen = nullptr;

int main() {
	// Initialize GLFW to version 3.3
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creates a window object
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD
#if defined(NANOGUI_GLAD)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw std::runtime_error("Could not initialize GLAD!");
	}
	glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
#endif

	// Sets OpenGL Screen Size and register screen resize callback
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, char_callback);
	glfwSetDropCallback(window, drop_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Create a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	// Create nanogui gui
	bool enabled = true;
	FormHelper* gui = new FormHelper(screen);
	ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Controls");
	gui->addGroup("Color");
	gui->addVariable("Object Color:", colval);

	gui->addGroup("Position");
	gui->addVariable("X", cameraX)->setSpinnable(true);
	gui->addVariable("Y", cameraY)->setSpinnable(true);
	gui->addVariable("Z", cameraZ)->setSpinnable(true);

	gui->addGroup("Rotate");
	gui->addVariable("Rotate Value", rotateValue)->setSpinnable(true);
	gui->addButton("Rotate right+", []() {
		//TODO
		std::cout << "Rotate right+" << std::endl;
		});
	gui->addButton("Rotate right-", []() {
		//TODO
		std::cout << "Rotate right-" << std::endl;
		});

	gui->addButton("Rotate up+", []() {
		//TODO
		std::cout << "Rotate up+" << std::endl;
		});
	gui->addButton("Rotate up-", []() {
		//TODO
		std::cout << "Rotate up-" << std::endl;
		});
	gui->addButton("Rotate front+", []() {
		//TODO
		std::cout << "Rotate front+" << std::endl;
		});
	gui->addButton("Rotate front-", []() {
		//TODO
		std::cout << "Rotate front-" << std::endl;
		});

	gui->addGroup("Configuration");
	gui->addVariable("Z Near", zNear)->setSpinnable(true);
	gui->addVariable("Z Far", zFar)->setSpinnable(true);
	gui->addVariable("Render Type", renderType, enabled)->setItems({ "Point", "Line", "Triangle" });
	gui->addVariable("Culling Type", cullingType, enabled)->setItems({ "CW", "CCW" });
	gui->addVariable("Model Name", modelName);
	gui->addButton("Reload model", []() {
		//TODO
		std::cout << "Reload Model" << std::endl;
		});
	gui->addButton("Reset Camera", []() {
		//TODO
		std::cout << "Reset Camera" << std::endl;
		});

	screen->setVisible(true);
	screen->performLayout();

	//End of nanogui gui

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	Shader shader("shader.vs", "shader.fs");

	// Game Loop
	while (!glfwWindowShouldClose(window)) {
		// get input
		processInput(window);

		// render events
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draws GUI
		screen->drawWidgets();


		// double buffer and input events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
	screen->keyCallbackEvent(key, scancode, action, mods);
}

void char_callback(GLFWwindow*, unsigned int codepoint) {
	screen->charCallbackEvent(codepoint);
}

void drop_callback(GLFWwindow*, int count, const char** filenames) {
	screen->dropCallbackEvent(count, filenames);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers) {
	screen->mouseButtonCallbackEvent(button, action, modifiers);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Resizes the window
	glViewport(0, 0, width, height);
	screen->resizeCallbackEvent(width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	screen->cursorPosCallbackEvent(xpos, ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	screen->scrollCallbackEvent(xoffset, yoffset);
}