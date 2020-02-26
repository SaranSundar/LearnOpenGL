// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>
#include <iostream>
#include "nanogui/nanogui.h"
#include "shaderloader.h"
#include "objloaderindex.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers);
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow*, unsigned int codepoint);

const unsigned int SCREEN_WIDTH = 1200;
const unsigned int SCREEN_HEIGHT = 900;

// Variables for the GUI
using namespace nanogui;
FormHelper* gui = nullptr;
float translateX = 0.0f;
float translateY = 0.0f;
float translateZ = 0.0f;
float offsetYaw = 0;
float offsetPitch = 0;
float offsetRoll = 0;
float prevYaw = 0;
float prevPitch = 0;
float prevRoll = 0;
float zNear = 0.4f;
float zFar = 15.0f;

Color modelColor(1.0f, 1.0f, 1.0f, 1.0f);
int modelShine = 32;
bool directionalLightStatus = false;
bool positionalLightStatus = false;

float positionalLightRadius = 5;
float positionalLightAngleX = 0;
float positionalLightAngleY = 0;
float positionalLightAngleZ = 0;

Color positionalLightAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
Color positionalLightDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
Color positionalLightSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);

Color directionalLightAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);
Color directionalLightDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
Color directionalLightSpecularColor(0.0f, 0.0f, 0.0f, 1.0f);

bool rotatePositionalLightX = false;
bool rotatePositionalLightY = false;
bool rotatePositionalLightZ = false;

enum test_enum {
	Item1,
	Item2,
	Item3
};
test_enum renderType = test_enum::Item3;
test_enum shadingType = test_enum::Item2;
test_enum depthType = test_enum::Item2;
test_enum cullingType = test_enum::Item2;

std::string modelName = "cyborg.obj";
Screen* screen = nullptr;

//camera settings
glm::vec3 cam_pos = glm::vec3(0, 2, 6);
glm::vec3 cam_dir = glm::vec3(0, 0, -1); //direction means what the camera is looking at
glm::vec3 temp_cam_dir = glm::vec3(0, 0, 1); //use this for the cross product or else when cam_dir and cam_up overlap, the cross product will be 0 (bad!)
glm::vec3 cam_up = glm::vec3(0, 1, 0); //up defines where the top of the camera is directing towards

//model settings
glm::mat4 model = glm::mat4(1.0f); //to apply scalor and rotational transformations
glm::mat4 model_original = glm::mat4(1.0f); //to apply scalor and rotational transformations
glm::vec3 modl_move = glm::vec3(0, 0, 0); //to apply translational transformations

//color settings
bool flag = false;
bool lights = false;
bool normalcol = false;
bool greyscale = false;
bool red = false;
bool green = false;
bool blue = false;
bool colour = false;

GLuint vertices_VBO;
GLuint VAO;
GLuint normals_VBO;
GLuint EBO;


std::vector<int> indices;
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<glm::vec2> UVs;

void resetCamera() {

	translateX = 0.0f;
	translateY = 0.0f;
	translateZ = 0.0f;
	offsetYaw = 0;
	offsetPitch = 0;
	offsetRoll = 0;
	prevYaw = 0;
	prevPitch = 0;
	prevRoll = 0;
	zNear = 0.4f;
	zFar = 15.0f;

	modl_move.y = 0;
	modl_move.x = 0;
	modl_move.z = 0;

	model = model_original;
	gui->refresh();

}

void recalculateVectors() {
	modl_move.y = translateY;
	modl_move.x = translateX;
	modl_move.z = translateZ;

	// Yaw, Pitch, Roll
	if (offsetYaw != prevYaw) {
		model = glm::rotate(model, glm::radians(offsetYaw), glm::vec3(1, 0, 0));
		prevYaw = offsetYaw;
	}
	if (offsetPitch != prevPitch) {
		model = glm::rotate(model, glm::radians(offsetPitch), glm::vec3(0, 0, 1));
		prevPitch = offsetPitch;
	}
	if (offsetRoll != prevRoll) {
		model = glm::rotate(model, glm::radians(offsetRoll), glm::vec3(0, 1, 0));
		prevRoll = offsetRoll;
	}
}

void reloadModel(const char* pathName) {

	std::vector<int> indices2;
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec3> normals2;
	std::vector<glm::vec2> UVs2;

	// If this path is wrong it causes nanogui to fail and show a white screen
	bool isSuccesfull = loadOBJ(pathName, indices2, vertices2, normals2, UVs2);

	if (isSuccesfull) {
		indices.clear();
		vertices.clear();
		normals.clear();
		UVs.clear();
		indices = indices2;
		vertices = vertices2;
		normals = normals2;
		UVs = UVs2;

		glBindVertexArray(VAO); //if you take this off nothing will show up because you haven't linked the VAO to the VBO
								//you have to bind before putting the point

		glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  //3*sizeof(GLfloat) is the offset of 3 float numbers
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  //3*sizeof(GLfloat) is the offset of 3 float numbers
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices.front(), GL_STATIC_DRAW);

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}
}

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

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Creates a nanogui screen and pass the glfw pointer to initialize
	screen = new Screen();
	screen->initialize(window, true);

	// Start of nanogui gui
	bool enabled = true;
	gui = new FormHelper(screen);
	ref<Window> nanoguiWindow =
		// First nanogui gui
		gui->addWindow(Eigen::Vector2i(10, 10), "Control Bar 1");
	gui->addGroup("Camera Position");
	gui->addVariable("X", translateX)->setSpinnable(true);
	gui->addVariable("Y", translateY)->setSpinnable(true);
	gui->addVariable("Z", translateZ)->setSpinnable(true);

	gui->addGroup("Camera Rotatation");
	gui->addVariable("Yaw", offsetYaw)->setSpinnable(true);
	gui->addVariable("Pitch", offsetPitch)->setSpinnable(true);
	gui->addVariable("Roll", offsetRoll)->setSpinnable(true);

	gui->addGroup("Configuration");
	gui->addVariable("Z Near", zNear)->setSpinnable(true);
	gui->addVariable("Z Far", zFar)->setSpinnable(true);
	gui->addVariable("Render Type", renderType, enabled)->setItems({ "Point", "Line", "Triangle" });
	gui->addVariable("Culling Type", cullingType, enabled)->setItems({ "CW", "CCW" });
	gui->addVariable("Shading Type", shadingType, enabled)->setItems({ "FLAT", "SMOOTH" });
	gui->addVariable("Depth Type", depthType, enabled)->setItems({ "LESS", "ALWAYS" });
	gui->addVariable("Model Name", modelName);
	gui->addButton("Reload model", []() {
		// Loads inputted model
		std::string pathName = "resources/objects/" + modelName;
		reloadModel(pathName.c_str());
		resetCamera();
		});
	gui->addButton("Reset Camera", []() {
		resetCamera();
		});

	// Second nanogui gui
	gui->addWindow(Eigen::Vector2i(210, 10), "Control Bar 2");
	gui->addGroup("Lighting");
	gui->addVariable("Object Color:", modelColor);
	gui->addVariable("Object Shininess", modelShine);
	gui->addVariable("Direction Light Status", directionalLightStatus);
	gui->addVariable("Direction Light Ambient Color", directionalLightAmbientColor);
	gui->addVariable("Direction Light Diffuse Color", directionalLightDiffuseColor);
	gui->addVariable("Direction Light Specular Color", directionalLightSpecularColor);
	gui->addVariable("Point Light Status", positionalLightStatus);
	gui->addVariable("Point Light Ambient Color", positionalLightAmbientColor);
	gui->addVariable("Point Light Diffuse Color", positionalLightDiffuseColor);
	gui->addVariable("Point Light Specular Color", positionalLightSpecularColor);
	gui->addVariable("Point Light Rotate on X", rotatePositionalLightX);
	gui->addVariable("Point Light Rotate on Y", rotatePositionalLightY);
	gui->addVariable("Point Light Rotate on Z", rotatePositionalLightZ);
	gui->addButton("Reset Point Light", []() {
		// Loads inputted model
		positionalLightAngleX = 0;
		positionalLightAngleY = 0;
		positionalLightAngleZ = 0;
		});
	screen->setVisible(true);
	screen->performLayout();
	//End of nanogui gui

	glEnable(GL_DEPTH_TEST);


	// Sets size of points when rendering as points
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize(2.0);

	 //Build and compile our shader program
	 //Vertex shader

	GLuint shader = loadSHADER("shader.vs", "shader.fs");
	glUseProgram(shader);

	// If this path is wrong it causes nanogui to fail and show a white screen
	loadOBJ("resources/objects/cyborg.obj", indices, vertices, normals, UVs);

	glGenVertexArrays(1, &VAO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //if you take this off nothing will show up because you haven't linked the VAO to the VBO
							//you have to bind before putting the point

	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);  //3*sizeof(GLfloat) is the offset of 3 float numbers
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  //3*sizeof(GLfloat) is the offset of 3 float numbers
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices.front(), GL_STATIC_DRAW);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	//glm is a math funtion
	glm::mat4 modl_matrix = glm::translate(glm::mat4(1.f), glm::vec3(3, 0, 0));
	glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_dir, cam_up);
	glm::mat4 proj_matrix = glm::perspective(glm::radians(45.f), 1.f, zNear, zFar); //perspective view. Third parameter should be > 0, or else errors
	glEnable(GL_DEPTH_TEST); //remove surfaces beyond the cameras render distance

	GLuint vm_loc = glGetUniformLocation(shader, "vm");
	GLuint pm_loc = glGetUniformLocation(shader, "pm");
	GLuint mm_loc = glGetUniformLocation(shader, "mm");
	//GLuint flag_id = glGetUniformLocation(shader, "flag");
	//GLuint lights_id = glGetUniformLocation(shader, "lights");
	//GLuint normalcol_id = glGetUniformLocation(shader, "normalcol");
	//GLuint greyscale_id = glGetUniformLocation(shader, "greyscale");
	//GLuint red_id = glGetUniformLocation(shader, "red");
	//GLuint green_id = glGetUniformLocation(shader, "green");
	//GLuint blue_id = glGetUniformLocation(shader, "blue");
	//GLuint colour_id = glGetUniformLocation(shader, "colour");

	//glUniformMatrix4fv(vm_loc, 1, GL_FALSE, &view_matrix[0][0]); OR
	glUniformMatrix4fv(vm_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(pm_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
	glUniformMatrix4fv(mm_loc, 1, GL_FALSE, glm::value_ptr(modl_matrix));

	glUniform3fv(glGetUniformLocation(shader, "light_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
	glUniform3fv(glGetUniformLocation(shader, "light_position"), 1, glm::value_ptr(glm::vec3(0.0, 20.0, 5.0)));
	glUniform3fv(glGetUniformLocation(shader, "object_color"), 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
	glUniform3fv(glGetUniformLocation(shader, "view_position"), 1, glm::value_ptr(glm::vec3(cam_pos)));



	// Game Loop
	while (!glfwWindowShouldClose(window)) {

		recalculateVectors();

		view_matrix = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);
		
		
		//glUniformMatrix4fv(vm_loc, 1, 0, glm::value_ptr(view_matrix));

		//glm::mat4 rotator = glm::rotate(glm::mat4(1.0f), i / 200.f, glm::vec3(1, 0, 0));
		glm::mat4 translator = glm::translate(glm::mat4(1.0f), modl_move);
		//glm::mat4 scalor = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
		modl_matrix = translator * model;


		glUseProgram(shader);
		vm_loc = glGetUniformLocation(shader, "vm");
		pm_loc = glGetUniformLocation(shader, "pm");
		mm_loc = glGetUniformLocation(shader, "mm");
		GLuint flag_id = glGetUniformLocation(shader, "flag");
		GLuint lights_id = glGetUniformLocation(shader, "lights");
		GLuint normalcol_id = glGetUniformLocation(shader, "normalcol");
		GLuint greyscale_id = glGetUniformLocation(shader, "greyscale");
		GLuint red_id = glGetUniformLocation(shader, "red");
		GLuint green_id = glGetUniformLocation(shader, "green");
		GLuint blue_id = glGetUniformLocation(shader, "blue");
		GLuint colour_id = glGetUniformLocation(shader, "colour");
		GLuint shinyness = glGetUniformLocation(shader, "shinyness");

		//glUniformMatrix4fv(vm_loc, 1, GL_FALSE, &view_matrix[0][0]); OR
		glUniformMatrix4fv(vm_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(pm_loc, 1, GL_FALSE, glm::value_ptr(proj_matrix));
		glUniformMatrix4fv(mm_loc, 1, GL_FALSE, glm::value_ptr(modl_matrix));

		glUniform1i(shinyness, modelShine);

		glUniform3fv(glGetUniformLocation(shader, "light_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
		glUniform3fv(glGetUniformLocation(shader, "light_position"), 1, glm::value_ptr(glm::vec3(0.0, 20.0, 5.0)));
		glUniform3fv(glGetUniformLocation(shader, "object_color"), 1, glm::value_ptr(glm::vec3(modelColor.r(), modelColor.g(), modelColor.b())));
		glUniform3fv(glGetUniformLocation(shader, "view_position"), 1, glm::value_ptr(glm::vec3(cam_pos)));


		
		glUniformMatrix4fv(vm_loc, 1, 0, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(mm_loc, 1, 0, glm::value_ptr(modl_matrix));
		
		//glUniform3fv(object_color_id, 1, glm::value_ptr(object_color));
		glUniform1i(flag_id, flag);
		glUniform1i(lights_id, lights);
		glUniform1i(normalcol_id, normalcol);
		glUniform1i(greyscale_id, greyscale);
		glUniform1i(red_id, red);
		glUniform1i(green_id, green);
		glUniform1i(blue_id, blue);
		glUniform1i(colour_id, colour);

		// Render
		// Clear the colorbuffer

		// Basic setup for depth and rendering
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST); 
		if (depthType == 1) {
			glDepthFunc(GL_LESS);
		}
		else {
			glDepthFunc(GL_ALWAYS);
		}
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Activates shaders and sets all uniforms
		//if (shadingType == 0) {
		//	modelShader.setBool("applySmoothing", false);
		//}
		//else if (shadingType == 1) {
		//	modelShader.setBool("applySmoothing", true);
		//}


		// Sets culling mode for model
		if (cullingType == 0) {
			glFrontFace(GL_CW); // Renders CW
		}
		else {
			glFrontFace(GL_CCW); // Renders CCW
		}

		// Sets render mode
		if (renderType == 0) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // Render as points
		}
		else if (renderType == 1) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Render as lines
		}
		else if (renderType == 2) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Render as triangles
		}
		else {
			std::cout << "Invalid render mode" << std::endl;
		}

		glBindVertexArray(VAO);
		// Sets render mode
		if (renderType == 0) {
			glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0); // Render as points
		}
		else {
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // Render as lines
		}

		glBindVertexArray(0);

		// Draws GUI
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		gui->refresh();

		// Swaps buffers, processes events.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &vertices_VBO);
	glDeleteBuffers(1, &normals_VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}



// Callbacks listen for inputs.
void key_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
	screen->keyCallbackEvent(key, scancode, action, mods);
}

void char_callback(GLFWwindow*, unsigned int codepoint) {
	screen->charCallbackEvent(codepoint);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int modifiers) {
	screen->mouseButtonCallbackEvent(button, action, modifiers);
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	screen->scrollCallbackEvent(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	screen->resizeCallbackEvent(width, height);
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	screen->cursorPosCallbackEvent(xpos, ypos);
}