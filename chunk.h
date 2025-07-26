#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstring>   // for memset
#include <algorithm> // for copy

#include "block_types.h"

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
	unsigned int VAO;

	int worldx, worldy;

public:
	Chunk(unsigned int VAO, int worldx = 0, int worldy = 0);

	void draw();

	BlockType blocks[CHUNK_MAX_X][CHUNK_MAX_Y][CHUNK_MAX_Z];
};

