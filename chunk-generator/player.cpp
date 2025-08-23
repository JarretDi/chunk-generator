#include "player.h"

bool Player::selectBlock(World & world) {
	vec3 start = camera.Position;
	vec3 dir = glm::normalize(camera.Front);

	ivec3 current = glm::floor(start);

	int stepx = dir.x > 0 ? 1 : -1;
	int stepy = dir.y > 0 ? 1 : -1;
	int stepz = dir.z > 0 ? 1 : -1;

	int nextVoxelBoundaryX = dir.x > 0 ? floor(start.x) + 1.0f : ceil(start.x) - 1.0f;
	int nextVoxelBoundaryY = dir.y > 0 ? floor(start.y) + 1.0f : ceil(start.y) - 1.0f;
	int nextVoxelBoundaryZ = dir.z > 0 ? floor(start.z) + 1.0f : ceil(start.z) - 1.0f;

	float tMaxX = dir.x != 0 ? (nextVoxelBoundaryX - start.x) / dir.x : INFINITY;
	float tMaxY = dir.y != 0 ? (nextVoxelBoundaryY - start.y) / dir.y : INFINITY;
	float tMaxZ = dir.z != 0 ? (nextVoxelBoundaryZ - start.z) / dir.z : INFINITY;

	float tDeltaX = abs(1 / dir.x);
	float tDeltaY = abs(1 / dir.y);
	float tDeltaZ = abs(1 / dir.z);

	ivec3 normal;
	float distanceTraveled = 0;

	while (distanceTraveled <= MAX_SELECT_DISTANCE) {
		if (world.getBlock(current) != BlockType::AIR) {
			selected.hit = true;
			selected.coords = current;
			selected.normal = normal;
			//std::cout << "BLOCK SELECTED AT (" << current.x << ", " << current.y << ", " << current.z << ")\n";
			return true;
		}

		if (tMaxX <= tMaxY && tMaxX <= tMaxZ) {
			current += ivec3(stepx, 0, 0);
			tMaxX += tDeltaX;
			normal = ivec3(-stepx, 0, 0);
			distanceTraveled = tMaxX;
		} else if (tMaxY <= tMaxZ) {
			current += ivec3(0, stepy, 0);
			tMaxY += tDeltaY;
			normal = ivec3(0, -stepy, 0);
			distanceTraveled = tMaxY;
		} else {
			current += ivec3(0, 0, stepz);
			tMaxZ += tDeltaZ;
			normal = ivec3(0, 0, -stepz);
			distanceTraveled = tMaxZ;
		}
	}

	return false;
}