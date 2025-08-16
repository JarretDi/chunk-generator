#include "chunk.h"

Chunk::Chunk(uint32_t seed, int worldx, int worldz) : seed(seed), worldx(worldx), worldz(worldz) {
	blocks.resize(CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z);
	generate(6); // for perlin noise

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);

	updateMesh();
	glBindVertexArray(0);
}

Chunk::~Chunk() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

BlockType Chunk::getBlock(vec3 coords) const {
	if (coords.x >= CHUNK_MAX_X || coords.y >= CHUNK_MAX_Y || coords.z >= CHUNK_MAX_Z ||
		coords.x < 0 || coords.y < 0 || coords.z < 0) return BlockType::AIR;

	return blocks[coords.x + CHUNK_MAX_X * (coords.y + CHUNK_MAX_Y * coords.z)];
}

vec3 Chunk::getModelCoords() const {
	return vec3(worldx, 0, worldz);
}

void Chunk::draw() const {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, meshVertices.size());
}

void Chunk::updateMesh() {
	meshVertices.clear();
	meshVertices.reserve(CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z * 6 * 4 / 2);

	int currentIndex = 0;

	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int y = 0; y < CHUNK_MAX_Y; y++) {
			for (int z = 0; z < CHUNK_MAX_Z; z++) {
				addBlockMesh({ x, y, z });
			}
		}
	}

	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshVertices.size(), meshVertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Chunk::addBlockMesh(vec3 coords) {
	if (getBlock(coords) == BlockType::AIR) return;

	// front face
	if (getBlock(coords + vec3(0, 0, 1)) == BlockType::AIR) addFace(coords, 0);

	// top face
	if (getBlock(coords + vec3(0, 1, 0)) == BlockType::AIR) addFace(coords, 1);

	// right face
	if (getBlock(coords + vec3(1, 0, 0)) == BlockType::AIR) addFace(coords, 2);

	// bottom face
	if (getBlock(coords + vec3(0, -1, 0)) == BlockType::AIR) addFace(coords, 3);

	// left face
	if (getBlock(coords + vec3(-1, 0, 0)) == BlockType::AIR) addFace(coords, 4);

	// back face
	if (getBlock(coords + vec3(0, 0, -1)) == BlockType::AIR) addFace(coords, 5);
}

void Chunk::addFace(vec3 coords, int index) {
	int start = index * 6;
	for (int i = 0; i < 6; i++) {
		Vertex vertex = cubeVertices[start + i];
		vertex.coords += coords;
		meshVertices.push_back(vertex);
	}
}

void Chunk::generate(int octaves) {
	// go through again and assign a height offset
	vector<int> offsets(CHUNK_MAX_X * CHUNK_MAX_Z);

	float frequency = INITIAL_FREQUENCY;
	int amplitude = INITIAL_AMPLITUDE;

	for (int i = 0; i < octaves; i++) {
		perlinNoise(frequency, amplitude, offsets);
		frequency /= 2;
		amplitude /= 2;
	}

	// go through each (x, z) and set the height, using a baseline height
	// then, fills air above each height, and grass below
	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int z = 0; z < CHUNK_MAX_Z; z++) {
			int height = HEIGHT_BASELINE + offsets[x + CHUNK_MAX_X * z];

			for (int y = 0; y < height; y++) {
				blocks[x + CHUNK_MAX_X * (y + CHUNK_MAX_Y * z)] = BlockType::GRASS;
			}
		}
	}
}

void Chunk::perlinNoise(float frequency, float amplitude, vector<int>& offsets) {
	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int z = 0; z < CHUNK_MAX_Z; z++) {
			vec2 samplePoint = 1.0f / frequency * (vec2(x, z) + vec2(worldx * CHUNK_MAX_X, worldz * CHUNK_MAX_Z));

			// get the four corners
			unsigned long x0 = floor(samplePoint.x);
			unsigned long x1 = x0 + 1;
			unsigned long z0 = floor(samplePoint.y);
			unsigned long z1 = z0 + 1;

			/*	ul   ur
				 +---+    ^ z
				 | · |    |
				 +---+     --> x
				bl   br  */

			// find the dot product between its displacement between corners and random vectors
			float bl = glm::dot(samplePoint - vec2(x0, z0), noiseHash(seed, x0, z0));
			float br = glm::dot(samplePoint - vec2(x1, z0), noiseHash(seed, x1, z0));
			float ul = glm::dot(samplePoint - vec2(x0, z1), noiseHash(seed, x0, z1));
			float ur = glm::dot(samplePoint - vec2(x1, z1), noiseHash(seed, x1, z1));

			// compute smooth interpolation factors for x and y
			float tx = glm::smoothstep(0.0f, 1.0f, glm::fract(samplePoint.x));
			float ty = glm::smoothstep(0.0f, 1.0f, glm::fract(samplePoint.y));
			
			// interpolate between them
			float blerp = glm::mix(bl, br, tx);
			float ulerp = glm::mix(ul, ur, tx);

			float bilerp = glm::mix(blerp, ulerp, ty);
			int heightOffset = bilerp * amplitude;
			offsets[x + CHUNK_MAX_X * z] += heightOffset;
		}
	}
}

vec2 Chunk::noiseHash(uint32_t seed, int x, int z) {
	// Hash to 32-bit
	seed ^= static_cast<uint32_t>(x) * 0x9E3779B1u;
	seed ^= static_cast<uint32_t>(z) * 0x85EBCA77u;
	seed ^= (seed >> 16);
	seed *= 0x27D4EB2Du;
	seed ^= (seed >> 15);

	// Convert to [0,1)
	float normalized = static_cast<float>(seed) / static_cast<float>(UINT32_MAX);

	// Map to [0, 2pi)
	float angle = normalized * 2.0f * glm::pi<float>();

	// Convert to unit vector
	return glm::vec2(std::cos(angle), std::sin(angle));
}