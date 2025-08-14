#include "world.h"

World::World(uint32_t seed, int numChunks) : seed(seed) {
	// setup the random engine
	if (seed == UINT32_MAX) {
		std::random_device rd;
		seed = rd();
	}

	for (int x = 0; x < numChunks; x++) {
		for (int z = 0; z < numChunks; z++) {
			chunks.emplace_back(std::make_unique<Chunk>(seed, x, z));
		}
	}
}

void World::draw(Shader & shader) {
	shader.use();
	for (int i = 0; i < chunks.size(); i++) {
		glm::mat4 model(1.0f);
		model = glm::scale(model, vec3(0.1));
		model = glm::translate(model, (float)CHUNK_MAX_X * chunks[i]->getCoords());
		shader.setMat4("model", model);
		chunks[i]->draw();
	}
}