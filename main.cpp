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

int main() {
	try {
		initialize();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}