#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstring>   // for memset
#include <algorithm> // for copy

#include <vector>
#include <array>
#include <unordered_map>

#include "block_types.h"
#include "arr3_hash.hpp"

using std::vector;
using std::array;
using std::unordered_map;

/*
A representation of a chunk of blocks 
has a world x, y (assuming no chunks stack on each other)
contains 16 x 16 x 128 blocks 
*/

constexpr int CHUNK_MAX_X = 16;
constexpr int CHUNK_MAX_Y = 16;
constexpr int CHUNK_MAX_Z = 128;

class Chunk
{
private:
	BlockType blocks[CHUNK_MAX_X][CHUNK_MAX_Y][CHUNK_MAX_Z];

	unsigned int VAO, VBO, EBO;

	int worldx, worldy;

	vector<vec3> meshVertices;

	vector<int> meshIndices;

	void updateMesh();

	void addBlockMesh(vec3 coords, unordered_map<vec3, int, vec3Hash> & vertexToIndex);

	void addBlockVertex(vec3 coords, int index, unordered_map<vec3, int, vec3Hash>& vertexToIndex);

public:
	Chunk(int worldx = 0, int worldy = 0);

	void draw();

	BlockType getBlock(vec3 coords);
};

