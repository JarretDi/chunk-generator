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

const void World::draw(Shader & shader, glm::vec2 playerChunk) {
	shader.use();
	for (const auto& entry : chunks) {
		const auto& coords = entry.first;
		const auto& chunk = entry.second;

		glm::vec3 chunkCoords = chunk->getModelCoords();

		if (glm::distance(chunkCoords, glm::vec3(playerChunk.x, 0.0f, playerChunk.y)) > renderDistance)
			continue;
	
		glm::mat4 model(1.0f);
		model = glm::scale(model, vec3(0.1));
		model = glm::translate(model, (float)CHUNK_MAX_X * chunkCoords);
		shader.setMat4("model", model);
		chunk->draw();
	}
}