#include "chunk.h"

Chunk::Chunk(int worldx, int worldy) : worldx(worldx), worldy(worldy) {
	std::memset(blocks, 1, sizeof(blocks));

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

BlockType Chunk::getBlock(vec3 coords) {
	if (coords.x >= CHUNK_MAX_X || coords.y >= CHUNK_MAX_Y || coords.z >= CHUNK_MAX_Z ||
		coords.x < 0 || coords.y < 0 || coords.z < 0) return BlockType::AIR;

	return blocks[(int)coords.x][(int)coords.y][(int)coords.z];
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