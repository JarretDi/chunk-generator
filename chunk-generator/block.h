#pragma once

#include <glm/glm.hpp>

#include <bitset>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

#include "mesh.h"

using glm::vec3;

namespace Block {
    using BlockType = uint8_t;

    // Maintain the BlockTag and tagToString together
    enum class BlockTag : uint8_t {
        Air,
        Transparent,

        COUNT, // used to easily get the length of the enum
    };

    inline std::string tagToString(BlockTag tag) {
        switch (tag) {
            case BlockTag::Air:   
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

        // Constructor to allow more convenient syntax
        BlockDef(
            std::string name, 
            std::initializer_list<int> textures, 
            std::initializer_list<BlockTag> tagList)
            : name(std::move(name)) {
            assert(textures.size() == 6);
            int i = 0;
            for (auto t : textures) {
                textureIndices[i++] = t;
            }

            for (auto t : tagList) {
                tags.set(static_cast<size_t>(t));
            }
        }

        inline bool hasTag(BlockTag tag) const {
            return tags.test(static_cast<size_t>(tag));
        }

        friend std::ostream& operator<<(std::ostream& os, const BlockDef& block) {
            os << "Block: " << block.name << "\n";

            os << "Textures: ";
            for (int i = 0; i < 6; ++i) os << block.textureIndices[i] << " ";
            os << "\n";

            os << "Tags: ";
            bool first = true;
            for (size_t i = 0; i < static_cast<size_t>(BlockTag::COUNT); ++i) {
                if (block.tags.test(i)) {
                    if (!first) os << ", ";
                    os << tagToString(static_cast<BlockTag>(i));
                    first = false;
                }
            }
            os << "\n";
            return os;
        }
    };

    class BlockRegistry { // Singleton
    public:
        static BlockRegistry& getInstance() {
            static BlockRegistry instance;
            return instance;
        }

        inline void registerBlock(BlockDef def) {
            blockDefs.push_back(std::move(def));
            uniqueID++;
        }

        inline const BlockDef& getDef(BlockType type) const {
            return blockDefs.at(type);
        }

        inline void print() const {
            for (const auto& def : blockDefs) {
                std::cout << def << "\n";
            }
        }

        void testRegister();

    private:
        BlockType uniqueID = 0;
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