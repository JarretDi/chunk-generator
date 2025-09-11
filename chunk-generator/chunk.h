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
#include "mesh.h"

using std::unordered_map;
using std::vector;

using glm::vec2;
using glm::vec3;
using glm::ivec3;

/*
A representation of a chunk of blocks 
has a world x, y (assuming no chunks stack on each other)
contains 16 x 128 x 16 blocks 
*/

static constexpr int CHUNK_MAX_X = 32;
static constexpr int CHUNK_MAX_Y = 48;
static constexpr int CHUNK_MAX_Z = 32;

static constexpr int HEIGHT_BASELINE = CHUNK_MAX_Y / 2;

// note: use the inverse of frequency for calculations
static constexpr int INITIAL_FREQUENCY = 64;
static constexpr int INITIAL_AMPLITUDE = 32;

class Chunk
{
private:
	vector<Block::BlockType> blocks;

	unsigned int VAO, VBO;

	uint32_t seed;

	int worldx, worldz;

	vector<Vertex> meshVertices;

	void updateMesh();

	void addBlockMesh(ivec3 coords);

	void addFace(ivec3 coords, int index);

	void generate(int octaves = 1);

	void perlinNoise(float frequency, float amplitude, vector<int> & offsets);

	vec2 noiseHash(uint32_t seed, int x, int z);

public:
	Chunk(uint32_t seed, int worldx = 0, int worldz = 0);

	~Chunk();

	void draw() const;

	inline const Block::BlockDef& getBlockDef(ivec3 coords) const {
		using Block::BlockRegistry;

		const bool outOfBounds =
			coords.x < 0 || coords.x >= CHUNK_MAX_X ||
			coords.y < 0 || coords.y >= CHUNK_MAX_Y ||
			coords.z < 0 || coords.z >= CHUNK_MAX_Z;

		if (outOfBounds) 
			return BlockRegistry::getInstance().getDef(0);

		int index = coords.x + CHUNK_MAX_X * (coords.y + CHUNK_MAX_Y * coords.z);
		Block::BlockType type = blocks[index];

		return BlockRegistry::getInstance().getDef(type);
	}

	inline bool removeBlock(ivec3 coords) {
		const bool outOfBounds =
			coords.x < 0 || coords.x >= CHUNK_MAX_X ||
			coords.y < 0 || coords.y >= CHUNK_MAX_Y ||
			coords.z < 0 || coords.z >= CHUNK_MAX_Z;

		if (outOfBounds)
			return false;


		int index = coords.x + CHUNK_MAX_X * (coords.y + CHUNK_MAX_Y * coords.z);
		std::cout << "Block ID before removal: " << +blocks[index] << std::endl;
		std::cout << "BlockDef: " << getBlockDef(coords).name << std::endl;

		blocks[index] = 0;

		std::cout << "Block ID after removal: " << +blocks[index] << std::endl;
		std::cout << "BlockDef: " << getBlockDef(coords).name << std::endl;

		updateMesh();

		return true;
	}

	inline ivec3 getModelCoords() const {
		return ivec3(worldx, 0, worldz);
	}
};