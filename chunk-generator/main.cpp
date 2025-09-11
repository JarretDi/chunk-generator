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
Player * gPlayer = nullptr;
World * gWorld = nullptr;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	Camera& camera = gPlayer->camera;
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
		gWorld->loadChunks(gPlayer->getChunkCoords());
	}
}

float lastX = WIDTH / 2, lastY = HEIGHT / 2;

void process_mouse_movement(GLFWwindow* window, double xPos, double yPos) {
	double xOffset = xPos - lastX;
	double yOffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	gPlayer->camera.ProcessMouseMovement(xOffset, yOffset, true);
}

void process_mouse_click(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && gPlayer->selectBlock(*gWorld)) {
		ivec3 blockCoords = gPlayer->getSelected();
		gWorld->removeBlockAt(blockCoords);
	}
}

void initialize() {
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	glfwSetMouseButtonCallback(window, &process_mouse_click);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
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
	mat4 view = gPlayer->camera.GetViewMatrix();
	static mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, (float)RENDER_DISTANCE * std::max(CHUNK_MAX_X, CHUNK_MAX_Z));
	blockShader.setMat4("view", view);
	blockShader.setMat4("projection", projection);

	glm::ivec2 playerChunk = gPlayer->getChunkCoords();

	blockShader.setVec3("lightColour", lightColour);
	blockShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
	blockShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
	blockShader.setVec3("material.specular", vec3(0.1f));
	blockShader.setFloat("material.shininess", 128.0f);

	glm::vec3 ambientColor = lightColour * glm::vec3(0.2f);
	glm::vec3 diffuseColor = lightColour * glm::vec3(0.5f);
	glm::vec3 specularColour = glm::vec3(1.0f);

	glm::vec3 lightDir = glm::normalize(view * glm::vec4(cos(glfwGetTime() / 2.0f), 0.2f, sin(glfwGetTime() / 2.0f), 0.0f));
	blockShader.setVec3("light.ambient", ambientColor);
	blockShader.setVec3("light.diffuse", diffuseColor);
	blockShader.setVec3("light.specular", specularColour);
	blockShader.setVec3("light.direction", lightDir);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gWorld->draw(blockShader, playerChunk);
}

void drawBlockOutline(vec3 coords) {
	static unsigned int VAO = 0, VBO = 0;
	if (VAO == 0) {

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Block::cubeVertices), Block::cubeVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		std::cout << "Outline VAO/VBO:" << VAO << ", " << VBO << "\n";
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	static Shader outlineShader("chunk-generator/shader.vs", "chunk-generator/outline.fs");
	outlineShader.use();

	mat4 model(1.0f);
	model = glm::translate(model, coords);
	model = glm::scale(model, vec3(1.1));

	mat4 view = gPlayer->camera.GetViewMatrix();
	static mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, (float)RENDER_DISTANCE * std::max(CHUNK_MAX_X, CHUNK_MAX_Z));

	outlineShader.setMat4("model", model);
	outlineShader.setMat4("view", view);
	outlineShader.setMat4("projection", projection);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void drawCursor() {
	static float crosshairVerts[] = {
	-0.02f,  0.0f,   0.02f, 0.0f,   
	 0.0f, -0.02f,   0.0f, 0.02f,  
	};

	static Shader crosshairShader("chunk-generator/cursor.vs", "chunk-generator/outline.fs");
	crosshairShader.use();

	static unsigned int VAO = 0, VBO = 0;
	if (VAO == 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVerts), crosshairVerts, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

void Block::BlockRegistry::testRegister() {
	using namespace Block;
	registerBlock({ "Air", {0, 0, 0, 0, 0, 0}, {BlockTag::Air, BlockTag::Transparent} });
	registerBlock({ "Grass", {1, 1, 1, 1, 1, 1}, {} });
}

int main() {
	try {
		initialize();
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	Block::BlockRegistry::getInstance().testRegister();
	Shader blockShader("chunk-generator/shader.vs", "chunk-generator/shader.fs");

	Player player{};
	World world{0};
	gPlayer = &player;
	gWorld = &world;
	gPlayer->camera.MovementSpeed = 25.0f;
	
	unsigned int dirtTex = loadTexture("grass.png");

	std::cout << "World size:" << sizeof(World) << std::endl;
	std::cout << "Chunk size:" << sizeof(Chunk) << std::endl;
	std::cout << "Long size:" << sizeof(long) << std::endl;
	std::cout << "Vec3 size:" << sizeof(glm::vec3) << std::endl;
	std::cout << "Vertex size:" << sizeof(Vertex) << std::endl;
	std::cout << "Vertex2 size:" << sizeof(Vertex2) << std::endl;

	glm::vec3 lightColour(1.0f, 1.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);
		
		glm::vec4 skyColour(0.3, 0.75, 0.85, 1.0); // shade of blue
		skyColour *= glm::dot(glm::normalize(glm::vec3(cos(glfwGetTime() / 2.0f), 0.2f, sin(glfwGetTime() / 2.0f))), {0, 1, 0});

		glClearColor(skyColour.r, skyColour.g, skyColour.b, skyColour.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		world.update();

		draw(blockShader, lightColour);
		if (player.selectBlock(world)) {
			drawBlockOutline(gPlayer->getSelected());
		}
		drawCursor();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}