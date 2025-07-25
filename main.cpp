#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <exception>

#include "shader.h"
#include "camera.h"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

GLFWwindow * window;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void initialize() {
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);

	window = glfwCreateWindow(WIDTH, HEIGHT, "ChunkGenerator", NULL, NULL);
	if (window == nullptr) {
		glfwTerminate();
		throw std::exception("Failed to Initialize Window");
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		glfwTerminate();
		throw std::exception("Failed to initialize GLAD");
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, &frame_buffer_size_callback);
}

const float cubeVertices[] = {
	// front face
	-0.5, -0.5, 0.5, // bottom left
	-0.5,  0.5, 0.5, // top left
	 0.5,  0.5, 0.5, // top right
	 0.5, -0.5, 0.5, // bottom right
	// back face
	-0.5, -0.5, -0.5,
	-0.5,  0.5, -0.5,
	 0.5, -0.5, -0.5,
	 0.5,  0.5, -0.5,
};

const int cubeIndices[] = {
	// front face
	0, 1, 2, // top left
	0, 2, 3, // top right
	// top face
	1, 2, 5,
	2, 5, 6,
	// right face
	2, 3, 6,
	3, 6, 7,
	// bottom face
	0, 3, 4,
	3, 4, 7,
	// left face
	0, 1, 4,
	1, 4, 5,
	// back face
	4, 5, 6,
	4, 6, 7
};

int main() {
	try {
		initialize();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	Shader blockShader("shader.vs", "shader.fs");

	unsigned int cubeVAO, cubeVBO, cubeEBO;

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.3, 0.75, 0.85, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}