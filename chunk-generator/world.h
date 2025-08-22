#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "chunk.h"
#include "shader.h"
#include "vecn_hash.hpp"

using glm::ivec2;
using glm::vec2;
using glm::ivec3;
using glm::vec3;

using std::priority_queue;

static constexpr int RENDER_DISTANCE = 16;

class World
{
private:
	uint32_t seed;
	unordered_map<ivec2, std::unique_ptr<Chunk>, vec2Hash> chunks;

	// used in figuring out which chunk to load first
	struct ChunkTask {
		ivec2 coords;
		int distance;
	};

	struct ChunkTaskCompare {
		bool operator()(const ChunkTask& a, const ChunkTask& b) const {
			return a.distance > b.distance;
		}
	};

	std::priority_queue<ChunkTask, std::vector<ChunkTask>, ChunkTaskCompare> toLoad;
	std::unordered_set<ivec2, vec2Hash> toLoadAdded;

public:
	World() : seed(seed) {}

	World(uint32_t seed, int numChunks = 4);

	void loadChunks(ivec2 playerChunk);

	void update();

	const void draw(Shader & shader, ivec2 playerChunk);
};

