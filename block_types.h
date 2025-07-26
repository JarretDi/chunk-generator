#pragma once

enum class BlockType {
	AIR = 0,
	GRASS = 1,
	STONE = 2,
};

constexpr float cubeVertices[] = {
	// front face
	0, 0, 1, // bottom left
	0, 1, 1, // top left
	1, 1, 1, // top right
	1, 0, 1, // bottom right
	// back face
	0, 0, 0,
	0, 1, 0,
	1, 1, 0,
	1, 0, 0,
};

constexpr int cubeIndices[] = {
	// front face
	0, 1, 2,
	0, 2, 3,
	// top face
	1, 5, 6,
	1, 6, 2,
	// right face
	2, 3, 7,
	2, 7, 6,
	// bottom face
	0, 4, 7,
	0, 7, 3,
	// left face
	0, 1, 5,
	0, 5, 4,
	// back face
	4, 5, 6,
	4, 6, 7
};

