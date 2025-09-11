#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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
	World(uint32_t seed = UINT32_MAX);

	void loadChunks(ivec2 playerChunk);

	void update(int numChunks = 1);

	const void draw(Shader & shader, ivec2 playerChunk);

	// returns a tuple where the first is chunk coords
	// econd is in chunk block coords
	std::pair<ivec2, ivec3> findChunk(ivec3 worldPosition) const;

	Block::BlockDef getBlockDef(ivec3 worldPosition) const;

	bool removeBlockAt(ivec3 worldPosition);

	Block::BlockType placeBlockAt(ivec3 worldPosition, Block::BlockType type);
};

