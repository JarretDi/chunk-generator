#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <exception>

#include "shader.h"
#include "camera.h"

using glm::vec3;
using glm::mat4;

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
	 0.5,  0.5, -0.5,
	 0.5, -0.5, -0.5,
};

const int cubeIndices[] = {
	// front face
	0, 1, 2,
	0, 2, 3,
	// top face
	1, 5, 6,
	1, 6, 2,
	// right face
	2, 3, 7,
	2, 7, 6,
	// bottom face
	0, 4, 7,
	0, 7, 3,
	// left face
	0, 1, 5,
	0, 5, 4,
	// back face
	4, 5, 6,
	4, 6, 7
};

void bufferCubeData(unsigned int & cubeVAO, unsigned int& cubeVBO, unsigned int& cubeEBO) {
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

int main() {
	try {
		initialize();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	unsigned int cubeVAO, cubeVBO, cubeEBO;
	bufferCubeData(cubeVAO, cubeVBO, cubeEBO);

	Shader blockShader("shader.vs", "shader.fs");
	Camera camera(vec3(0.0f, 0.0f, 3.0f));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.3, 0.75, 0.85, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		blockShader.use();
		mat4 model(1.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		blockShader.setMat4("model", model);
		blockShader.setMat4("view", view);
		blockShader.setMat4("projection", projection);

		blockShader.setVec3("vertexColour", vec3(0.25, 0.85, 0.3));

		glBindVertexArray(cubeVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	
	glfwTerminate();
	return 0;
}