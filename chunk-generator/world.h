#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <vector>

#include "chunk.h"
#include "shader.h"

using glm::vec2;

class World
{
private:
	uint32_t seed;
	vector<std::unique_ptr<Chunk>> chunks;

public:
	World(uint32_t seed, int numChunks = 4);

	void draw(Shader & shader);
};

