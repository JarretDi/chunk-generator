#include <array>
#include <functional>
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::ivec2;
using glm::ivec3;

struct vec2Hash {
    size_t operator()(const ivec2 vec) const {
        size_t h = 0;
        // Combine std::hash<float> results with bit mixing
        h ^= std::hash<float>{}(vec.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<float>{}(vec.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};

struct vec3Hash {
    size_t operator()(const ivec3 vec) const {
        size_t h = 0;
        // Combine std::hash<float> results with bit mixing
        h ^= std::hash<float>{}(vec.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<float>{}(vec.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= std::hash<float>{}(vec.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
        return h;
    }
};