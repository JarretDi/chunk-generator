#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "chunk-generator/shader.h"

using std::string;
using std::vector;

struct Vertex
{
	glm::vec3 coords;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

