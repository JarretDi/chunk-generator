#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "shader.h"
#include "vecn_hash.hpp"

using glm::vec2;

static constexpr int renderDistance = 16;

class World
{
private:
	uint32_t seed;
	unordered_map<vec2, std::unique_ptr<Chunk>, vec2Hash> chunks;

public:
	World(uint32_t seed, int numChunks = 4);

	const void draw(Shader & shader, glm::vec2 playerChunk);
};

