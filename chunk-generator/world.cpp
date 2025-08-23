#include "world.h"

World::World(uint32_t seed) : seed(seed) {
	if (seed == UINT32_MAX) {
		std::random_device rd;
		seed = rd();
	}

	loadChunks({ 0, 0 });
	update(2 * RENDER_DISTANCE * RENDER_DISTANCE);
}

void World::loadChunks(glm::ivec2 playerChunk) {
	for (int x = playerChunk.x - RENDER_DISTANCE / 2; x <= playerChunk.x + RENDER_DISTANCE / 2; x++) {
		for (int z = playerChunk.y - RENDER_DISTANCE / 2; z <= playerChunk.y + RENDER_DISTANCE / 2; z++) {
			glm::ivec2 coords(x, z);
			bool inWorld = chunks.find(coords) != chunks.end();
			bool inQueue = toLoadAdded.find(coords) != toLoadAdded.end();
			if (!inWorld && !inQueue) {
				int squaredDist = (playerChunk.x - coords.x) * (playerChunk.x - coords.x) + (playerChunk.y - coords.y) * (playerChunk.y - coords.y);
				toLoad.push({coords, squaredDist});
				toLoadAdded.insert(coords);
			}
		}
	}
}

void World::update(int numChunks) {
	for (int i = 0; i < numChunks; i++) {
		if (toLoad.empty()) return;
		ivec2 loadNow = toLoad.top().coords;
		toLoad.pop();

		if (chunks.find(loadNow) == chunks.end())
			chunks.emplace(loadNow, std::make_unique<Chunk>(seed, loadNow.x, loadNow.y));
		toLoadAdded.erase(loadNow);
	}
}

const void World::draw(Shader & shader, glm::ivec2 playerChunk) {
	shader.use();
	for (const auto& entry : chunks) {
		const auto& coords = entry.first;
		const auto& chunk = entry.second;

		glm::vec3 chunkCoords = chunk->getModelCoords();

		if (glm::distance(chunkCoords, glm::vec3(playerChunk.x, 0.0f, playerChunk.y)) > 0.5 * RENDER_DISTANCE * std::max(CHUNK_MAX_X, CHUNK_MAX_Z))
			continue;
	
		glm::mat4 model(1.0f);
		model = glm::translate(model, (float)CHUNK_MAX_X * chunkCoords);
		shader.setMat4("model", model);
		chunk->draw();
	}
}

BlockType World::getBlock(ivec3 worldPosition) const {
	// should be the only out of bounds check (world is theoretically infinite)
	if (worldPosition.y < 0 || worldPosition.y >= CHUNK_MAX_Y) return BlockType::AIR;

	// for caching to reduce number of map .at calls
	static Chunk* lastChunk = nullptr;
	static ivec2 lastChunkCoords;

	ivec2 chunkWorldCoords = {
		floor((float)worldPosition.x / CHUNK_MAX_X), 
		floor((float)worldPosition.z / CHUNK_MAX_Z)
	};

	ivec3 inChunkCoords = { 
		(worldPosition.x % CHUNK_MAX_X + CHUNK_MAX_X) % CHUNK_MAX_X,
		worldPosition.y,
		(worldPosition.z % CHUNK_MAX_Z + CHUNK_MAX_Z) % CHUNK_MAX_Z 
	};

	if (lastChunk == nullptr || lastChunkCoords != chunkWorldCoords) {
		lastChunk = chunks.at(chunkWorldCoords).get();
		lastChunkCoords = chunkWorldCoords;
	}

	return lastChunk->getBlock(inChunkCoords);
}