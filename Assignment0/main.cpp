#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "nanogui/nanogui.h"

#include <stb_image.h>

#include "Shader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers);
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow*, unsigned int codepoint);
void drop_callback(GLFWwindow*, int count, const char** filenames);
void load_model(const char* pathName);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 900;

unsigned int VBO, VAO;

// camera
Camera* camera = nullptr;


//GUI stuff
using namespace nanogui;
Color colval(0.5f, 0.5f, 0.7f, 1.f);
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;

float rotateValue = 5.0f;

float zNear = 0.1f;
float zFar = 10.0f;

float rotateRight = 0.0f;
float rotateFront = 0.0f;
float rotateUp = 0.0f;
enum test_enum {
	Item1,
	Item2,
	Item3
};
test_enum renderType = test_enum::Item2;
test_enum cullingType = test_enum::Item2;
std::string modelName = "cyborg.obj";

Screen* screen = nullptr;

Model* model = nullptr;

FormHelper* gui = nullptr;

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

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	model = new Model();
	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Create a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	// Create nanogui gui
	bool enabled = true;
	gui = new FormHelper(screen);
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
		std::cout << "Rotate right+" << std::endl;
		rotateRight += rotateValue;
		});
	gui->addButton("Rotate right-", []() {
		std::cout << "Rotate right-" << std::endl;
		rotateRight -= rotateValue;
		});

	gui->addButton("Rotate up+", []() {
		std::cout << "Rotate up+" << std::endl;
		rotateUp += rotateValue;
		});
	gui->addButton("Rotate up-", []() {
		std::cout << "Rotate up-" << std::endl;
		rotateUp -= rotateValue;
		});
	gui->addButton("Rotate front+", []() {
		std::cout << "Rotate front+" << std::endl;
		rotateFront += rotateValue;
		});
	gui->addButton("Rotate front-", []() {
		std::cout << "Rotate front-" << std::endl;
		rotateFront -= rotateValue;
		});

	gui->addGroup("Configuration");
	gui->addVariable("Z Near", zNear)->setSpinnable(true);
	gui->addVariable("Z Far", zFar)->setSpinnable(true);
	gui->addVariable("Render Type", renderType, enabled)->setItems({ "Point", "Line", "Triangle" });
	gui->addVariable("Culling Type", cullingType, enabled)->setItems({ "CW", "CCW" });
	gui->addVariable("Model Name", modelName);
	gui->addButton("Reload model", []() {
		std::cout << "Reload Model" << std::endl;
		std::string pathName = "resources/objects/" + modelName;
		load_model(pathName.c_str());
		});
	gui->addButton("Reset Camera", []() {
		std::cout << "Reset Camera" << std::endl;
		camera = nullptr;
		camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
		cameraX = 0.0f;
		cameraY = 0.0f;
		cameraZ = 0.0f;
		cameraZ = 3;
		cameraY = (model->maxY + model->minY) / 2;
		rotateValue = 5.0f;
		rotateRight = 0.0f;
		rotateFront = 0.0f;
		rotateUp = 0.0f;
		zNear = 0.1f;
		zFar = 10.0f;
		gui->refresh();
		});

	screen->setVisible(true);
	screen->performLayout();

	//End of nanogui gui

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	Shader shader("shader.vs", "shader.fs");

	load_model("resources/objects/cyborg.obj");

	// Game Loop
	while (!glfwWindowShouldClose(window)) {

		// get input
		processInput(window);

		// Translates camera from gui input
		// Yaw is turn head left and right
		// Pitch is turn hed up and down
		// Roll is plane doing barrel roll
		camera->TranslateCamera(cameraX, cameraY, cameraZ, rotateRight, rotateUp, rotateFront);

		// render events
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		shader.setVec3("ourColor", colval.r(), colval.g(), colval.b());

		// pass projection matrix to shader 
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, zNear, zFar);
		shader.setMatrix("projection", projection);


		// camera/view transformation
		glm::mat4 view = camera->GetViewMatrix();
		shader.setMatrix("view", view);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO); 
		glm::mat4 modelObj = glm::mat4(1.0f);
		modelObj = glm::translate(modelObj, glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setMatrix("model", modelObj);

		if (cullingType == 0) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		
		if (renderType == 0) {
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize(5.0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
			glDrawArrays(GL_POINTS, 0, model->vertices.size());
		}
		else if (renderType == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
		}
		else if (renderType == 2) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, model->vertices.size());
		}

		// Screen GUI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void load_model(const char* pathName) {
	model->load_obj(pathName);

	cameraZ = 3;
	cameraY = (model->maxY + model->minY) / 2;
	cullingType = test_enum::Item2;
	gui->refresh();

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, model->vertices.size() * sizeof(Model::Vertex), &(model->vertices.front()), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Model::Vertex), (GLvoid*)offsetof(Model::Vertex, Position));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
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
	glViewport(0, 0, width, height);
	screen->resizeCallbackEvent(width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	screen->cursorPosCallbackEvent(xpos, ypos);

}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	screen->scrollCallbackEvent(xoffset, yoffset);
}