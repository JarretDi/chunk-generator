#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <exception>

#include "block.h"
#include "camera.h"
#include "player.h"
#include "shader.h"
#include "world.h"

using glm::vec3;
using glm::mat4;

constexpr int WIDTH = 1600;
constexpr int HEIGHT = 1200;

GLFWwindow * window;
Player player;
World world;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	Camera& camera = player.getCamera();
	bool hasMoved = false;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		hasMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		hasMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		hasMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		hasMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
	if (hasMoved) {
		world.loadChunks(player.getChunkCoords());
	}
}

float lastX = WIDTH / 2, lastY = HEIGHT / 2;

void process_mouse_movement(GLFWwindow* window, double xPos, double yPos) {
	double xOffset = xPos - lastX;
	double yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	player.getCamera().ProcessMouseMovement(xOffset, yOffset, true);
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

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, &frame_buffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, &process_mouse_movement);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glCullFace(GL_FRONT); 
}

unsigned int loadTexture(char const * path) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (!data) {
		std::cout << "ERR :: IMAGE FAILED TO LOAD" << std::endl;
		return 0;
	}

	GLenum format;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// setup texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

	return texture;
}

void draw(Shader& blockShader, glm::vec3& lightColour)
{
	blockShader.use();
	mat4 view = player.getCamera().GetViewMatrix();
	static mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, (float)RENDER_DISTANCE * std::max(CHUNK_MAX_X, CHUNK_MAX_Z));
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);

	glm::ivec2 playerChunk = player.getChunkCoords();

	blockShader.setVec3("lightColour", lightColour);
	blockShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
	blockShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
	blockShader.setVec3("material.specular", vec3(0.1f));
	blockShader.setFloat("material.shininess", 128.0f);

	glm::vec3 ambientColor = lightColour * glm::vec3(0.2f);
	glm::vec3 diffuseColor = lightColour * glm::vec3(0.5f);
	glm::vec3 specularColour = glm::vec3(1.0f);

	glm::vec3 lightDir = glm::normalize(view * glm::vec4(cos(glfwGetTime() / 2), sin(glfwGetTime() / 2), 0.2f, 0.0f));
	blockShader.setVec3("light.ambient", ambientColor);
	blockShader.setVec3("light.diffuse", diffuseColor);
	blockShader.setVec3("light.specular", specularColour);
	blockShader.setVec3("light.direction", lightDir);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	world.draw(blockShader, playerChunk);
}

int main() {
	try {
		initialize();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	Shader blockShader("chunk-generator/shader.vs", "chunk-generator/shader.fs");
	player = Player();
	player.getCamera().MovementSpeed = 25.0f;
	world = World(0, 16);

	
	unsigned int dirtTex = loadTexture("grass.png");

	std::cout << "Worldsize:" << sizeof(World) << std::endl;
	std::cout << "Chunksize:" << sizeof(Chunk) << std::endl;

	glm::vec3 lightColour(1.0f);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

		glClearColor(0.3, 0.75, 0.85, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world.update();

		draw(blockShader, lightColour);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}