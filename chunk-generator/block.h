#pragma once

#include <glm/glm.hpp>

#include <bitset>
#include <string>

#include "mesh.h"

using glm::vec3;

namespace Block {
    using BlockType = uint8_t;

    // Maintain the BlockTag and tagToString together
    enum class BlockTag : uint8_t {
        Invisible,
        Transparent,

        COUNT, // used to easily get the length of the enum
    };

    inline std::string tagToString(BlockTag tag) {
        switch (tag) {
            case BlockTag::Invisible:   
                return "Invisible";
            case BlockTag::Transparent: 
                return "Transparent";
            default:                    
                return "Unknown";
        }
    }

    struct BlockDef {
        std::string name;
        int textureIndices[6]{};
        std::bitset<static_cast<size_t>(BlockTag::COUNT)> tags;

        inline bool hasTag(BlockTag tag) const {
            return tags.test(static_cast<size_t>(tag));
        }
    };

    class BlockRegistry { // Singleton
    public:
        static BlockRegistry& getInstance() {
            static BlockRegistry instance;
            return instance;
        }

        inline void registerBlock(BlockDef def) {
            blockDefs[uniqueID++] = std::move(def);
        }

        inline const BlockDef& get(BlockType type) const {
            return blockDefs.at(type);
        }

        void testRegister();

    private:
        int uniqueID = 0;
        std::vector<BlockDef> blockDefs;
        BlockRegistry() = default;
    };

    // Cube vertices, 6 faces × 2 triangles × 3 vertices = 36 vertices
    const Vertex cubeVertices[] = {
        // --- Front face ---
        {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0,0}},
        {{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1,0}},
        {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1,1}},
        {{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1,1}},
        {{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {0,1}},
        {{-0.5f, -0.5f,  0.5f}, {0, 0, 1}, {0,0}},

        // --- Back face ---
        {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0,0}},
        {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1,0}},
        {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {1,1}},
        {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {1,1}},
        {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0,1}},
        {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0,0}},

        // --- Left face ---
        {{-0.5f, -0.5f, -0.5f}, {-1,0,0}, {0,0}},
        {{-0.5f, -0.5f,  0.5f}, {-1,0,0}, {1,0}},
        {{-0.5f,  0.5f,  0.5f}, {-1,0,0}, {1,1}},
        {{-0.5f,  0.5f,  0.5f}, {-1,0,0}, {1,1}},
        {{-0.5f,  0.5f, -0.5f}, {-1,0,0}, {0,1}},
        {{-0.5f, -0.5f, -0.5f}, {-1,0,0}, {0,0}},

        // --- Right face ---
        {{ 0.5f, -0.5f,  0.5f}, {1,0,0}, {0,0}},
        {{ 0.5f, -0.5f, -0.5f}, {1,0,0}, {1,0}},
        {{ 0.5f,  0.5f, -0.5f}, {1,0,0}, {1,1}},
        {{ 0.5f,  0.5f, -0.5f}, {1,0,0}, {1,1}},
        {{ 0.5f,  0.5f,  0.5f}, {1,0,0}, {0,1}},
        {{ 0.5f, -0.5f,  0.5f}, {1,0,0}, {0,0}},

        // --- Top face ---
        {{-0.5f, 0.5f,  0.5f}, {0,1,0}, {0,0}},
        {{ 0.5f, 0.5f,  0.5f}, {0,1,0}, {1,0}},
        {{ 0.5f, 0.5f, -0.5f}, {0,1,0}, {1,1}},
        {{ 0.5f, 0.5f, -0.5f}, {0,1,0}, {1,1}},
        {{-0.5f, 0.5f, -0.5f}, {0,1,0}, {0,1}},
        {{-0.5f, 0.5f,  0.5f}, {0,1,0}, {0,0}},

        // --- Bottom face ---
        {{-0.5f, -0.5f, -0.5f}, {0,-1,0}, {0,0}},
        {{ 0.5f, -0.5f, -0.5f}, {0,-1,0}, {1,0}},
        {{ 0.5f, -0.5f,  0.5f}, {0,-1,0}, {1,1}},
        {{ 0.5f, -0.5f,  0.5f}, {0,-1,0}, {1,1}},
        {{-0.5f, -0.5f,  0.5f}, {0,-1,0}, {0,1}},
        {{-0.5f, -0.5f, -0.5f}, {0,-1,0}, {0,0}}
    };
}