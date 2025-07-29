#include "world.h"

World::World(uint32_t seed, int numChunks) : seed(seed) {
	// setup the random engine
	if (seed == UINT32_MAX) {
		std::random_device rd;
		seed = rd();
	}

	rng = std::mt19937(seed);
	dist = std::uniform_real_distribution<float>(0.0, 2.0 * glm::pi<float>());

	// setup the noisemap
	// note: this is a flattened 2d vector (of vectors, but a different one)
	noiseMap = vector<vec2>(INITIAL_FREQUENCY * INITIAL_FREQUENCY);

	// give each noise vertex a unit vector
	for (int x = 0; x < INITIAL_FREQUENCY; x++) {
		for (int z = 0; z < INITIAL_FREQUENCY; z++) {
			float angle = dist(rng);
			noiseMap[x + INITIAL_FREQUENCY * z] = vec2(sin(angle), cos(angle));
		}
	}

	for (int x = 0; x < numChunks; x++) {
		for (int z = 0; z < numChunks; z++) {
			chunks.emplace_back(std::make_unique<Chunk>(noiseMap, x, z));
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