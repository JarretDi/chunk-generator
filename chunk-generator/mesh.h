#pragma once

#include <glm/glm.hpp>


#include "shader.h"

using std::string;
using glm::ivec3;

struct Vertex
{
	glm::vec3 coords;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Vertex2
{
	// Heavy compression of vertex data, organized as:
	// 000000 00000 00 000 00000 000000 00000
	// flags  typ  cnr fac z     y      x
	// assuming x, z < 32, y < 64
	// normal is a face 0-5
	// corner is 0-3 for each square corner (0 top-left, 1, bot-left...)
	// <64 block types
	// flags unused for now
	uint32_t data;

	Vertex2(ivec3 pos, int face, int corner, uint8_t type) {
		assert(pos.x < 32 && pos.y < 64 && pos.z < 32);
		assert(face < 6);
		assert(corner < 4);
		assert(type < 64);

		data = 0;
		data |= (pos.x & 0x1F) << 0;
		data |= (pos.y & 0x3F) << 5;
		data |= (pos.z & 0x1F) << 11;

		data |= (face & 0x7) << 16;
		data |= (corner & 0x3) << 19;
		data |= (type & 0x3F) << 21;
	}

	ivec3 getPos() const {
		int x = (data >> 0) & 0x1F;
		int y = (data >> 5) & 0x3F;
		int z = (data >> 11) & 0x1F;
		return ivec3(x, y, z);
	}

	// Add a chunk/world offset to the vertex
	Vertex2& operator+=(const ivec3& offset) {
		ivec3 p = getPos();
		p += offset;

		data &= ~65535; // clears old x, y, z
		// Re-pack into data
		data |= (p.x & 0x1F) << 0;
		data |= (p.y & 0x3F) << 5;
		data |= (p.z & 0x1F) << 11;

		return *this;
	}
};

