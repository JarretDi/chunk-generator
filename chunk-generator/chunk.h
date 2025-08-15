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
#include "../mesh.h"

using std::unordered_map;
using std::vector;

using glm::vec2;
using glm::vec3;

/*
A representation of a chunk of blocks 
has a world x, y (assuming no chunks stack on each other)
contains 16 x 128 x 16 blocks 
*/

static constexpr int CHUNK_MAX_X = 16;
static constexpr int CHUNK_MAX_Y = 64;
static constexpr int CHUNK_MAX_Z = 16;

static constexpr int HEIGHT_BASELINE = CHUNK_MAX_Y / 2;

// note: use the inverse of frequency for calculations
static constexpr int INITIAL_FREQUENCY = 64;
static constexpr int INITIAL_AMPLITUDE = 48;

class Chunk
{
private:
	std::unique_ptr<BlockType[]> blocks;

	unsigned int VAO, VBO;

	uint32_t seed;

	int worldx, worldz;

	vector<Vertex> meshVertices;

	void updateMesh();

	void addBlockMesh(vec3 coords);

	void addFace(vec3 coords, int index);

	void generate(int octaves = 1);

	void perlinNoise(float frequency, float amplitude, vector<int> & offsets);

	vec2 noiseHash(uint32_t seed, int x, int z);

public:
	Chunk(uint32_t seed, int worldx = 0, int worldz = 0);

	~Chunk();

	void draw() const;

	BlockType getBlock(vec3 coords) const;

	vec3 getModelCoords() const;

	Chunk(Chunk&&) noexcept = default;
	Chunk& operator=(Chunk&&) noexcept = default;
};