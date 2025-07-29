#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm> // for fill
#include <cmath>
#include <iostream>
#include <memory>  // for unique ptr
#include <random>

#include <unordered_map>
#include <vector>

#include "block.h"
#include "arr3_hash.hpp"

using std::unordered_map;
using std::vector;

using glm::vec2;
using glm::vec3;

/*
A representation of a chunk of blocks 
has a world x, y (assuming no chunks stack on each other)
contains 16 x 128 x 16 blocks 
*/

static constexpr int CHUNK_MAX_X = 256;
static constexpr int CHUNK_MAX_Y = 16;
static constexpr int CHUNK_MAX_Z = 256;

static constexpr int HEIGHT_BASELINE = CHUNK_MAX_Y / 2;

class Chunk
{
private:
	std::unique_ptr<BlockType[]> blocks;

	unsigned int VAO, VBO, EBO;

	int worldx, worldy;

	vector<vec3> meshVertices;

	vector<int> meshIndices;

	void updateMesh();

	void addBlockMesh(vec3 coords, unordered_map<vec3, int, vec3Hash> & vertexToIndex);

	void addBlockVertex(vec3 coords, int index, unordered_map<vec3, int, vec3Hash>& vertexToIndex);

	void generate(uint32_t seed = UINT32_MAX);

public:
	Chunk(int worldx = 0, int worldy = 0);

	~Chunk();

	void draw();

	BlockType getBlock(vec3 coords);
};

