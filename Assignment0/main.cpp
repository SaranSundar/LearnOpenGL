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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 900;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);



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
test_enum renderType = Item2;
test_enum cullingType = Item1;
std::string modelName = "A string";


Screen* screen = nullptr;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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
		})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
		gui->addButton("Rotate right-", []() {
			//TODO
			std::cout << "Rotate right-" << std::endl;
			})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
			gui->addButton("Rotate up+", []() {
				//TODO
				std::cout << "Rotate up+" << std::endl;
				})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
				gui->addButton("Rotate up-", []() {
					//TODO
					std::cout << "Rotate up-" << std::endl;
					})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
					gui->addButton("Rotate front+", []() {
						//TODO
						std::cout << "Rotate front+" << std::endl;
						})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;
						gui->addButton("Rotate front-", []() {
							//TODO
							std::cout << "Rotate front-" << std::endl;
							})->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

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

							glfwSetMouseButtonCallback(window,
								[](GLFWwindow*, int button, int action, int modifiers) {
									screen->mouseButtonCallbackEvent(button, action, modifiers);
								}
							);

							//End of nanogui gui

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
							glewExperimental = GL_TRUE;
							// Initialize GLEW to setup the OpenGL Function pointers
							glewInit();

							glEnable(GL_DEPTH_TEST);

							// build and compile our shader program
							Shader shader("shader.vs", "shader.fs");

							// set up vertex data (and buffer(s)) and configure vertex attributes
							// ------------------------------------------------------------------
							float vertices[] = {
							-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
							 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
							 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
							 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
							-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
							-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

							-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
							 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
							 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
							 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
							-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
							-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

							-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
							-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
							-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
							-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
							-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
							-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

							 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
							 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
							 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
							 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
							 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
							 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

							-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
							 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
							 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
							 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
							-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
							-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

							-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
							 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
							 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
							 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
							-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
							-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
							};

							glm::vec3 cubePositions[] = {
						  glm::vec3(0.0f,  0.0f,  0.0f),
						  glm::vec3(2.0f,  5.0f, -15.0f),
						  glm::vec3(-1.5f, -2.2f, -2.5f),
						  glm::vec3(-3.8f, -2.0f, -12.3f),
						  glm::vec3(2.4f, -0.4f, -3.5f),
						  glm::vec3(-1.7f,  3.0f, -7.5f),
						  glm::vec3(1.3f, -2.0f, -2.5f),
						  glm::vec3(1.5f,  2.0f, -2.5f),
						  glm::vec3(1.5f,  0.2f, -1.5f),
						  glm::vec3(-1.3f,  1.0f, -1.5f)
							};
							unsigned int indices[] = {
								0, 1, 3, // first triangle
								1, 2, 3  // second triangle
							};

							// VBO and EBO is a list, VAO is a container
							unsigned int VBO, VAO, EBO;

							// Create 1 UID each for the list and container
							glGenVertexArrays(1, &VAO);
							glGenBuffers(1, &VBO);
							glGenBuffers(1, &EBO);

							// Sets container and list to be modifiable
							glBindVertexArray(VAO);

							glBindBuffer(GL_ARRAY_BUFFER, VBO);
							// Copies data into VBO, and the VBO is automatically in the VAO
							glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

							// Copies index order into EBO
							glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
							glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

							// All 3 below stored in VAO

							// position attribute
							glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
							glEnableVertexAttribArray(0);
							// texture coord attribute
							glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
							glEnableVertexAttribArray(1);

							// glVertexAttribPointer bound VBO to the VAO, so we can unbind the VBO now so we don't modify it
							glBindBuffer(GL_ARRAY_BUFFER, 0);
							// Unbind the VAO so we don't modify it
							glBindVertexArray(0);

							//------------------
							// load and create a texture
							unsigned int texture1, texture2;
							glGenTextures(1, &texture1);
							glBindTexture(GL_TEXTURE_2D, texture1);

							// Sets the texture wrap for x,y,z
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

							// set texture filtering parameters
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

							// tell stb_image.h to flip loaded texture's on the y-axis.
							stbi_set_flip_vertically_on_load(true);
							// load image, create texture, and generate mipmaps
							int width, height, numChannels;
							unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &numChannels, 0);
							if (data) {
								glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
								glGenerateMipmap(GL_TEXTURE_2D);
							}
							else {
								std::cout << "Failed to load texture" << std::endl;
							}
							stbi_image_free(data);

							glGenTextures(1, &texture2);
							glBindTexture(GL_TEXTURE_2D, texture2);

							// Sets the texture wrap for x,y,z
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

							// set texture filtering parameters
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

							// load image, create texture, and generate mipmaps
							data = stbi_load("resources/textures/smiley.png", &width, &height, &numChannels, 0);
							if (data) {
								// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
								glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
								glGenerateMipmap(GL_TEXTURE_2D);
							}
							else {
								std::cout << "Failed to load texture" << std::endl;
							}
							stbi_image_free(data);

							// tell OpenGL for each sampler which texture unit it belongs to
							shader.use();
							shader.setInt("texture1", 0);
							shader.setInt("texture2", 1);

							glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
							shader.setMatrix("projection", projection);

							// Game Loop
							while (!glfwWindowShouldClose(window)) {
								// per-frame time logic
								// --------------------
								float currentFrame = glfwGetTime();
								deltaTime = currentFrame - lastFrame;
								lastFrame = currentFrame;



								// get input
								processInput(window);

								// render events
								glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
								glEnable(GL_DEPTH_TEST);
								glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

								// activate and bind textures per unit
								glActiveTexture(GL_TEXTURE0);
								glBindTexture(GL_TEXTURE_2D, texture1);
								glActiveTexture(GL_TEXTURE1);
								glBindTexture(GL_TEXTURE_2D, texture2);

								// activate shader
								shader.use();

								// pass projection matrix to shader (note that in this case it could change every frame)
								glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
								shader.setMatrix("projection", projection);

								// camera/view transformation
								glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
								shader.setMatrix("view", view);

								// render boxes
								glBindVertexArray(VAO);
								for (unsigned int i = 0; i < 10; i++)
								{
									// calculate the model matrix for each object and pass it to shader before drawing
									glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
									model = glm::translate(model, cubePositions[i]);
									float angle = 20.0f * i;
									model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
									shader.setMatrix("model", model);

									glDrawArrays(GL_TRIANGLES, 0, 36);
								}

								// unbind VAO
								glBindVertexArray(0);

								// Draws GUI
								screen->drawWidgets();


								// double buffer and input events
								glfwSwapBuffers(window);
								glfwPollEvents();
							}

							// Clean up remaining resources
							glDeleteVertexArrays(1, &VAO);
							glDeleteBuffers(1, &VBO);
							glDeleteBuffers(1, &EBO);
							glfwTerminate();
							return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Resizes the window
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	screen->cursorPosCallbackEvent(xpos, ypos);
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}