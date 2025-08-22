#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "camera.h"
#include "world.h"

using glm::ivec3;

constexpr int MAX_SELECT_DISTANCE = 8;

class Player
{
private:
	Camera camera;

	struct Raycast {
		bool hit;
		ivec3 coords;
		ivec3 normal;
	};

	Raycast selected{false, vec3(0), vec3(0)};

public:
	Player(ivec3 start = ivec3(0, 32, 0));

	inline Camera & getCamera() {
		return camera;
	}

	ivec2 getChunkCoords() const;

	// Casts a ray from center of screen (camera) and returns true if it hits a block within MAX_SELECT_DISTANCE
	// modifies the selected Raycast field with this information
	bool selectBlock(World & world);

	inline ivec3 getSelected() const {
		return selected.coords;
	}
};

