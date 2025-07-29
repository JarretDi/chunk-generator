#include "chunk.h"

Chunk::Chunk(const vector<vec2>& noiseMap, int worldx, int worldz) : worldx(worldx), worldz(worldz) {
	blocks = std::make_unique<BlockType[]>(CHUNK_MAX_X * CHUNK_MAX_Y * CHUNK_MAX_Z);
	generate(noiseMap, 4);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	updateMesh();
}

Chunk::~Chunk() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

BlockType Chunk::getBlock(vec3 coords) {
	if (coords.x >= CHUNK_MAX_X || coords.y >= CHUNK_MAX_Y || coords.z >= CHUNK_MAX_Z ||
		coords.x < 0 || coords.y < 0 || coords.z < 0) return BlockType::AIR;

	return blocks[coords.x + CHUNK_MAX_X * (coords.y + CHUNK_MAX_Y * coords.z)];
}

vec3 Chunk::getCoords() {
	return vec3(worldx, 0, worldz);
}

void Chunk::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, meshIndices.size(), GL_UNSIGNED_INT, 0);
}

void Chunk::updateMesh() {
	meshVertices.clear();
	meshIndices.clear();
	unordered_map<vec3, int, vec3Hash> vertexToIndex;

	int currentIndex = 0;

	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int y = 0; y < CHUNK_MAX_Y; y++) {
			for (int z = 0; z < CHUNK_MAX_Z; z++) {
				addBlockMesh({ x, y, z }, vertexToIndex);
			}
		}
	}

	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * meshVertices.size(), meshVertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * meshIndices.size(), meshIndices.data(), GL_STATIC_DRAW);
}

void Chunk::addBlockMesh(vec3 coords, unordered_map<vec3, int, vec3Hash>& vertexToIndex) {
	if (getBlock(coords) == BlockType::AIR) return;

	// front face
	if (getBlock(coords + vec3(0, 0, 1)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 0, vertexToIndex); // these indexes come from cubeIndices
		addBlockVertex(coords, 1, vertexToIndex);
		addBlockVertex(coords, 2, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 0, vertexToIndex);
		addBlockVertex(coords, 2, vertexToIndex);
		addBlockVertex(coords, 3, vertexToIndex);
	}

	// top face
	if (getBlock(coords + vec3(0, 1, 0)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 1, vertexToIndex);
		addBlockVertex(coords, 5, vertexToIndex);
		addBlockVertex(coords, 6, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 1, vertexToIndex);
		addBlockVertex(coords, 6, vertexToIndex);
		addBlockVertex(coords, 2, vertexToIndex);
	}

	// right face
	if (getBlock(coords + vec3(1, 0, 0)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 2, vertexToIndex);
		addBlockVertex(coords, 3, vertexToIndex);
		addBlockVertex(coords, 7, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 2, vertexToIndex);
		addBlockVertex(coords, 7, vertexToIndex);
		addBlockVertex(coords, 6, vertexToIndex);
	}

	// bottom face
	if (getBlock(coords + vec3(0, -1, 0)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 0, vertexToIndex);
		addBlockVertex(coords, 4, vertexToIndex);
		addBlockVertex(coords, 7, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 0, vertexToIndex);
		addBlockVertex(coords, 7, vertexToIndex);
		addBlockVertex(coords, 3, vertexToIndex);
	}

	// left face
	if (getBlock(coords + vec3(-1, 0, 0)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 0, vertexToIndex);
		addBlockVertex(coords, 1, vertexToIndex);
		addBlockVertex(coords, 5, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 0, vertexToIndex);
		addBlockVertex(coords, 5, vertexToIndex);
		addBlockVertex(coords, 4, vertexToIndex);
	}

	// back face
	if (getBlock(coords + vec3(0, 0, -1)) == BlockType::AIR) {
		// first triangle
		addBlockVertex(coords, 4, vertexToIndex);
		addBlockVertex(coords, 5, vertexToIndex);
		addBlockVertex(coords, 6, vertexToIndex);

		// second triangle
		addBlockVertex(coords, 4, vertexToIndex);
		addBlockVertex(coords, 6, vertexToIndex);
		addBlockVertex(coords, 7, vertexToIndex);
	}
}

void Chunk::addBlockVertex(vec3 coords, int index, unordered_map<vec3, int, vec3Hash>& vertexToIndex) {
	vec3 vertex = cubeVertices[index] + coords;
	if (vertexToIndex.find(vertex) == vertexToIndex.end()) {
		meshVertices.push_back(vertex);
		meshIndices.push_back(meshVertices.size() - 1);
		vertexToIndex[vertex] = meshVertices.size() - 1;
	}
	else {
		meshIndices.push_back(vertexToIndex[vertex]);
	}
}

void Chunk::generate(const vector<vec2>& noiseMap, int octaves) {
	// go through again and assign a height offset
	vector<int> offsets(CHUNK_MAX_X * CHUNK_MAX_Z);

	float frequency = INITIAL_FREQUENCY;
	int amplitude = INITIAL_AMPLITUDE;

	for (int i = 0; i < octaves; i++) {
		perlinNoise(frequency, amplitude, noiseMap, offsets);
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

void Chunk::perlinNoise(float frequency, float amplitude, const vector<vec2>& noiseMap, vector<int>& offsets) {
	for (int x = 0; x < CHUNK_MAX_X; x++) {
		for (int z = 0; z < CHUNK_MAX_Z; z++) {
			vec2 samplePoint = 1.0f / frequency * (vec2(x, z) + vec2(worldx * CHUNK_MAX_X, worldz * CHUNK_MAX_Z));

			// get the four corners
			int x0 = floor(samplePoint.x);
			int x1 = x0 + 1;
			int z0 = floor(samplePoint.y);
			int z1 = z0 + 1;

			/*	ul   ur
				 +---+    ^ z
				 | · |    |
				 +---+     --> x
				bl   br  */

				// find the dot product between its displacement between corners and random vectors
			float bl = glm::dot(samplePoint - vec2(x0, z0), noiseMap[x0 + INITIAL_FREQUENCY * z0]);
			float br = glm::dot(samplePoint - vec2(x1, z0), noiseMap[x1 + INITIAL_FREQUENCY * z0]);
			float ul = glm::dot(samplePoint - vec2(x0, z1), noiseMap[x0 + INITIAL_FREQUENCY * z1]);
			float ur = glm::dot(samplePoint - vec2(x1, z1), noiseMap[x1 + INITIAL_FREQUENCY * z1]);

			// interpolate between them
			float blerp = glm::mix(bl, br, samplePoint.x - floor(samplePoint.x));
			float ulerp = glm::mix(ul, ur, samplePoint.x - floor(samplePoint.x));

			float bilerp = glm::mix(blerp, ulerp, samplePoint.y - floor(samplePoint.y));
			int heightOffset = bilerp * amplitude;
			offsets[x + CHUNK_MAX_X * z] += heightOffset;
		}
	}
}