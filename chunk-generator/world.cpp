#include "world.h"

World::World(uint32_t seed, int renderDistance) : seed(seed) {
	if (seed == UINT32_MAX) {
		std::random_device rd;
		seed = rd();
	}

	for (int x = -renderDistance; x < renderDistance; x++) {
		for (int z = -renderDistance / 2; z < renderDistance / 2; z++) {
			chunks.emplace(vec2(x,z), std::make_unique<Chunk>(seed, x, z));
		}
	}
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

void World::update() {
	for (int i = 0; i < 3; i++) {
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

		if (glm::distance(chunkCoords, glm::vec3(playerChunk.x, 0.0f, playerChunk.y)) > RENDER_DISTANCE * std::max(CHUNK_MAX_X, CHUNK_MAX_Z))
			continue;
	
		glm::mat4 model(1.0f);
		model = glm::translate(model, (float)CHUNK_MAX_X * chunkCoords);
		shader.setMat4("model", model);
		chunk->draw();
	}
}