#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "nanogui/nanogui.h"

#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;




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
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// build and compile our shader program
	Shader shader("shader.vs", "shader.fs");

	// 3 points for a triangle on a normalized plane from -1 to 1, (x, y, z) and the color rgb value for each point
	float vertices[] = {
		// positions         // colors
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
	};

	// order of each 3 points above to form 2 triangles
	unsigned int indices[] = {
	0, 1, 2,   // first triangle
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

	// Tells OpenGL how to read the position data in VBO and then enables it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Tells OpenGL how to read the color data in VBO and then enables it
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// glVertexAttribPointer bound VBO to the VAO, so we can unbind the VBO now so we don't modify it
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO so we don't modify it
	glBindVertexArray(0);


	// Game Loop
	while (!glfwWindowShouldClose(window)) {
		// get input
		processInput(window);

		// render events
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		// use shader and bind VAO
		shader.use();
		glBindVertexArray(VAO);
		// draw a rectangle with starting index and vertices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		// 3 vertices to connect
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		// unbind VAO
		glBindVertexArray(0);

		// double buffer and input events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up remaining resources
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	// Checks if the key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Resizes the window
	glViewport(0, 0, width, height);
}