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

void World::loadChunks(glm::ivec2 playerChunk) {
	for (int x = playerChunk.x - RENDER_DISTANCE; x <= playerChunk.x + RENDER_DISTANCE; x++) {
		for (int z = playerChunk.y - RENDER_DISTANCE; z <= playerChunk.y + RENDER_DISTANCE; z++) {
			if (x < 0 || z < 0) continue;
			glm::vec2 coords(x, z);
			auto it = chunks.find(coords);
			if (it == chunks.end()) {
				float dist = glm::distance((glm::vec2)playerChunk, coords);
				toLoad.push({coords, dist});
			}
		}
	}
}

void World::update() {
	for (int i = 0; i < 3; i++) {
		if (toLoad.empty()) return;
		vec2 loadNow = toLoad.top().coords;
		toLoad.pop();

		if (chunks.find(loadNow) == chunks.end())
			chunks.emplace(loadNow, std::make_unique<Chunk>(seed, loadNow.x, loadNow.y));
	}
}

const void World::draw(Shader & shader, glm::ivec2 playerChunk) {
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