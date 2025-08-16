#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <queue>
#include <random>
#include <unordered_map>
#include <vector>

#include "chunk.h"
#include "shader.h"
#include "vecn_hash.hpp"

using glm::ivec2;
using glm::vec2;
using std::priority_queue;

static constexpr int RENDER_DISTANCE = 16;

class World
{
private:
	uint32_t seed;
	unordered_map<vec2, std::unique_ptr<Chunk>, vec2Hash> chunks;

	// used in figuring out which chunk to load first
	struct ChunkTask {
		glm::ivec2 coords;
		float distance;
	};

	struct ChunkTaskCompare {
		bool operator()(const ChunkTask& a, const ChunkTask& b) const {
			return a.distance > b.distance;
		}
	};

	std::priority_queue<ChunkTask, std::vector<ChunkTask>, ChunkTaskCompare> toLoad;

public:
	World() : seed(seed) {}

	World(uint32_t seed, int numChunks = 4);

	void loadChunks(glm::ivec2 playerChunk);

	void update();

	const void draw(Shader & shader, glm::ivec2 playerChunk);
};

