#include "world.h"

World::World(uint32_t seed, int renderDistance) : seed(seed) {
	if (seed == UINT32_MAX) {
		std::random_device rd;
		seed = rd();
	}

	for (int x = 0; x < renderDistance; x++) {
		for (int z = 0; z < renderDistance; z++) {
			chunks.emplace(vec2(x,z), std::make_unique<Chunk>(seed, x, z));
		}
	}
}

void World::loadChunks(glm::vec2 playerChunk) {
	for (int x = playerChunk.x - RENDER_DISTANCE; x <= playerChunk.x + RENDER_DISTANCE; x++) {
		for (int z = playerChunk.y - RENDER_DISTANCE; z <= playerChunk.y + RENDER_DISTANCE; z++) {
			if (x < 0 || z < 0) continue;
			glm::vec2 coords(x, z);
			auto it = chunks.find(coords);
			if (it == chunks.end()) {
				chunks.emplace(vec2(x, z), std::make_unique<Chunk>(seed, x, z));
			}
		}
	}
}

const void World::draw(Shader & shader, glm::vec2 playerChunk) {
	shader.use();
	for (const auto& entry : chunks) {
		const auto& coords = entry.first;
		const auto& chunk = entry.second;

		glm::vec3 chunkCoords = chunk->getModelCoords();

		if (glm::distance(chunkCoords, glm::vec3(playerChunk.x, 0.0f, playerChunk.y)) > RENDER_DISTANCE)
			continue;
	
		glm::mat4 model(1.0f);
		model = glm::scale(model, vec3(0.1));
		model = glm::translate(model, (float)CHUNK_MAX_X * chunkCoords);
		shader.setMat4("model", model);
		chunk->draw();
	}
}